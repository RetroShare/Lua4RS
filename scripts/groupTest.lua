rs.clear();

grpList = peers.getGroupInfoList()
for i = 1, #grpList do
	grpInfo = grpList[i]
	rs.print(grpInfo["name"])
	if #grpInfo["peerIds"] > 0 then
		peerIds = grpInfo["peerIds"]
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