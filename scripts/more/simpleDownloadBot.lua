-- peer IDs (SSL)
allowedIds = {}

-- split
function split(str)
	t = {}
	for s in string.gmatch(str, "%S+") do
		table.insert(t, s)
	end
	return t
end

-- inTable
function inTable(tbl, item)
    for key, value in pairs(tbl) do
        if value == item then return true end
    end
    return false
end

-- add leading 'P' for IDs
for index, id in pairs(allowedIds) do
	if string.sub(id, 1, 1) ~= "P" then
		allowedIds[index] = "P" .. id
	end
end

msg = args.msg
chatid = args.chatid

if inTable(allowedIds, chatid) then
	-- peer is allowed to send commands
	parts = split(msg, " ")

	if parts[1] == "!download" then
		name = parts[2]
		hash = parts[3]
		size = parts[4]

		if name == nil or hash == nil or size == nil then
			chat.sendChat(chatid, "usage: !download *file name* *file hash* *file size*")
		else
			chat.sendChat(chatid, "processing request ...")
			ok = files.fileRequest(name, hash, tonumber(size))
			if ok then
				chat.sendChat(chatid, "... success!")
			else
				chat.sendChat(chatid, "... failed!")
			end
		end
	end
end
	
