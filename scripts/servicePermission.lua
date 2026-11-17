-- Daily service permission schedule, in local time: start inclusive, finish exclusive.
-- Add a Lua4RS startup trigger and a timer interval trigger (e.g. every 30 seconds).
-- Configure the plugin JSON API connection; rsServiceControl development endpoints
-- must be enabled in RetroShare. No C++ binding or plugin rebuild is required.
-- This blocks peer access, not the service's internal processing.
-- Uses the same controls as Web UI > My Services. Names come from getOwnServices.
-- Use service = "*" for every service, or the exact Web UI name / numeric full ID.
local rules = {
    {service = "Chat", start = "22:00", finish = "07:00"},
    -- {service = "Turtle Router", start = "22:00", finish = "07:00"},
    -- {service = "*", start = "22:00", finish = "07:00"},
    -- {service = "File Transfer", start = "09:00", finish = "17:00"},
}
-- Set true and run once to restore all saved permissions before removing the script.
local restoreOnly = false
-- Standalone runner: set this to an existing writable, profile-specific directory.
local stateDirectory = nil

local legacyNames = {
    [0x0011] = "Discovery", [0x0012] = "Chat", [0x0013] = "Messages",
    [0x0014] = "Turtle Router", [0x0016] = "Heartbeat", [0x0017] = "File Transfer",
    [0x0018] = "Global Router", [0x0019] = "File Database", [0x0020] = "Service Info",
    [0x0021] = "Bandwidth", [0x0101] = "Banlist", [0x0102] = "Status",
    [0x0028] = "GXS Tunnels", [0x0211] = "GXS Identity", [0x0213] = "GXS Wiki",
    [0x0214] = "GXS Wire", [0x0215] = "GXS Forums", [0x0216] = "GXS Boards",
    [0x0217] = "GXS Channels", [0x0218] = "GXS Circles", [0x0219] = "GXS Reputation",
    [0x0230] = "GXS Mails", [0x1011] = "RTT", [0x2003] = "FeedReader",
}
local output = type(rs) == "table" and rs.print or print
local function api(method, parameters)
    local result, err = jsonapi.call("/rsServiceControl/" .. method, parameters)
    assert(type(result) == "table" and result.retval == true,
        method .. ": " .. tostring(err or "API returned failure; check development API availability"))
    return result
end
local function minute(value)
    local h, m = tostring(value):match("^(%d%d):(%d%d)$")
    h, m = tonumber(h), tonumber(m)
    assert(h and h < 24 and m and m < 60, "Invalid HH:MM time: " .. tostring(value))
    return h * 60 + m
end
local function run()
    assert(type(jsonapi) == "table", "Run in Lua4RS or lua4rs-jsonapi")
    local directory = stateDirectory or (type(rs) == "table" and rs.luaFolder and rs.luaFolder())
    assert(directory, "Set stateDirectory to a profile-specific writable directory")
    local lookup, active, serviceNames = {}, {}, {}
    local info = api("getOwnServices", {}).info
    assert(type(info) == "table" and type(info.mServiceList) == "table",
        "Invalid service list from getOwnServices")
    for _, entry in ipairs(info.mServiceList) do
        local fullId, service = entry.key, entry.value
        assert(type(fullId) == "number" and fullId % 256 == 0
            and fullId >= 0x02000000 and fullId <= 0x02ffff00
            and type(service) == "table" and type(service.mServiceName) == "string",
            "Invalid service entry from getOwnServices")
        serviceNames[fullId] = service.mServiceName
        lookup[service.mServiceName:lower()] = fullId
    end
    assert(next(serviceNames), "RetroShare returned no services")
    -- Keep old friendly names working, but only for services actually installed.
    for id, name in pairs(legacyNames) do
        local fullId = 0x02000000 + id * 256
        if serviceNames[fullId] and not lookup[name:lower()] then lookup[name:lower()] = fullId end
    end
    local now = os.date("*t")
    now = now.hour * 60 + now.min
    if not restoreOnly then
        -- Validate every rule before changing any permissions. Overlapping rules combine.
        for _, rule in ipairs(rules) do
            local all = rule.service == "*"
            local id = type(rule.service) == "number" and rule.service
                or lookup[tostring(rule.service):lower()]
            assert(all or serviceNames[id], "Unknown or unavailable service: " .. tostring(rule.service))
            local first, last = minute(rule.start), minute(rule.finish)
            assert(first ~= last, "Start and finish must differ for " .. tostring(rule.service))
            local inside
            if first < last then inside = now >= first and now < last
            else inside = now >= first or now < last end
            if all then
                for serviceId in pairs(serviceNames) do active[serviceId] = active[serviceId] or inside end
            else active[id] = active[id] or inside end
            output("Schedule " .. tostring(rule.service) .. " " .. rule.start .. "-" .. rule.finish
                .. (inside and ": blocking now" or ": outside blocked window")
                .. " (local time; startup + repeating timer required)")
        end
    end
    for fullId, name in pairs(serviceNames) do
        local ok, err = pcall(function()
            -- Keep existing backup filenames compatible with earlier versions.
            local id = math.floor((fullId - 0x02000000) / 256)
            local path = directory .. "/servicePermission-" .. string.format("%04x", id) .. ".json"
            local file, readError, code = io.open(path, "rb")
            local saved
            if file then
                local data = assert(file:read("*a")); file:close()
                saved = assert(jsonapi.decode(data))
                assert(type(saved) == "table" and saved.mServiceId == fullId
                    and type(saved.mDefaultAllowed) == "boolean"
                    and type(saved.mPeersAllowed) == "table" and type(saved.mPeersDenied) == "table",
                    "Invalid permission backup: " .. path)
            else
                assert(code == 2, readError) -- Only a missing file means no saved permissions.
            end
            if active[fullId] then
                local current = api("getServicePermissions", {serviceId = fullId}).permissions
                assert(type(current) == "table" and current.mServiceId == fullId
                    and type(current.mDefaultAllowed) == "boolean"
                    and type(current.mPeersAllowed) == "table" and type(current.mPeersDenied) == "table",
                    "Invalid permissions for " .. name)
                if not saved then
                    -- Commit the backup before touching permissions. Keep it on API failure
                    -- (including timeouts, where the server may have applied the update).
                    local data = assert(jsonapi.encode(current))
                    local tmp = assert(io.open(path .. ".tmp", "wb"))
                    assert(tmp:write(data)); assert(tmp:close())
                    assert(os.rename(path .. ".tmp", path))
                end
                if current.mDefaultAllowed or next(current.mPeersAllowed) then
                    current.mDefaultAllowed = false
                    current.mPeersAllowed = jsonapi.array()
                    api("updateServicePermissions", {serviceId = fullId, permissions = current})
                    local verified = api("getServicePermissions", {serviceId = fullId}).permissions
                    assert(type(verified) == "table" and verified.mDefaultAllowed == false
                        and type(verified.mPeersAllowed) == "table" and not next(verified.mPeersAllowed),
                        "Permission update did not block " .. name .. "; backup retained for retry")
                    output("Disabled " .. name .. " during scheduled window")
                end
            elseif saved then
                api("updateServicePermissions", {serviceId = fullId, permissions = saved})
                assert(os.remove(path))
                output("Restored original permissions for " .. name)
            end
        end)
        if not ok then output("servicePermission: " .. name .. ": " .. tostring(err)) end
    end
end
local ok, err = pcall(run)
if not ok then output("servicePermission: " .. tostring(err)) end

-- Backups survive restarts; keep this script running so expired windows restore.
-- If RetroShare/the timer is stopped, restoration happens on the next run.
-- Do not delete the backup files while permissions are disabled. During a window,
-- manual permission edits are overridden; restoration uses the original snapshot.
-- Use only one scheduler instance per profile. Removed rules restore on the next run.
