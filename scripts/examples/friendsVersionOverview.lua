-- Friends' versions: which RetroShare versions your friends report through
-- discovery.
--
-- Trigger: none, click Run.   Access: read-only (works in the sandbox).

-- "0.6.7.2", or "Revision 1234" for very old peers; nil if unrecognized
local function versionOf(reported)
    local revision = reported:match("Revision%s*:?%s*([%d%.]+)")
    if revision then
        local number = revision:match("^%d+%.%d+%.%d+%.(%d+)$") or revision:match("^(%d+)$")
        if number then return "Revision " .. number end
    end
    return reported:match("(%d+%.%d+%.%d+[%w%.%+%-]*)")
end

rs.clear()
local counts, lines, unknown = {}, {}, 0
local friends = peers.getFriendList() or {}
for _, id in ipairs(friends) do
    local name = peers.getPeerName(id)
    if type(name) ~= "string" or name == "" then name = id end
    local reported = disc.getPeerVersion(id)
    if type(reported) ~= "string" or not reported:match("%S") then
        unknown = unknown + 1
    else
        local version = versionOf(reported) or reported:match("^%s*(.-)%s*$")
        counts[version] = (counts[version] or 0) + 1
        lines[#lines + 1] = version .. "  " .. name
    end
end

local versions = {}
for version in pairs(counts) do versions[#versions + 1] = version end
table.sort(versions)
for _, version in ipairs(versions) do
    rs.print(string.format("%-20s %d", version, counts[version]))
end
if unknown > 0 then rs.print(unknown .. " friend(s) reported no version (yet)") end
rs.print("--------------------")
table.sort(lines)
for _, line in ipairs(lines) do rs.print(line) end
