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

function contains(a, array)
	for i = 1, #array do
		if array[i] == a then
			return true
		end
	end
	return false
end

function getGroupId()
	grpList = peers.getGroupInfoList()
	for i = 1, #grpList do
		if grpList[i]["name"] == groupName then
			return grpList[i]["id"]
		end 
	end
end

-- function to combine everything
function run()
	-- create group if needed
	if checkGroup() ~= true then addGroup() end

	grpInfo = peers.getGroupInfo( getGroupId() )
	
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
		if days >= limit then
			rs.print("burying " .. details["name"] .. " (offline for " .. days .. " days)")
			peers.assignPeerToGroup(grpInfo["id"], id, true)
		else
			if contains(id, grpInfo["peerIds"]) then
				peers.assignPeerToGroup(grpInfo["id"], id, false)
			end
		end
	end
end


-- use removeGroup() to cleanup your groups (will remove the group for offline peers)
-- go!
run()
