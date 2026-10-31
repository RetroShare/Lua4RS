-- Reset ALL advertised service permissions: allow by default, clear exceptions.
-- Requires the JSON API development service-control endpoints.
local apply = false -- Review output before enabling. This broadens peer access.
local output = type(rs) == "table" and rs.print or print
local function api(path, params)
    local result, err = jsonapi.call("/rsServiceControl/" .. path, params)
    assert(result, err)
    assert(result.retval == true, path .. " failed")
    return result
end
local pending = {}
for _, entry in ipairs(api("getOwnServices").info.mServiceList) do
    local p = api("getServicePermissions", {serviceId = entry.key}).permissions
    p.mDefaultAllowed = true
    p.mPeersAllowed = jsonapi.array()
    p.mPeersDenied = jsonapi.array()
    pending[#pending + 1] = {serviceId = entry.key, permissions = p}
    output("Reset " .. entry.value.mServiceName .. ": allow by default, clear peer exceptions")
end
if apply then
    for _, update in ipairs(pending) do api("updateServicePermissions", update) end
end
output(apply and "Permissions reset." or "Preview only. Set apply = true to reset permissions.")
