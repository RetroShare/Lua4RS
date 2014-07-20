-- script to move long time offline peers to a seperated group
-- !!! this may cause a GUI lag !!!

-- enter the group name 
groupName = "Graveyard"

-- days offline 
limit = 14

rs.clear()

-- remove group for offline peers
function removeGroup()
	grpList = peers.getGroupInfoList()
	for i = 1, #grpList do
		if grpList[i]["name"] == groupName then
			rs.print("removing " .. grpList[i]["name"] .. " (" .. grpList[i]["id"] .. ")")
			peers.removeGroup(grpList[i]["id"])
		end 
	end
end

-- add group for offline peers
function addGroup()
	grpInfo = {}
	grpInfo["flag"] = 0
	grpInfo["id"] = "0"
	grpInfo["name"] = groupName
	grpInfo["peerIds"] = nil

	peers.addGroup(grpInfo)
end

-- check whether the group exists
function checkGroup()
	grpList = peers.getGroupInfoList()
	for i = 1, #grpList do
		if grpList[i]["name"] == groupName then
			return true
		end 
	end

	-- didn't fing anything 
	return false
end

-- removes all peers from group
function clearGroup()
	done = false

	-- get group(s)
	grpList = peers.getGroupInfoList()
	for i = 1, #grpList do
		if grpList[i]["name"] == groupName then
			done = true
			-- got it. now remove every single peer
			for p = 1, #grpList[i]["peerIds"] do
				rs.print("removing peer " ..  peers.getPeerName(grpList[i]["peerIds"][p]))
				peers.assignPeerToGroup(grpList[i]["id"], grpList[i]["peerIds"][p], false)				
			end
		end 
	end

	if done == false then
		rs.print("couldn't find group " .. groupName)
	end
end

-- adds offline peers to 
function addToGroup()
	grpInfo = nil
	-- find group
	grpList = peers.getGroupInfoList()
	for i = 1, #grpList do
		if grpList[i]["name"] == groupName then
			-- got it. now add peers
			grpInfo = grpList[i]
			break;
		end 
	end

	if grpInfo == nil then
		rs.print("couldn't find group " .. groupName)
		return
	end

	-- get infos
	now = os.time()
	friends = peers.getFriendList()

	for i = 1, #friends do
		details = peers.getPeerDetails(friends[i])	
		diff = now - details["lastUsed"]

		-- get gpg id for groups
		id = peers.getGPGId(details["id"])		

		-- how long in days
		days = diff / 60 / 60 / 24
		days = math.ceil(days)

		-- peer offline for too long?
		if(days >= limit) then
			rs.print("burying " .. details["name"] .. " (offline for " .. days .. " days)")
			peers.assignPeerToGroup(grpInfo["id"], id, true)
		end
	end
end

-- function to combine everything
function run()
	-- create group if needed
	if checkGroup() ~= true then addGroup() end

	-- remove all peers from group
	rs.print("clearing group ... ")
	clearGroup()

	-- now add offline peers
	addToGroup()
end


-- use removeGroup() to cleanup your groups (will remove the group for offline peers)
-- go!
run()
