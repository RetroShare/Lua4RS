-- expect-error: selftest.write: write access denied (script runs as sandbox-readonly
assert(selftest.read() == true)
selftest.write()
