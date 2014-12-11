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
		return rev, false
	else
		return rev, true
	end
end

function getName( id )
	return peers.getPeerName( id )
end

rs.clear()

-- down, up = disc.getWaitingDiscCount()
-- rs.print("waiting discovery down=" .. down .. " up=" .. up)

friends = peers.getFriendList()
revList = {}
for i = 1 , #friends do
	f = friends[i]
	revStr = disc.getPeerVersion(f)
	if revStr ~= nil then
		revNum, sane = getVersionNumber(revStr)
		if sane then
			table.insert(revList, revNum .. " - " .. getName(f))
		else
			rs.print(getName(f) .. " is using " .. revStr)
		end
	end
end

rs.print("--------------------")
table.sort(revList)
for key, value in pairs(revList) do
	rs.print(value)
end
