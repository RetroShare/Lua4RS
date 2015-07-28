--[[
structue:
	["matchName"] = {
		["enabled"] = true, ["only"] = true, ["response"] = "pong",
		["matches"] = {
			[1] = "ping",
			[2] = "PING",
			[3] =  ...
		}
	},

	matchName: the name of the matching rule
	enabled: is this rule active?
	only: match to whole message (only = true) or only check if the message contains one match (onyl = false)
	response: what to respond
	matches: array with all strings to match
]]--

-- edit here

matchingTable = {
	["ping"] = {
		["enabled"] = true, ["only"] = true, ["response"] = "pong",
		["matches"] = {
			[1] = "ping",
			[2] = "PING",
		}
	},
	["pong"] = {
		["enabled"] = false, ["only"] = true, ["response"] = "ping",
		["matches"] = {
			[1] = "pong",
			[2] = "PONG",
		}
	},
	["test"] = {
		["enabled"] = true, ["only"] = true, ["response"] = "test back :P",
		["matches"] = {
			[1] = "test",
		}
	},
	["Lua4RS"] = {
		["enabled"] = true, ["only"] = false, ["response"] = "Lua4RS \\o/",
		["matches"] = {
			[1] = "Lua4RS",
			[2] = "lua4rs",
		}
	},
}

botPrefix = "[Lua4RS]"

-- list all matching rules 
rulesCommandEnabled = true
rulesCommandMatch = "!rules"

-- edit end

msg = args.msg
chatid = args.chatid

if msg ~= nil then

rs.print("got message from lobby " .. chatid .. " user: " .. args.nick .. " msg: " .. msg)
-- rs.print("msg is " .. string.len(msg) .. " char(s) long")

if string.len(msg) <= 150 then
	-- check for matches
	for matchName, matchValue in pairs(matchingTable) do
		if matchValue["enabled"] then
			-- triggered?
			triggered = false
			matches = matchValue["matches"]
			for i = 1, #matches do
				if matchValue["only"] then
					if msg == matches[i] then triggered = true end
				else
					if string.find(msg, matches[i]) ~= nil then triggered = true end
				end
			end

			if triggered then
				-- respond!
				rs.print("rule " .. matchName .. " triggered")
				chat.sendChat(chatid, botPrefix .. ": " .. matchValue["response"])
			end
		end
	end

	-- !rules command
	if rulesCommandEnabled then
		if msg == rulesCommandMatch then
			response = botPrefix .. ": rules known: "
			for matchName, matchValue in pairs(matchingTable) do
				response = response .. matchName
				if not matchValue["enabled"] then response = response .. "(disabled)" end
				response = response .. ", "
			end
			response = string.sub(response, 1, -3)
			chat.sendChat(chatid, response)
		end
	end
end

end