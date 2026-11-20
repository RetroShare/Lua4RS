-- IP overview: how many of your friends' addresses are IPv4 or IPv6, and
-- which protocol the connected friends use right now.
--
-- Trigger: none, click Run.   Access: read-only (works in the sandbox).

local function ipType(address)
    if type(address) ~= "string" then return nil end
    -- Address list entries look like "ipv4://1.2.3.4:port ..." or "ipv6://[..]:port ..."
    local scheme = address:match("^(ipv[46])://")
    if scheme then return scheme end
    -- connectAddr is a bare address: "1.2.3.4" or "2001:db8::1"
    if address:match("^%d+%.%d+%.%d+%.%d+$") then return "ipv4" end
    if address:find(":", 1, true) then return "ipv6" end
    return nil
end

rs.clear()
local known = { ipv4 = 0, ipv6 = 0 }
local connected = { ipv4 = 0, ipv6 = 0 }
local friendsWithIpv6 = 0
local friends = peers.getFriendList() or {}

for _, id in ipairs(friends) do
    local details = peers.getPeerDetails(id)
    if details then
        local hasIpv6 = false
        for _, address in ipairs(details.ipAddressList or {}) do
            local kind = ipType(address)
            if kind then known[kind] = known[kind] + 1 end
            if kind == "ipv6" then hasIpv6 = true end
        end
        if hasIpv6 then friendsWithIpv6 = friendsWithIpv6 + 1 end

        if peers.isOnline(id) then
            local kind = ipType(details.connectAddr)
            if kind then connected[kind] = connected[kind] + 1 end
        end
    end
end

rs.print(#friends .. " friend locations, " .. friendsWithIpv6 .. " of them with an IPv6 address")
rs.print("Known addresses:     IPv4 " .. known.ipv4 .. "   IPv6 " .. known.ipv6)
rs.print("Connected right now: IPv4 " .. connected.ipv4 .. "   IPv6 " .. connected.ipv6)
