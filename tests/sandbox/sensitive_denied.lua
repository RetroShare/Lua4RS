-- expect-error: selftest.sensitive: sensitive access denied (script runs as sandbox-readonly
assert(selftest.read() == true)
selftest.sensitive()
