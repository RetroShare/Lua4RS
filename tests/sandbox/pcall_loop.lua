-- expect-error: execution time limit
-- Catching the abort must not keep the script alive.
while true do pcall(function() while true do end end) end
