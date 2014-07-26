-- change here
-- upload speed
minUL = 10
maxUL = 150

-- download speed
minDL = 50
maxDL = 500

-- time (in hours 0-23)
slowTime = 9
powerTime = 22

-- change done

rs.clear()
 
if slowTime == powerTime or slowTime < 0 or slowTime >= 24 or powerTime < 0 or powerTime >= 24 then
	rs.print("Please choose proper values")
	return
end

swap = false
if slowTime > powerTime then
	swap = true
end

-- determine if dl/up speed must be slowed down
slow = false
now = tonumber( os.date("%H") )
if now >= slowTime then
	slow = true
end

if now >= powerTime then
	slow = false
end

if swap then
	slow = not slow
end

rs.print("Current hour is " .. os.date("%H") .. " - slowTime goes from " .. slowTime .. " to " .. powerTime .. " -> reduce dl/ul speed: " .. tostring( slow ))

if slow then
	config.setMaxDataRates(minDL, minUL)
else
	config.setMaxDataRates(maxDL, maxUL)
end

down, up = config.getMaxDataRates()
rs.print("New limits are: down=" .. down .. "kB up=" .. up .. "kB")