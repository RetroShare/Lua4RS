-- change this 
limit = 60 * 60 * 24 * 14  -- two weeks (in seconds)

now = os.time()

rs.clear()
rs.print("long time offline friends:")
friends = peers.getFriendList()
for i = 1, #friends do
	f = friends[i]
	d = peers.getPeerDetails(f)	
	lastTimeOnline = d["lastUsed"]
	diff = now - lastTimeOnline
	if(diff > limit) then
		-- how long in days
		days = diff / 60 / 60 / 24
		days = math.ceil(days)
		rs.print(d["name"] .. " since " .. days .. " days")
	end
end
