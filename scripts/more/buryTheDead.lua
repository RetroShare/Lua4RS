-- script to move long time offline peers to a seperated group
-- !!! this may cause a GUI lag !!!

-- enter the group name
local groupName = "Graveyard"

-- days offline
local limit = 30

-- rs.clear()

-- remove group for offline peers
function removeGroup()
        local grpList = peers.getGroupInfoList()
        for i = 1, #grpList do
                if grpList[i]["name"] == groupName then
                        rs.print("removing " .. grpList[i]["name"] .. " (" .. grpList[i]["id"] .. ")")
                        peers.removeGroup(grpList[i]["id"])
                end
        end
end

-- add group for offline peers
function addGroup()
        local grpInfo = {}
        grpInfo["flag"] = 0
        grpInfo["id"] = "0"
        grpInfo["name"] = groupName
        grpInfo["peerIds"] = nil

        peers.addGroup(grpInfo)
end

-- check whether the group exists
function checkGroup()
        local grpList = peers.getGroupInfoList()
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
        local grpList = peers.getGroupInfoList()
        for i = 1, #grpList do
                if grpList[i]["name"] == groupName then
                        return grpList[i]["id"]
                end
        end
end

function getDays(details)
        local now = os.time()
        local diff = now - details["lastUsed"]

        -- how long in days
        local days = diff / 60 / 60 / 24
        return math.ceil(days)
end

-- function to combine everything
function run()
        -- create group if needed
        if checkGroup() ~= true then addGroup() end

        local grpInfo = peers.getGroupInfo( getGroupId() )

        -- get infos
        local friends = peers.getFriendList()

        for i = 1, #friends do
                local details = peers.getPeerDetails(friends[i])
                -- get gpg id for groups
                local id = peers.getGPGId(details["id"])

                local days = getDays(details)

                -- peer offline for too long?
                if days >= limit then
                        if not contains(id, grpInfo["peerIds"]) then
                                rs.print("burrying " .. details["name"] .. " (offline for " .. days .. " days)")
                                peers.assignPeerToGroup(grpInfo["id"], id, true)
                        end
                end
        end

        for i = 1, #friends do
                local details = peers.getPeerDetails(friends[i])
                -- get gpg id for groups
                local id = peers.getGPGId(details["id"])

                local days = getDays(details)

                -- peer offline for too long?
                if days < limit then
                        if contains(id, grpInfo["peerIds"]) then
                                rs.print("removing " .. details["name"])
                                peers.assignPeerToGroup(grpInfo["id"], id, false)
                        end
                end
        end
end


-- use removeGroup() to cleanup your groups (will remove the group for offline peers)
-- go!
run()
rs.print('buryTheDead done')
