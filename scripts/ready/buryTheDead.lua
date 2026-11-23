-- buryTheDead: move friends that have been offline for a long time into a
-- separate group, and move them back out once they connect again.
--
-- Click Run now and then, or let a timer run it (e.g. once a day).
-- Access: changes friend groups, so approve it as "Write" first.
-- With many friends, a run may make the GUI lag briefly.

-- name of the group for long-offline friends
local groupName = "Graveyard"

-- days without a connection before a friend is buried
local limit = 30

local SECONDS_PER_DAY = 24 * 60 * 60

-- find our group, returns its info table (incl. peerIds) or nil
local function findGroup()
    for _, grp in ipairs(peers.getGroupInfoList() or {}) do
        if grp.name == groupName then
            return grp
        end
    end
end

-- find our group, create it if it does not exist yet
local function getOrCreateGroup()
    local grp = findGroup()
    if not grp then
        rs.print("creating group " .. groupName)
        peers.addGroup({ flag = 0, id = "0", name = groupName })
        grp = findGroup()
    end
    return grp
end

-- delete the group again (call this manually to clean up)
function removeGroup()
    local grp = findGroup()
    if grp then
        rs.print("removing group " .. grp.name .. " (" .. grp.id .. ")")
        peers.removeGroup(grp.id)
    end
end

-- Groups contain PGP ids (persons), the friend list contains SSL ids
-- (locations). Collect, per PGP id, the most recent connection of any of its
-- locations, so a person with one dead and one active location stays alive.
local function collectLastSeen()
    local now = os.time()
    local persons = {} -- pgpId -> { name = ..., lastSeen = ... }

    for _, ssl in ipairs(peers.getFriendList() or {}) do
        local details = peers.getPeerDetails(ssl)
        local pgp = peers.getGPGId(ssl)
        if details and pgp then
            -- a currently connected location counts as seen right now
            local seen = peers.isOnline(ssl) and now or (details.lastConnect or 0)
            local p = persons[pgp]
            if not p then
                p = { name = details.name or pgp, lastSeen = 0 }
                persons[pgp] = p
            end
            if seen > p.lastSeen then p.lastSeen = seen end
        end
    end

    return persons, now
end

local function run()
    local grp = getOrCreateGroup()
    if not grp then
        rs.print("buryTheDead: could not find or create group " .. groupName)
        return
    end

    local buried = {}
    for _, id in ipairs(grp.peerIds or {}) do buried[id] = true end

    local persons, now = collectLastSeen()

    for pgp, p in pairs(persons) do
        local days = math.floor((now - p.lastSeen) / SECONDS_PER_DAY)
        local dead = days >= limit

        if dead and not buried[pgp] then
            rs.print("burying " .. p.name .. " (offline for " .. days .. " days)")
            peers.assignPeerToGroup(grp.id, pgp, true)
        elseif not dead and buried[pgp] then
            rs.print("unearthing " .. p.name)
            peers.assignPeerToGroup(grp.id, pgp, false)
        end
    end
end

run()
rs.print("buryTheDead done")
