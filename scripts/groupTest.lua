rs.clear();

-- get list of oll groups
grpList = peers.getGroupInfoList()

for i = 1, #grpList do
	-- get group info
	grpInfo = grpList[i]
	
	-- print group name
	rs.print(grpInfo["name"])
		
	-- check if there is any peer in the group
	if #grpInfo["peerIds"] > 0 then
		-- if yes: get peer ids
		peerIds = grpInfo["peerIds"]
		
		-- print their names
		for j = 1, #peerIds do
			id = peerIds[j]
			rs.print(" - " .. peers.getPeerName(id) .. " (" .. id .. ")")
		end
	end
end

-- insert the peer id from a peer that is in a group
idToRemove=""

if idToRemove ~= "" then
	-- groupId == "" && assign == false -> remove from all groups
	peers.assignPeerToGroup("", idToRemove, false)
end
