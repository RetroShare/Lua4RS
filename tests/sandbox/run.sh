#!/bin/sh
# Sandbox tests. Usage: tests/sandbox/run.sh path/to/lua4rs-jsonapi
#
# Each *.lua declares in its header:
#   -- mode: readonly | write | trusted   (default readonly)
#   -- expect: ok                         or
#   -- expect-error: <substring of the error message>
runner=${1:?usage: $0 path/to/lua4rs-jsonapi}
dir=$(dirname "$0")
export LUA4RS_TIMEOUT_MS=${LUA4RS_TIMEOUT_MS:-300}
pass=0; fail=0

check() { # name rc output expected-error
    if [ -z "$4" ]; then
        [ "$2" -eq 0 ] && ok=1 || ok=0
    else
        [ "$2" -ne 0 ] && case $3 in *"$4"*) ok=1;; *) ok=0;; esac || ok=0
    fi
    if [ $ok -eq 1 ]; then pass=$((pass + 1)); echo "ok    $1"
    else fail=$((fail + 1)); echo "FAIL  $1 (rc=$2)"; printf '%s\n' "$3" | sed 's/^/      /'; fi
}

for t in "$dir"/*.lua; do
    mode=$(sed -n 's/^-- mode: *//p' "$t" | head -n 1)
    expect=$(sed -n 's/^-- expect-error: *//p' "$t" | head -n 1)
    case ${mode:-readonly} in
        trusted) out=$(timeout 30 "$runner" "$t" 2>&1); rc=$? ;;
        *)       out=$(timeout 30 "$runner" "--sandbox=${mode:-readonly}" "$t" 2>&1); rc=$? ;;
    esac
    check "$(basename "$t")" "$rc" "$out" "$expect"
done

# Precompiled chunks must be rejected (text only).
bin=$(mktemp); printf '\033Lua' > "$bin"
out=$(timeout 30 "$runner" --sandbox "$bin" 2>&1); rc=$?; rm -f "$bin"
check "binary chunk" "$rc" "$out" "attempt to load a binary chunk"

echo "$pass passed, $fail failed"
[ $fail -eq 0 ]
