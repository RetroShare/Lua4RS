function getVersionNumber( s ) 
	b, e = string.find(s, "Revision")

	-- check for ':'
	x = s:sub(e + 1, e + 1)
	if x == ":" then 
		e = e + 3
	else
		e = e + 2 
	end

	-- revision starts at e and is usually 4 characters long
	rev = s:sub(e, e + 3)

	-- check for sane revision
	if tonumber(rev) == nil then
		-- the revision string might be '0.6.0.xxxx'
		e = e + 6
		rev = s:sub(e, e + 3)

		-- check for sane revision
		if tonumber(rev) == nil then
			return rev, false
		else
			return rev, true
		end
	else
		return rev, true
	end
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

rs.clear()

-- down, up = disc.getWaitingDiscCount()
-- rs.print("waiting discovery down=" .. down .. " up=" .. up)

friends = peers.getFriendList()
revList = {}
revListNum = {}
numFriends = 0
for i = 1 , #friends do
	f = friends[i]
	revStr = disc.getPeerVersion(f)
	if revStr ~= nil then
		numFriends = numFriends + 1
		revNum, sane = getVersionNumber(revStr)
		if sane then
			table.insert(revList, revNum .. " - " .. getName(f))
			if revListNum[revNum] ~= nil then
				revListNum[revNum] = revListNum[revNum] + 1
			else
				revListNum[revNum] = 1
			end
		else
			rs.print(getName(f) .. " is using " .. revStr)
		end
	end
end

rs.print("--------------------")
rs.print(numFriends .. " discovery entries were found")
for key, value in pairsByKeys(revListNum) do
	rs.print(key .. ": " .. value .. " time(s)")
end

rs.print("--------------------")
table.sort(revList)
for key, value in pairs(revList) do
	rs.print(value)
end
