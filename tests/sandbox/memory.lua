-- expect-error: not enough memory
local t, s = {}, string.rep("x", 1 << 20)
for i = 1, 1e6 do t[i] = s .. i end
