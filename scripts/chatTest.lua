rs.clear()

msg = args.msg
chatid = args.chatid

rs.print("got message from lobby " .. chatid)

find = string.find(msg, "ping")
if find ~= nil then
	rs.print("found ping")
	chat.sendChat(chatid, "[Lua] pong")
end

find = string.find(msg, "PING")
if find ~= nil then
	rs.print("found ping")
	chat.sendChat(chatid, "[Lua] PONG (for the funny guy)")
end
