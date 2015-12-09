function getVersionNumber( s ) 
	b, e = string.find(s, "Revision")

	e = e + 2

	-- revision starts at e and is up to 8 characters long
	rev = s:sub(e, e + 7)

	return rev, true
end

function getName( id )
	return peers.getPeerName( id )
end

function pairsByKeys (t, f)
	local a = {}
	for n in pairs(t) do table.insert(a, n) end
	table.sort(a, f)
	local i = 0      -- iterator variable
	local iter = function ()   -- iterator function
		i = i + 1
		if a[i] == nil then return nil
		else return a[i], t[a[i]]
		end
	end
   return iter
end

function getCommitNumber( hash )
	local handle = io.popen("cd /home/michael/Projects/RetroShare; git describe " .. hash)
	local result = handle:read("*a")
	handle:close()
	-- rs.print(result)
	-- rs.print(tostring(""..string.find(result, ".+-.+-(.+)-.+")))
	for i in string.gmatch(result, ".+-.+-(.+)-.+") do
		return i
	end
	return 0
end

msg = args.msg
chatid = args.chatid

-- rs.print("got message from lobby " .. chatid .. " user: " .. args.nick .. " msg: " .. msg)
-- rs.print("msg is " .. string.len(msg) .. " char(s) long")


if msg ~= nil and  string.len(msg) <= 150 then
	if msg == "!versions" then
		local friends = peers.getFriendList()
		local revList = {}
		local revListNum = {}
		local revListHash = {}
		local numFriends = 0
		for i = 1 , #friends do
			local f = friends[i]
			local revStr = disc.getPeerVersion(f)
			if revStr ~= nil then
				numFriends = numFriends + 1
				local revHash, sane = getVersionNumber(revStr)
				if sane then
					local revNum = "r" .. getCommitNumber(revHash)
					revListHash[revNum] = "g" .. revHash
					table.insert(revList, revNum .. " - " .. getName(f))
					if revListNum[revNum] ~= nil then
						revListNum[revNum] = revListNum[revNum] + 1
					else
						revListNum[revNum] = 1
					end
				end
			end
		end

		toSend = numFriends .. " discovery entries were found"
		for key, value in pairsByKeys(revListNum) do
			toSend = toSend .. "<br>" .. key .. " (" .. revListHash[key] .. "): " .. value .. " time(s)"
		end
		chat.sendChat(chatid, toSend)
	end
end