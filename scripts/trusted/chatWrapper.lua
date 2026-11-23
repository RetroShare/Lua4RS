-- Run a trusted local command and send its stdout to a configured chat.
-- Prefer the standalone runner for slow commands: io.popen blocks the GUI.
local command = "" -- Full shell command, e.g. python C:/scripts/report.py
local destination = "" -- Plugin ChatId string (e.g. L + lobby ID in hex).
-- Standalone runner: set destination to a JSON ChatId table instead, e.g.
-- {type = 1, peer_id = "SSL_ID"} for private chat, or {type = 3, lobby_id = ID}.
-- Numeric JSON lobby IDs must fit the client's exact 53-bit range.
local send = false -- Preview output first, then enable sending.
local maxBytes = 16000
local output = type(rs) == "table" and rs.print or print
assert(command ~= "", "Set command to a trusted local program; never build it from incoming chat text")
assert(destination ~= "", "Set destination before running")
local pipe, err = io.popen(command, "r")
assert(pipe, err)
local text = pipe:read(maxBytes + 1) or ""
local ok, why, code = pipe:close()
assert(ok, "Command failed: " .. tostring(why) .. " " .. tostring(code))
assert(#text <= maxBytes, "Command output too large; reduce the report size")
assert(text:match("%S"), "Command produced no output")
if not send then output(text); output("Preview only. Set send = true to send this output."); return end
-- Escape report text so RetroShare renders it as text rather than HTML.
local message = "<pre>" .. text:gsub("&", "&amp;"):gsub("<", "&lt;"):gsub(">", "&gt;") .. "</pre>"
if type(destination) == "string" then
    assert(type(chat) == "table", "String ChatId destinations require the Lua4RS plugin")
    assert(chat.sendChat(destination, message), "Sending chat failed")
else
    assert(type(destination) == "table", "destination must be a ChatId string or table")
    local result, errorMessage = jsonapi.call("/rsChats/sendChat", {id = destination, msg = message})
    assert(result, errorMessage)
    assert(result.retval == true, "Sending chat failed")
end
output("Report sent.")
