-- SyncFriends for Lua4RS: keeps the friend lists of your own nodes in sync over
-- (distant) mail. Every message carries the sender's complete friend list
-- ("state", not tickets), so late or duplicate mails do no harm.
--
-- Triggers: mail_received (apply lists), friends_changed + every hour (send
-- when the own list changed, at least once a day), startup.
-- Needs sandbox-write: reads mail and invites (sensitive), adds friends (write).
--
-- Configuration. It is part of the approved code: changing it needs a new approval.
local SEND_AS        = "<own identity id>"         -- own identity the mails come from
local SEND_TO        = { "<partner identity id>" } -- identities that get my friend list
local ACCEPT_FROM    = { "<partner identity id>" } -- identities that may change my friends
local REMOVE_FRIENDS = false          -- also remove friends the partner removed
local RESEND_SECONDS = 24 * 3600       -- send the list at least this often
local DRY_RUN        = true            -- only log which friends would be added/removed

local SUBJECT  = "[Lua4RS SyncFriends]"
local PROTOCOL = 1

local function contains(list, value)
    for _, v in ipairs(list) do if v == value then return true end end
    return false
end

-- Every change to friends goes through here: with DRY_RUN (and in test runs)
-- it is only logged.
local dryRun = DRY_RUN
local selftest = false -- run with simulated args (read-only): never sends
local function act(what, f, ...)
    if dryRun then print("would " .. what) return true end
    local ok = f(...)
    print((ok and "" or "FAILED: ") .. what)
    return ok
end

-- Own friend list: friends[pgpId] = { pgpId, name, locations = { {sslId, name, invite} } }
local function ownFriends()
    local byPgp, order = {}, {}
    for _, ssl in ipairs(peers.getFriendList() or {}) do
        local d = peers.getPeerDetails(ssl)
        if d and d.gpg_id then
            local f = byPgp[d.gpg_id]
            if not f then
                f = { pgpId = d.gpg_id, name = d.name or "", locations = jsonapi.array() }
                byPgp[d.gpg_id] = f
                order[#order + 1] = d.gpg_id
            end
            local invite = peers.getShortInvite(ssl)
            if invite then
                f.locations[#f.locations + 1] = { sslId = ssl, name = d.location or "", invite = invite }
            end
        end
    end
    table.sort(order)
    local list = jsonapi.array()
    for _, pgp in ipairs(order) do list[#list + 1] = byPgp[pgp] end
    return list
end

-- Order-independent summary of a list, to notice changes.
local function fingerprint(friends)
    local keys = {}
    for _, f in ipairs(friends) do
        for _, l in ipairs(f.locations) do keys[#keys + 1] = f.pgpId .. "/" .. l.sslId end
    end
    table.sort(keys)
    return table.concat(keys, ",")
end

local function send(message)
    message.syncfriends = PROTOCOL
    message.sent = os.time()
    local seq = (store.get("seq") or 0) + 1
    message.seq = seq
    local body = jsonapi.encode(message)
    local sent, err = mail.sendMail(SEND_AS, SEND_TO, SUBJECT, body)
    print(("sent %s #%d (%d bytes) to %d of %d: %s"):format(message.type, seq, #body, sent, #SEND_TO, err or "ok"))
    if sent > 0 then store.set("seq", seq) end
    return sent > 0
end

local function sendList(force)
    local friends = ownFriends()
    local fp = fingerprint(friends)
    local last = store.get("lastSent") or {}
    if not force and last.fingerprint == fp and os.time() - (last.time or 0) < RESEND_SECONDS then
        return
    end
    if send({ type = "list", friends = friends }) then
        store.set("lastSent", { fingerprint = fp, time = os.time() })
    end
end

-- Applies a partner's list. Returns nothing; prints every action.
local function applyList(from, message)
    local mine = {}
    for _, ssl in ipairs(peers.getFriendList() or {}) do mine[ssl] = true end
    local ownPgp = peers.getGPGOwnId()

    local now = {}
    for _, f in ipairs(message.friends or {}) do
        now[f.pgpId] = true
        if f.pgpId ~= ownPgp then
            for _, l in ipairs(f.locations or {}) do
                if not mine[l.sslId] and l.sslId ~= peers.getOwnId() then
                    act(("add %s (%s / %s)"):format(l.sslId, f.name, l.name), peers.acceptInvite, l.invite)
                end
            end
        end
    end

    local key = "partner:" .. from
    local previous = store.get(key) or {}
    if REMOVE_FRIENDS then
        for _, pgp in ipairs(previous.pgpIds or {}) do
            if not now[pgp] and pgp ~= ownPgp and peers.isGPGAccepted(pgp) then
                act(("remove %s (%s)"):format(pgp, peers.getGPGName(pgp) or "?"), peers.removeFriend, pgp)
            end
        end
    end
    local pgpIds = {}
    for pgp in pairs(now) do pgpIds[#pgpIds + 1] = pgp end
    table.sort(pgpIds)
    store.set(key, { seq = message.seq, pgpIds = pgpIds, time = os.time() })
    -- First list from this partner: answer with ours, so both sides know each other.
    if not previous.seq then
        if selftest then print("would answer with own list") else sendList(true) end
    end
end

-- Checks and applies one message (decoded JSON) from `from`.
-- RetroShare authenticates the sender before a mail arrives (distant mail: the
-- author's signature is checked by GxsTrans/GRouter), so `from` can be trusted;
-- it must be an identity (not a node) listed in ACCEPT_FROM.
local function handle(from, fromIdentity, message)
    if not fromIdentity then print("ignored: sender " .. from .. " is not an identity") return end
    if not contains(ACCEPT_FROM, from) then print("ignored: sender " .. from .. " is not in ACCEPT_FROM") return end
    if type(message) ~= "table" or message.syncfriends ~= PROTOCOL then print("ignored: unknown format") return end
    local previous = store.get("partner:" .. from) or {}
    if (message.seq or 0) <= (previous.seq or 0) then
        print(("ignored: #%s from %s is not newer than #%s"):format(message.seq, from, previous.seq or 0)) return
    end
    if message.type == "request" then
        sendList(true)
        store.set("partner:" .. from, { seq = message.seq, pgpIds = previous.pgpIds, time = os.time() })
    elseif message.type == "list" then
        applyList(from, message)
    else
        print("ignored: unknown type " .. tostring(message.type))
    end
end

local function onMail(msgId)
    local m = mail.getMessage(msgId)
    if not m or m.title ~= SUBJECT then return end -- not ours: leave it alone
    local json = m.body:match("%b{}") -- tolerate HTML around the JSON
    local message = json and jsonapi.decode(json)
    handle(m.from, m.fromType == "identity", message)
    -- Also in DRY_RUN: a processed SyncFriends mail is not handled again.
    print((mail.trashMessage(msgId) and "" or "FAILED: ") .. "move mail " .. msgId .. " to trash")
end

-- Entry point ------------------------------------------------------------------
if args and args.selftest then
    -- Test run with simulated args (always read-only): only shows what would happen.
    dryRun, selftest = true, true
    local t = args.selftest
    handle(t.from or ACCEPT_FROM[1], (t.fromType or "identity") == "identity", t.message)
elseif args and args.msgId then
    onMail(args.msgId)
elseif args and args.list and args.list ~= 1 then
    -- Plugin builds before 2026-11-20 had one list_changed event that also fired
    -- for shared folder lists (often): only the friend list (1) matters.
else
    sendList(false) -- startup, timer, friends_changed: send if changed or due
end
