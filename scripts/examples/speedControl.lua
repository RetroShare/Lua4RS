-- Speed control: lowers your bandwidth limits during the day and raises them
-- at night.
--
-- Needs regular runs, e.g. a timer every 10 minutes (Automation tab).
-- Access: changes RetroShare settings, so approve it as "Write" first.
--         Without approval it only prints what it would do.

-- Settings (kB/s)
local SLOW = { down = 50,  up = 10 }    -- during the slow hours
local FAST = { down = 500, up = 150 }   -- the rest of the day
local SLOW_FROM  = 9    -- slow hours start at this hour (0-23) ...
local SLOW_UNTIL = 22   -- ... and end at this hour (may wrap past midnight)

local hour = tonumber(os.date("%H"))
local slow
if SLOW_FROM <= SLOW_UNTIL then
    slow = hour >= SLOW_FROM and hour < SLOW_UNTIL
else -- e.g. 22 until 6
    slow = hour >= SLOW_FROM or hour < SLOW_UNTIL
end
local wanted = slow and SLOW or FAST

local down, up = config.getMaxDataRates()
if down == wanted.down and up == wanted.up then return end -- nothing to change

rs.print(string.format("%02d:00, %s hours: setting limits to down %d kB/s, up %d kB/s",
                       hour, slow and "slow" or "fast", wanted.down, wanted.up))
local ok, err = pcall(config.setMaxDataRates, wanted.down, wanted.up)
if not ok then rs.print("Not changed: " .. tostring(err)) end
