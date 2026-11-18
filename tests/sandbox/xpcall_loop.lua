-- expect-error: execution time limit
while true do xpcall(function() while true do end end, function(e) return e end) end
