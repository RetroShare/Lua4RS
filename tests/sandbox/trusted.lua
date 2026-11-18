-- mode: trusted
-- expect: ok
-- Trusted scripts keep the full standard library (previous behaviour).
assert(io and os.execute and load and debug)
