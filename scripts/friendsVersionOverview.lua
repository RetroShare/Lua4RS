-- Show versions reported by discovery, including modern and legacy peers.
local function getVersionNumber(version)
    if type(version) ~= "string" then return "unknown", false end
    version = version:match("^%s*(.-)%s*$")
    if version == "" then return "unknown", false end

    -- Older peers advertised an SVN revision, sometimes as 0.6.0.1234.
    local revision = version:match("Revision%s*:?%s*([%d%.]+)")
    if revision then
        local number = revision:match("^%d+%.%d+%.%d+%.(%d+)$")
                    or revision:match("^(%d+)$")
        if number then return "Revision " .. number, true end
    end

    -- Current peers advertise a release version rather than an SVN revision.
    local release = version:match("(%d+%.%d+%.%d+[%w%.%+%-]*)")
    if release then return release, true end
    return version, false
end

rs.clear()
local friends = peers.getFriendList() or {}
local versions, counts = {}, {}
local found, unknown = 0, 0
for _, id in ipairs(friends) do
    local name = peers.getPeerName(id)
    if type(name) ~= "string" or name == "" then name = tostring(id) end
    local reported = disc.getPeerVersion(id)
    if type(reported) == "string" and reported:match("%S") then
        found = found + 1
        local version, recognized = getVersionNumber(reported)
        if recognized then
            counts[version] = (counts[version] or 0) + 1
            versions[#versions + 1] = version .. " - " .. name
        else
            rs.print(name .. " is using " .. version)
        end
    else
        unknown = unknown + 1
        rs.print(name .. ": version not available from discovery")
    end
end

rs.print("--------------------")
rs.print(found .. " discovery entries were found")
if unknown > 0 then rs.print(unknown .. " friend(s) have no reported version") end
local keys = {}
for version in pairs(counts) do keys[#keys + 1] = version end
table.sort(keys)
for _, version in ipairs(keys) do
    rs.print(version .. ": " .. counts[version] .. " time(s)")
end
rs.print("--------------------")
table.sort(versions)
for _, line in ipairs(versions) do rs.print(line) end
