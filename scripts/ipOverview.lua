--[[
local IPType = {
    [0] = "Error",
    [1] = "IPv4",
    [2] = "IPv6",
    [3] = "string",
}
]]--

function GetIPType(ip)
    -- must pass in a string value
    if ip == nil or type(ip) ~= "string" then
        return 0
    end

    -- check for format 1.11.111.111 for ipv4
    local chunks = {ip:match("(%d+)%.(%d+)%.(%d+)%.(%d+)")}
    if (#chunks == 4) then
        for _,v in pairs(chunks) do
            if (tonumber(v) < 0 or tonumber(v) > 255) then
                return 0
            end
        end
        return 1
    else
			rs.print("WOHOO")
        return 0
    end

    -- check for ipv6 format, should be 8 'chunks' of numbers/letters
    local _, chunks = ip:gsub("[%a%d]+%:?", "")
		rs.print(chunks .. "")
    if chunks == 8 then
        return 2
    end

    -- if we get here, assume we've been given a random string
    return 3
end

local friends = peers.getFriendList()
local ipv4 = 0
local ipv6 = 0
local ipv4con = 0
local ipv6con = 0

for i = 1, #friends do
	local details = peers.getPeerDetails(friends[i])
	--rs.print(details['name'])

	local ips = details['ipAddressList']
	for j = 1, #ips do
		-- IPs in list
		--rs.print(ips[j])
		local ip = ips[j]
		if ip.find(ip, "IPv4") ~= nil then ipv4 = ipv4 + 1 end
		if ip.find(ip, "IPv6") ~= nil then ipv6 = ipv6 + 1 end
	end

	-- actuall connected to
	ip = details["connectAddr"]
	if ip ~= nil then
		--rs.print(ip)
		if GetIPType(ip) == 1 then ipv4con = ipv4con + 1 end
		if GetIPType(ip) == 2 then ipv6con = ipv6con + 1 end
	end
end

rs.print("Friends IPs (all)\tIPv4: " .. ipv4 .. "   IPv6: " .. ipv6)
rs.print("Friends IPs (connected)\tIPv4: " .. ipv4con .. "   IPv6: " .. ipv6con)