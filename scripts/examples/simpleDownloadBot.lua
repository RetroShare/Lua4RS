-- Download bot: starts downloads that allowed friends request in a private
-- chat with "!download NAME HASH SIZE".
--
-- Trigger: the event "A chat message arrives" (Automation tab), then Save.
-- Access: sends chat messages and starts downloads, so approve it as "Write" first.

-- Settings
local ALLOWED_IDS = {} -- SSL ids of the friends who may request downloads

if type(args) ~= "table" or type(args.msg) ~= "string" then
    rs.print("Download bot: waiting for chat messages. Enable the trigger \"A chat message arrives\" and Save.")
    if #ALLOWED_IDS == 0 then rs.print("ALLOWED_IDS is empty: nobody may request downloads yet.") end
    return
end

-- Private chats have the chat id "P" + SSL id.
local allowed = {}
for _, id in ipairs(ALLOWED_IDS) do
    allowed[id:sub(1, 1) == "P" and id or "P" .. id] = true
end
local chatid = args.chatid
if not allowed[chatid] then return end

local command, name, hash, size = args.msg:match("^%s*(%S+)%s*(%S*)%s*(%S*)%s*(%S*)")
if command ~= "!download" then return end
if name == "" or hash == "" or not math.tointeger(tonumber(size)) then
    chat.sendChat(chatid, "usage: !download NAME HASH SIZE (size in bytes)")
    return
end
chat.sendChat(chatid, "processing request ...")
local ok = files.fileRequest(name, hash, math.tointeger(tonumber(size)))
chat.sendChat(chatid, ok and "... success!" or "... failed!")
