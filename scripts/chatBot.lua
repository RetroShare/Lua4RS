--[[
structue:
	["matchName"] = {
		["enabled"] = true, ["only"] = true, ["case"] = false,
		["response"] = "pong",
		["matches"] = {
			[1] = "ping",
			[2] = "PING",
			[3] =  ...
		}
	},

	matchName: the name of the matching rule
	enabled: is this rule active?
	only: match to whole message (only = true) or only check if the message contains one match (only = false)
	case: case sensitive?
	response: what to respond
	matches: array with all strings to match

	__nick__ will be replaced by the sender's nickname
]]--

-- edit here

matchingTable = {
	["ping"] = {
		["enabled"] = true, ["only"] = true, ["case"] = false,
		["response"] = "pong",
		["matches"] = {
			[1] = "ping",
		}
	},
	["pong"] = {
		["enabled"] = false, ["only"] = true, ["case"] = false,
		["response"] = "ping",
		["matches"] = {
			[1] = "pong",
		}
	},
	["test"] = {
		["enabled"] = true, ["only"] = true, ["case"] = false,
		["response"] = "@__nick__: test back :P",
		["matches"] = {
			[1] = "test",
		}
	},
	["Lua4RS"] = {
		["enabled"] = true, ["only"] = false, ["case"] = false,
		["response"] = "\\o/",
		["matches"] = {
			[1] = "Lua4RS",
		}
	},
	["info"] = {
		["enabled"] = true, ["only"] = true, ["case"] = true,
		["response"] = "I'm just a Lua script! You can find me here: https://github.com/sehraf/Lua4RS",
		["matches"] = {
			[1] = "!info",
		}
	},
}

botPrefix = "[Lua4RS]: "

-- list all matching rules (set to nill to disable)
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
			-- check for case senitivity
			msgCopy = msg
			if not matchValue["case"] then msgCopy = string.lower(msgCopy) end

			-- triggered?
			triggered = false
			matches = matchValue["matches"]
			for i = 1, #matches do
				-- check for match
				if matchValue["only"] then
					if msgCopy == matches[i] then triggered = true end
				else
					if string.find(msgCopy, matches[i]) ~= nil then triggered = true end
				end
			end

			if triggered then
				-- respond!
				rs.print("rule " .. matchName .. " triggered")
				response = matchValue["response"]
				response, _ = string.gsub(response, "__nick__", args.nick)
				chat.sendChat(chatid, botPrefix .. response)
			end
		end
	end

	-- !rules command
	if rulesCommandMatch ~= nill then
		if msg == rulesCommandMatch then
			response = botPrefix .. "rules known: "
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