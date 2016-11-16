function getVersionNumber( s ) 
	b, e = string.find(s, "Revision")

	if not b then return false end

	e = e + 2

	-- revision starts at e and is up to 8 characters long
	rev = s:sub(e, e + 7)

	return rev, true
end

function getName( id )
	return peers.getPeerName( id )
end

--------------------------------------
-- Insert value of any type into array
--------------------------------------
local function arrayInsert( ary, val, idx )
    -- Needed because table.insert has issues
    -- An "array" is a table indexed by sequential
    -- positive integers (no empty slots)
    local lastUsed = #ary + 1
    local nextAvail = lastUsed + 1

    -- Determine correct index value
    local index = tonumber(idx) -- Don't use idx after this line!
    if (index == nil) or (index > nextAvail) then
        index = nextAvail
    elseif (index < 1) then
        index = 1
    end

    -- Insert the value
    if ary[index] == nil then
        ary[index] = val
    else
        -- TBD: Should we try to allow for skipped indices?
        for j = nextAvail,index,-1 do
            ary[j] = ary[j-1]
        end
        ary[index] = val
    end
end

--------------------------------
-- Compare two items of any type
--------------------------------
local function compareAnyTypes( op1, op2 ) -- Return the comparison result
    -- Inspired by http://lua-users.org/wiki/SortedIteration
    local type1, type2 = type(op1),     type(op2)
    local num1,  num2  = tonumber(op1), tonumber(op2)
    
    if ( num1 ~= nil) and (num2 ~= nil) then  -- Number or numeric string
        return  num1 < num2                     -- Numeric compare
    elseif type1 ~= type2 then                -- Different types
        return type1 < type2                    -- String compare of type name
    -- From here on, types are known to match (need only single compare)
    elseif type1 == "string"  then            -- Non-numeric string
        return op1 < op2                        -- Default compare
    elseif type1 == "boolean" then
        return op1                              -- No compare needed!
         -- Handled above: number, string, boolean
    else -- What's left:   function, table, thread, userdata
        return tostring(op1) < tostring(op2)  -- String representation
    end
end

-------------------------------------------
-- Iterate over a table in sorted key order
-------------------------------------------
local function pairsByKeys (tbl, func)
    -- Inspired by http://www.lua.org/pil/19.3.html
    -- and http://lua-users.org/wiki/SortedIteration

    if func == nil then
        func = compareAnyTypes
    end

    -- Build a sorted array of the keys from the passed table
    -- Use an insertion sort, since table.sort fails on non-numeric keys
    local ary = {}
    local lastUsed = 0
    for key --[[, val--]] in pairs(tbl) do
        if (lastUsed == 0) then
            ary[1] = key
        else
            local done = false
            for j=1,lastUsed do  -- Do an insertion sort
                if (func(key, ary[j]) == true) then
                    arrayInsert( ary, key, j )
                    done = true
                    break
                end
            end
            if (done == false) then
                ary[lastUsed + 1] = key
            end
        end
        lastUsed = lastUsed + 1
    end

    -- Define (and return) the iterator function
    local i = 0                -- iterator variable
    local iter = function ()   -- iterator function
        i = i + 1
        if ary[i] == nil then
            return nil
        else
            return ary[i], tbl[ary[i]]
        end
    end
    return iter
end

function getCommitNumber( hash )
	local handle = io.popen("cd ~/Projects/RetroShare; git describe --tags " .. hash)
	-- local handle = io.popen("cd /tmp/RetroShare; git describe " .. hash)
	local result = handle:read("*a")
	handle:close()
	-- rs.print(result)
	-- rs.print(tostring(""..string.find(result, ".+-.+-(.+)-.+")))
	for i in string.gmatch(result, ".+-?.+-(.+)-.+") do
		return i
	end
	return 0
end

function getCurrentGitTag()
	local handle = io.popen("cd ~/Projects/RetroShare; git describe --tags")
	-- local handle = io.popen("cd /tmp/RetroShare; git describe")
	local result = handle:read("*a")
	handle:close()
	-- rs.print(result)
	-- rs.print(tostring(""..string.find(result, ".+-.+-(.+)-.+")))
	for i in string.gmatch(result, "(.+-?.+)-.+-.+") do
		return i
	end
	return 0
end

msg = args.msg
chatid = args.chatid

-- rs.print("got message from lobby " .. chatid .. " user: " .. args.nick .. " msg: " .. msg)
-- rs.print("msg is " .. string.len(msg) .. " char(s) long")


if msg ~= nil and  string.len(msg) <= 150 then
	if msg == "!versions" then
		local friends = peers.getFriendList()
		local revList = {}
		local revListNum = {}
		local revListHash = {}
		local numFriends = 0

		for i = 1 , #friends do
			local f = friends[i]
			local revStr = disc.getPeerVersion(f)

			if revStr ~= nil then
				numFriends = numFriends + 1
				local revHash, sane = getVersionNumber(revStr)
				if sane then
					local revNum = "" .. getCommitNumber(revHash)
					if revNum ~= "0" then
						revListHash[revNum] = "g" .. revHash
					else
						if revHash == "01234567" then
							revNum = "001"
							revListHash[revNum] = "not set (01234567)"
						else
							revNum = "000"
							revListHash[revNum] = "unkown (nonuniform version string)"
						end
					end
					table.insert(revList, revNum .. " - " .. getName(f))
					if revListNum[revNum] ~= nil then
						revListNum[revNum] = revListNum[revNum] + 1
					else
						revListNum[revNum] = 1
					end
				end
			end
		end

		toSend = numFriends .. " discovery entries were found"
		toSend = toSend .. "<br>current tag: <b>" .. getCurrentGitTag() .. "</b> - showing #commits since tag"
		-- build shiny table
		toSend = toSend .. "<table>"
			toSend = toSend .. "<tr>"
				toSend = toSend .. "<th>#commits</th>"
				toSend = toSend .. "<th>&nbsp;git hash</th>"
				toSend = toSend .. "<th>times seen</th>"
			toSend = toSend .. "<tr>"
		for key, value in pairsByKeys(revListNum) do
			toSend = toSend .. "<tr>"
				-- add revision with leading 'r'
				toSend = toSend .. "<td>r" .. key .. "</td>"
				-- add git hash (leading 'g' is already inclded)
				toSend = toSend .. "<td>" .. revListHash[key] .. "</td>"
				-- add number of occurrences
				toSend = toSend .. "<td>" .. value .. "</td>"
			toSend = toSend .. "</tr>"
		end
		toSend = toSend .. "</table>"
		chat.sendChat(chatid, toSend)
	end
end
