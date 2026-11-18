-- expect: ok
-- Normal error handling keeps working.
local ok, err = pcall(error, "boom")
assert(not ok and err == "boom")
assert(select(2, xpcall(error, function(e) return "handled " .. e end, "x")) == "handled x")
-- yield across the pcall wrapper (needs the continuation in the wrapper)
local co = coroutine.wrap(function()
    pcall(function() coroutine.yield(1) end)
    return 2
end)
assert(co() == 1 and co() == 2)
