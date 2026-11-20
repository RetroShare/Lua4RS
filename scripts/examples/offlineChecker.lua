-- Offline checker: lists friend locations that have not connected for a while.
--
-- Trigger: none, click Run (or a daily timer).   Access: read-only (works in the sandbox).

-- Settings
local DAYS = 14   -- report locations offline for at least this many days

local now = os.time()
local limit = DAYS * 86400

rs.clear()
rs.print("Friend locations offline for " .. DAYS .. "+ days:")

local online = {}
for _, id in ipairs(peers.getOnlineList() or {}) do online[id] = true end

local friends = peers.getFriendList() or {}
local found, neverSeen = {}, 0
for _, id in ipairs(friends) do
    if not online[id] then
        local details = peers.getPeerDetails(id)
        if details then
            local name = details.name ~= "" and details.name or id
            if details.location and details.location ~= "" then
                name = name .. " (" .. details.location .. ")"
            end
            -- lastConnect is a Unix time; 0 means never connected.
            local last = tonumber(details.lastConnect) or 0
            if last <= 0 or last > now then
                neverSeen = neverSeen + 1
            elseif now - last >= limit then
                found[#found + 1] = { name = name, days = (now - last) // 86400 }
            end
        end
    end
end

table.sort(found, function(a, b)
    if a.days ~= b.days then return a.days > b.days end
    return a.name < b.name
end)
for _, friend in ipairs(found) do
    rs.print(string.format("%5d days  %s", friend.days, friend.name))
end
rs.print(#friends .. " locations checked, " .. #found .. " offline for " .. DAYS .. "+ days")
if neverSeen > 0 then rs.print(neverSeen .. " location(s) never connected") end
