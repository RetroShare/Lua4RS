-- Online history: remembers how many friends were online, using the script's
-- store (kept between runs and restarts), and prints a small summary.
--
-- Trigger: a timer, e.g. every hour (Automation tab), or click Run a few times.
-- Access: read-only (works in the sandbox).

-- Settings
local KEEP = 48   -- number of samples to keep

local history = store.get("history") or {}
history[#history + 1] = { time = os.time(), online = #(peers.getOnlineList() or {}) }
while #history > KEEP do table.remove(history, 1) end
store.set("history", history) -- written when the run ends

local low, high, sum = math.huge, 0, 0
for _, sample in ipairs(history) do
    low, high = math.min(low, sample.online), math.max(high, sample.online)
    sum = sum + sample.online
end

rs.clear()
rs.print(os.date("%H:%M", history[#history].time) .. ": " .. history[#history].online .. " friends online")
rs.print(string.format("Last %d samples since %s: min %d, max %d, average %.1f",
                       #history, os.date("%Y-%m-%d %H:%M", history[1].time), low, high, sum / #history))
