-- expect: ok
-- A failed allocation is an ordinary error; the script may handle it.
local ok, err = pcall(string.rep, "x", 1 << 30)
assert(not ok and tostring(err):find("not enough memory"), tostring(err))
