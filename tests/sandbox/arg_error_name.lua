-- Argument errors of a native binding name the function, not '?'.
-- expect-error: bad argument #1 to 'number'
assert(selftest.number(7) == 7)
selftest.number("not a number")
