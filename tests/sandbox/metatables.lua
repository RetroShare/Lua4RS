-- expect: ok
local t = setmetatable({}, {__index = function(_, k) return k .. "!" end})
assert(t.x == "x!")
