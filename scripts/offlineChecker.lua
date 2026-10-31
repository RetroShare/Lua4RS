-- Report offline friend locations last connected at least this many days ago.
local daysLimit = 14
local limit = daysLimit * 86400
local now = os.time()
local output = type(rs) == "table" and rs.print or print
if type(rs) == "table" and rs.clear then rs.clear() end
output("Long-time offline friends (" .. daysLimit .. "+ days):")

-- Inside the plugin no HTTP connection or API credentials are needed.
local native = type(peers) == "table" and type(peers.getFriendList) == "function"
    and type(peers.getOnlineList) == "function" and type(peers.getPeerDetails) == "function"
local function api(endpoint, parameters)
    if type(jsonapi) ~= "table" or type(jsonapi.call) ~= "function" then
        return nil, "JSON API client is unavailable. Run in Lua4RS or lua4rs-jsonapi."
    end
    local result, err, status = jsonapi.call(endpoint, parameters or {})
    if type(result) ~= "table" then
        local message = tostring(err or "Invalid response") .. " (HTTP " .. tostring(status or 0) .. ")"
        if status == 401 or status == 403 then
            message = message .. ". Check your authorized API username and password."
        end
        return nil, message
    end
    if result.retval ~= true then return nil, endpoint .. " returned failure" end
    return result
end
local function list(method)
    if native then
        local result = peers[method]()
        if type(result) ~= "table" then return nil, "RetroShare could not retrieve " .. method end
        return result
    end
    local result, err = api("/rsPeers/" .. method)
    if not result then return nil, err end
    if type(result.sslIds) ~= "table" then return nil, method .. ": response has no sslIds array" end
    return result.sslIds
end
local friends, err = list("getFriendList")
if not friends then output("Failed to load friend list: " .. err); return end
local connected, onlineError = list("getOnlineList")
if not connected then output("Cannot determine which friends are offline: " .. onlineError); return end
local online = {}
for _, id in ipairs(connected) do online[id] = true end

local matches, unknown, failed = {}, 0, 0
for _, id in ipairs(friends) do
    if not online[id] then
        local details, detailsError
        if native then
            details = peers.getPeerDetails(id)
        else
            local response
            response, detailsError = api("/rsPeers/getPeerDetails", {sslId = id})
            if response then details = response.det end
        end
        if type(details) ~= "table" then
            failed = failed + 1
            output("Could not read " .. tostring(id) .. ": " .. (detailsError or "Peer details unavailable"))
        else
            local name = type(details.name) == "string" and details.name ~= "" and details.name or tostring(id)
            if type(details.location) == "string" and details.location ~= "" then
                name = name .. " (" .. details.location .. ")"
            end
            -- Current RetroShare fills lastConnect from its last-contact Unix timestamp.
            -- Zero means no recorded connection; it is not an elapsed duration.
            local last = tonumber(details.lastConnect)
            if not last or last <= 0 or last > now then
                unknown = unknown + 1
                output(name .. ": no usable last-connection time (unknown or never connected)")
            elseif now - last >= limit then
                matches[#matches + 1] = {name = name, days = math.floor((now - last) / 86400)}
            end
        end
    end
end

table.sort(matches, function(a, b)
    if a.days == b.days then return a.name < b.name end
    return a.days > b.days
end)
for _, friend in ipairs(matches) do
    output(friend.name .. ": last connected " .. friend.days .. " days ago")
end
output("Checked " .. #friends .. " friend locations; " .. #matches .. " long-time offline.")
if unknown > 0 then output(unknown .. " offline location(s) have no usable connection history.") end
if failed > 0 then output(failed .. " location(s) could not be checked.") end
