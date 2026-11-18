-- expect-error: __gc metamethods are not allowed
setmetatable({}, {__gc = function() while true do end end})
