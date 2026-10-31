-- Enable Automation > A chat message arrives, then Save.
-- Type !versions in a lobby to summarize versions reported by your friends.
-- Current RetroShare shares release versions, not enough data for Git commit counts.

local output = type(rs) == "table" and rs.print or print
if type(args) ~= "table" or type(args.msg) ~= "string" then
    output("Discovery bot: waiting for !versions in chat. Enable the chat-message trigger and Save.")
    return
end
local command = args.msg:match("^%s*(.-)%s*$"):lower()
if command ~= "!versions" then return end
local chatid = args.chatid
if type(chatid) ~= "string" or not (chatid:match("^L%x+$") or
    chatid:match("^[PD]%x+$") or chatid == "B") then
    output("Discovery bot: missing or invalid chat ID; cannot reply.")
    return
end

local function versionLabel(version)
    if type(version) ~= "string" then return nil end
    version = version:match("^%s*(.-)%s*$")
    if version == "" then return nil end
    local revision = version:match("Revision%s*:?%s*([%w%.%+%-]+)")
    if revision then return "Revision " .. revision end
    local release = version:match("(%d+%.%d+%.%d+[%w%.%+%-]*)")
    return release or version
end
local function escape(text)
    return (text:gsub("&", "&amp;"):gsub("<", "&lt;"):gsub(">", "&gt;")
                :gsub('"', "&quot;"):gsub("'", "&#39;"))
end

local friends = peers.getFriendList()
if type(friends) ~= "table" then
    output("Discovery bot: could not load the friend list.")
    return
end
local counts, known, unknown = {}, 0, 0
for _, id in ipairs(friends) do
    local version = versionLabel(disc.getPeerVersion(id))
    if version then
        counts[version] = (counts[version] or 0) + 1
        known = known + 1
    else
        unknown = unknown + 1
    end
end
local keys = {}
for version in pairs(counts) do keys[#keys + 1] = version end
table.sort(keys)
local rows = {
    "<p><b>Friend version overview</b><br/>" .. known .. " discovery entries from " .. #friends .. " friend locations.</p>",
    "<table><tr><th>Reported version</th><th>Locations</th></tr>"
}
for _, version in ipairs(keys) do
    rows[#rows + 1] = "<tr><td>" .. escape(version) .. "</td><td>" .. counts[version] .. "</td></tr>"
end
rows[#rows + 1] = "</table>"
if unknown > 0 then rows[#rows + 1] = "<p>" .. unknown .. " location(s) have no reported version.</p>" end
if known == 0 then rows[#rows + 1] = "<p>No version information is available yet.</p>" end
local message = table.concat(rows)
if not chat.sendChat(chatid, message) then
    output("Discovery bot: sending the version overview failed for " .. chatid)
else
    output("Discovery bot: sent version overview (" .. known .. " known, " .. unknown .. " unknown).")
end
