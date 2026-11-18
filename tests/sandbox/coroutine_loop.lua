-- expect-error: execution time limit
while true do coroutine.resume(coroutine.create(function() while true do end end)) end
