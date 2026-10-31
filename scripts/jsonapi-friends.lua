-- Configure RETROSHARE_API_URL, RETROSHARE_API_USER and
-- RETROSHARE_API_PASSWORD in the environment before starting the runner/plugin.
local response, err, status = jsonapi.call("/rsPeers/getFriendList")
assert(response, string.format("%s (HTTP %s)", tostring(err), tostring(status)))
assert(response.retval, "RetroShare could not retrieve the friend list")
for _, peerId in ipairs(response.sslIds) do
    print(peerId)
end
