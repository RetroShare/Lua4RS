#!/bin/sh
# Smoke test against a running RetroShare with the Lua4RS MCP endpoint.
# Usage: tests/mcp/smoke.sh <token> [url]
# The token is shown in Lua4RS → MCP (Show).
set -u
token=${1:?usage: smoke.sh <token> [url]}
url=${2:-http://127.0.0.1:9092/lua4rs/mcp}
fail=0
tmp=$(mktemp)

post() { # post <expected-status> <description> <json> [extra curl args...]
    expected=$1; what=$2; body=$3; shift 3
    out=$(curl -s -o /dev/stderr -w '%{http_code}' -X POST "$url" \
        -H 'Content-Type: application/json' -H 'Accept: application/json, text/event-stream' \
        "$@" --data "$body" 2>"$tmp")
    if [ "$out" = "$expected" ]; then echo "ok   $what ($out)"; else echo "FAIL $what: got $out, expected $expected"; fail=1; fi
    head -c 400 "$tmp"; echo; echo
}

auth="Authorization: Bearer $token"
post 401 "no token"         '{"jsonrpc":"2.0","id":1,"method":"ping"}'
post 401 "wrong token"      '{"jsonrpc":"2.0","id":1,"method":"ping"}' -H 'Authorization: Bearer 00'
post 403 "foreign origin"   '{"jsonrpc":"2.0","id":1,"method":"ping"}' -H "$auth" -H 'Origin: http://evil.example'
post 200 "initialize"       '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-06-18","capabilities":{},"clientInfo":{"name":"smoke","version":"1"}}}' -H "$auth"
post 202 "initialized"      '{"jsonrpc":"2.0","method":"notifications/initialized"}' -H "$auth"
post 200 "tools/list"       '{"jsonrpc":"2.0","id":2,"method":"tools/list"}' -H "$auth"
post 200 "get_environment"  '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"get_environment","arguments":{}}}' -H "$auth"
post 200 "list_scripts"     '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"list_scripts","arguments":{}}}' -H "$auth"
post 200 "read_script"      '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"read_script","arguments":{"name":"chatBot.lua"}}}' -H "$auth"

# No SSE stream: GET is 405.
status=$(curl -s -o /dev/null -w '%{http_code}' -X GET "$url" -H "$auth")
[ "$status" = 405 ] && echo "ok   GET is 405" || { echo "FAIL GET: $status"; fail=1; }

rm -f "$tmp"
[ $fail = 0 ] && echo "all ok" || echo "some checks failed"
exit $fail
