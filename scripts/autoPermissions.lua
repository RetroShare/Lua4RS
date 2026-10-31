-- Apply service permissions by group, address or peer name.
-- Configure the JSON API connection; requires development service-control APIs.
-- Rules run in order: later matching rules override earlier rules.
local apply = false -- Review output, then set true to write changes.
local rules = {
    {kind = "group", match = "Graveyard", permissions = {disc = true, chat = false}},
    {kind = "address", match = "%.onion$", permissions = {disc = false}},
    {kind = "address", match = "%.b32%.i2p$", permissions = {disc = false}},
    -- {kind = "name", match = "test", permissions = {turtle = false}},
}
-- Group names match exactly; address/name rules use case-sensitive Lua patterns.
-- Service names must match getOwnServices (e.g. disc, chat, turtle).
local output = type(rs) == "table" and rs.print or print
local function api(path, params)
    local result, err = jsonapi.call(path, params)
    assert(result, err)
    assert(result.retval ~= false, path .. " failed")
    return result
end
local services, changes = {}, {}
for _, entry in ipairs(api("/rsServiceControl/getOwnServices").info.mServiceList) do
    services[entry.value.mServiceName] = entry.key
end
for _, rule in ipairs(rules) do
    assert(rule.kind == "group" or rule.kind == "address" or rule.kind == "name", "Unknown rule kind")
    assert(type(rule.match) == "string", "Missing rule match")
    if rule.kind ~= "group" then assert(pcall(string.match, "", rule.match), "Invalid Lua pattern") end
    for name, allowed in pairs(rule.permissions) do
        assert(services[name], "Unknown service: " .. name)
        assert(type(allowed) == "boolean", "Permission must be boolean")
    end
end
local groups = api("/rsPeers/getGroupInfoList").groupInfoList
local peers = {}
for _, id in ipairs(api("/rsPeers/getFriendList").sslIds) do
    peers[id] = api("/rsPeers/getPeerDetails", {sslId = id}).det
end
local function matches(rule, d)
    if rule.kind == "group" then
        for _, group in ipairs(groups) do
            if group.name == rule.match then
                for _, id in ipairs(group.peerIds) do if id == d.gpg_id then return true end end
            end
        end
        return false
    end
    if rule.kind == "name" then return (d.name or ""):find(rule.match) ~= nil end
    if d.isHiddenNode then return (d.hiddenNodeAddress or ""):find(rule.match) ~= nil end
    if (d.connectAddr or ""):find(rule.match) then return true end
    for _, address in ipairs(d.ipAddressList or {}) do
        if address:find(rule.match) then return true end
    end
    return false
end
for _, rule in ipairs(rules) do
    for id, details in pairs(peers) do
        if matches(rule, details) then
            for name, allowed in pairs(rule.permissions) do
                local service = services[name]
                changes[service] = changes[service] or {name = name, peers = {}}
                changes[service].peers[id] = allowed
            end
        end
    end
end
-- Prepare every update before writing any of them. Unrelated peers are preserved.
local pending = {}
for id, change in pairs(changes) do
    local p = api("/rsServiceControl/getServicePermissions", {serviceId = id}).permissions
    local field = p.mDefaultAllowed and "mPeersDenied" or "mPeersAllowed"
    local members = {}
    for _, peer in ipairs(p[field]) do members[peer] = true end
    for peer, allowed in pairs(change.peers) do
        members[peer] = (p.mDefaultAllowed and not allowed) or (not p.mDefaultAllowed and allowed) or nil
        output((apply and "Apply " or "Preview ") .. change.name .. " / " .. peer .. ": " .. tostring(allowed))
    end
    p[field] = jsonapi.array()
    for peer in pairs(members) do table.insert(p[field], peer) end
    table.sort(p[field])
    pending[#pending + 1] = {serviceId = id, permissions = p}
end
if apply then
    for _, update in ipairs(pending) do api("/rsServiceControl/updateServicePermissions", update) end
end
output(apply and "Permissions applied." or "Preview only. Set apply = true to apply these rules.")
