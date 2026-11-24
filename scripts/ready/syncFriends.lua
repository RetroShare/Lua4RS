-- SyncFriends: keeps the friend lists of your own nodes in sync over
-- (distant) mail. Every message carries the sender's complete friend list
-- ("state", not tickets), so late or duplicate mails do no harm.
--
-- Trigger: the event "A mail arrives" (Automation tab): applies received lists.
-- Also needs regular runs, e.g. a timer every hour: sends the list when it
-- changed, and at least once a day.
-- The daily resend doubles as a heartbeat: a partner we have not heard from for
-- SILENT_AFTER seconds counts as offline and only gets a list every
-- PROBE_SECONDS, so its mailbox does not fill up. When it writes again, it gets
-- our current list right away.
-- Needs sandbox-write: reads mail and invites (sensitive), adds friends (write).
--
-- Every run logs a banner with the mode, one line per partner (contacted or
-- skipped, with the reason), and a short summary. If nothing seems to happen,
-- read that log first: DRY_RUN, a silent partner, and a failed send each say so
-- there. A partner whose distant-mail permission rejects our mail cannot be
-- detected here: our send succeeds, the partner just never answers (silent).
--
-- Configuration. It is part of the approved code: changing it needs a new approval.
local SEND_AS        = "<own identity id>"         -- own identity the mails come from
local SEND_TO        = { "<partner identity id>" } -- identities that get my friend list
local ACCEPT_FROM    = { "<partner identity id>" } -- identities that may change my friends
local REMOVE_FRIENDS = false          -- also remove friends the partner removed
local RESEND_SECONDS = 24 * 3600       -- send the list at least this often
local SILENT_AFTER   = 3 * RESEND_SECONDS -- no mail from a partner this long: treat as offline
local PROBE_SECONDS  = 7 * 24 * 3600   -- offline partners get the list only this often
local DRY_RUN        = true            -- only log friend add/remove (mails are still sent)

local SUBJECT  = "[Lua4RS SyncFriends]"
local PROTOCOL = 1

local function contains(list, value)
    for _, v in ipairs(list) do if v == value then return true end end
    return false
end

-- Human-readable duration, integer only (no locale-dependent decimals).
local function duration(s)
    s = math.max(0, math.floor(s))
    if s < 90 then return s .. "s" end
    if s < 90 * 60 then return math.floor(s / 60) .. "m" end
    if s < 48 * 3600 then return math.floor(s / 3600) .. "h" end
    return math.floor(s / 86400) .. "d"
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

local function send(recipients, message)
    message.syncfriends = PROTOCOL
    message.sent = os.time()
    local seq = (store.get("seq") or 0) + 1
    message.seq = seq
    local body = jsonapi.encode(message)
    local sent, err = mail.sendMail(SEND_AS, recipients, SUBJECT, body)
    if sent >= #recipients and not err then
        print(("sent %s #%d (%d bytes) to %d partner(s)"):format(message.type, seq, #body, sent))
    else
        -- Only local problems end up here: SEND_AS is not an own identity, or a
        -- recipient identity is unknown on this node. A partner that rejects our
        -- mail (its distant-mail permission) is not reported: it just goes silent.
        print(("FAILED to send %s #%d (%d bytes): reached %d of %d partner(s)%s")
            :format(message.type, seq, #body, sent, #recipients, err and (": " .. err) or ""))
    end
    if sent > 0 then store.set("seq", seq) end
    return sent > 0
end

-- When did we last get a valid message from `id`? Partners we never heard from
-- count from the first time we considered sending to them, so a partner that
-- never answers also goes silent after SILENT_AFTER.
local function lastHeard(id, contact)
    local partner = store.get("partner:" .. id) or {}
    return partner.time or contact.since
end

local function isSilent(id)
    local heard = lastHeard(id, store.get("contact:" .. id) or {})
    return heard ~= nil and os.time() - heard > SILENT_AFTER
end

-- Sends the own list to the partners in SEND_TO (or only to `only`) for which
-- it is due. Per partner: active ones get it on every change and at least every
-- RESEND_SECONDS, silent ones only every PROBE_SECONDS. `force` sends anyway.
-- Logs one line per partner (contacted / skipped, with the reason).
local function sendList(force, only)
    local friends = ownFriends()
    local fp = fingerprint(friends)
    local now = os.time()
    local recipients, contacts, skipped = {}, {}, 0
    for _, id in ipairs(SEND_TO) do
        if not only or id == only then
            local c = store.get("contact:" .. id) or {}
            if not c.since then c.since = now; store.set("contact:" .. id, c) end
            local silent = now - lastHeard(id, c) > SILENT_AFTER
            local age = now - (c.time or 0)
            local never = not c.time
            local due, reason
            if force then
                due, reason = true, "forced reply"
            elseif silent then
                -- Never answered at all vs. stopped answering: the first often
                -- means the partner rejects our mail (distant-mail permission).
                local heard = (store.get("partner:" .. id) or {}).time
                local state = heard and ("silent for " .. duration(now - heard))
                    or ("never heard from it in " .. duration(now - c.since))
                due = age >= PROBE_SECONDS
                reason = state .. (due and ", probe due" or (", probed " .. duration(age) ..
                    " ago, next probe in " .. duration(PROBE_SECONDS - age))) ..
                    "; offline, or it rejects our mail: check its distant-mail permission"
            elseif never then
                due, reason = true, "first contact"
            elseif c.fingerprint ~= fp then
                due, reason = true, "friend list changed"
            elseif age >= RESEND_SECONDS then
                due, reason = true, "periodic resend due"
            else
                due, reason = false, ("no change, sent " .. duration(age) ..
                    " ago, next resend in " .. duration(RESEND_SECONDS - age))
            end
            if due then
                recipients[#recipients + 1] = id
                contacts[id] = c
                print(("  -> %s: SEND (%s)"):format(id, reason))
            else
                skipped = skipped + 1
                print(("  -- %s: skip (%s)"):format(id, reason))
            end
        end
    end
    if #recipients == 0 then
        if only then
            print(("reply: %s is not in SEND_TO, not sending"):format(only))
        else
            print(("summary: nothing to send (%d partner(s) up to date or offline)"):format(skipped))
        end
        return
    end
    local ok = send(recipients, { type = "list", friends = friends })
    if ok then
        for id, c in pairs(contacts) do
            c.fingerprint, c.time = fp, now
            store.set("contact:" .. id, c)
        end
    end
    print(("summary: %s %d partner(s), skipped %d%s")
        :format(ok and "contacted" or "tried", #recipients, skipped, ok and "" or " -- SEND FAILED, see above"))
end

-- Applies a partner's list. Prints every action and a count summary.
local function applyList(from, message)
    local mine = {}
    for _, ssl in ipairs(peers.getFriendList() or {}) do mine[ssl] = true end
    local ownPgp, ownSsl = peers.getGPGOwnId(), peers.getOwnId()

    local added, known, failed = 0, 0, 0
    local now = {}
    for _, f in ipairs(message.friends or {}) do
        now[f.pgpId] = true
        if f.pgpId ~= ownPgp then
            for _, l in ipairs(f.locations or {}) do
                if l.sslId == ownSsl then
                    -- our own location: nothing to do
                elseif mine[l.sslId] then
                    known = known + 1
                elseif act(("add %s (%s / %s)"):format(l.sslId, f.name, l.name), peers.acceptInvite, l.invite) then
                    added = added + 1
                else
                    failed = failed + 1
                end
            end
        end
    end

    local removed = 0
    local key = "partner:" .. from
    local previous = store.get(key) or {}
    if REMOVE_FRIENDS then
        for _, pgp in ipairs(previous.pgpIds or {}) do
            if not now[pgp] and pgp ~= ownPgp and peers.isGPGAccepted(pgp) then
                if act(("remove %s (%s)"):format(pgp, peers.getGPGName(pgp) or "?"), peers.removeFriend, pgp) then
                    removed = removed + 1
                else
                    failed = failed + 1
                end
            end
        end
    end
    local pgpIds = {}
    for pgp in pairs(now) do pgpIds[#pgpIds + 1] = pgp end
    table.sort(pgpIds)
    store.set(key, { seq = message.seq, pgpIds = pgpIds, time = os.time() })
    print(("applied list from %s: %d location(s) added, %d already known, %d removed, %d FAILED%s%s")
        :format(from, added, known, removed, failed,
            REMOVE_FRIENDS and "" or " (REMOVE_FRIENDS off)",
            dryRun and " -- DRY_RUN: only logged, nothing changed" or ""))
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
    -- Answer right away if this is the first list from the partner (so both
    -- sides know each other) or the partner was silent (it missed our updates).
    local answer = not previous.seq or isSilent(from)
    if message.type == "request" then
        answer = true
        store.set("partner:" .. from, { seq = message.seq, pgpIds = previous.pgpIds, time = os.time() })
    elseif message.type == "list" then
        applyList(from, message)
    else
        print("ignored: unknown type " .. tostring(message.type))
        return
    end
    if answer then
        local why = message.type == "request" and "partner asked for it"
            or (not previous.seq and "first contact" or "partner was silent")
        if selftest then
            print("would reply with own list (" .. why .. ")")
        else
            print("replying with own list (" .. why .. ")")
            sendList(true, from)
        end
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
    print("SyncFriends | selftest: read-only, nothing is sent or changed")
    local t = args.selftest
    handle(t.from or ACCEPT_FROM[1], (t.fromType or "identity") == "identity", t.message)
elseif args and args.msgId then
    print(("SyncFriends | incoming mail %s | %s"):format(tostring(args.msgId),
        dryRun and "DRY_RUN ON (friend add/remove only logged; mails are still sent)" or "LIVE"))
    onMail(args.msgId)
elseif args and args.list and args.list ~= 1 then
    -- Plugin builds before 2026-11-20 had one list_changed event that also fired
    -- for shared folder lists (often): only the friend list (1) matters.
else
    print(("SyncFriends | checking %d partner(s) | %s"):format(#SEND_TO,
        dryRun and "DRY_RUN ON (friend add/remove only logged; mails are still sent)" or "LIVE"))
    sendList(false) -- startup, timer, friends_changed: send if changed or due
end
