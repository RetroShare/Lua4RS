-- expect: ok
-- The shared string metatable is hidden; method syntax still works.
assert(getmetatable("") == false)
assert(("abc"):upper() == "ABC")
-- Changing the env's copy of string does not touch the real string table.
string.upper = nil
assert(("abc"):upper() == "ABC")
