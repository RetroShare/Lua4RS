-- expect: ok
-- Dangerous globals are gone, safe ones are there.
for _, name in ipairs{"io", "debug", "package", "require", "load", "loadstring",
                      "dofile", "loadfile", "collectgarbage"} do
    assert(_ENV[name] == nil, name .. " must not be available")
end
assert(string.dump == nil, "string.dump")
for _, name in ipairs{"execute", "exit", "getenv", "remove", "rename", "tmpname", "setlocale"} do
    assert(os[name] == nil, "os." .. name)
end
for _, name in ipairs{"call", "configure", "subscribe"} do
    assert(jsonapi[name] == nil, "jsonapi." .. name)
end
assert(os.time and os.date and os.clock and os.difftime)
assert(string.format and table.concat and math.floor and utf8.char and coroutine.create)
assert(jsonapi.encode and jsonapi.decode and jsonapi.array and jsonapi.null)
assert(_G == _ENV)
assert(type(_VERSION) == "string")
