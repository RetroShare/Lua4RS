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

tool() { # tool ok|error <description> <tool> <arguments-json>
    want=$1; what=$2; name=$3; args=$4
    body=$(curl -s -X POST "$url" -H 'Content-Type: application/json' -H "$auth" \
        --data "{\"jsonrpc\":\"2.0\",\"id\":9,\"method\":\"tools/call\",\"params\":{\"name\":\"$name\",\"arguments\":$args}}")
    case "$body" in
        *'"isError":false'*) got=ok ;;
        *'"isError":true'*) got=error ;;
        *) got="no result" ;;
    esac
    if [ "$got" = "$want" ]; then echo "ok   $what ($got)"; else echo "FAIL $what: got $got, expected $want"; fail=1; fi
    echo "$body" | head -c 300; echo; echo
}

# Write tools on a throwaway script. Leaves nothing behind if all goes well.
s=mcp-smoke.lua
tool ok    "write_script creates"        write_script "{\"name\":\"$s\",\"code\":\"print('a')\\nprint('b')\\n\",\"description\":\"smoke test\"}"
tool ok    "edit_script"                 edit_script  "{\"name\":\"$s\",\"old_text\":\"print('b')\",\"new_text\":\"print('c')\"}"
tool error "edit_script, text missing"   edit_script  "{\"name\":\"$s\",\"old_text\":\"print('zzz')\",\"new_text\":\"x\"}"
tool error "write_script, stale hash"    write_script "{\"name\":\"$s\",\"code\":\"x\",\"expected_hash\":\"0000\"}"
tool error "write_script, bad name"      write_script '{"name":"../evil.lua","code":"x"}'
tool error "delete shipped example"      delete_script '{"name":"chatBot.lua"}'
tool ok    "run_script saved"            run_script   "{\"name\":\"$s\"}"
tool ok    "store: write via script"     write_script "{\"name\":\"$s\",\"code\":\"store.set('runs', (store.get('runs') or 0) + 1)\\nprint(store.get('runs'))\"}"
tool ok    "store: run saved script"     run_script   "{\"name\":\"$s\"}"
tool ok    "store: test run (dropped)"   run_script   "{\"name\":\"$s\",\"args\":{}}"
tool ok    "read_store (runs = 1)"       read_store   "{\"name\":\"$s\"}"
tool error "read_store, unknown script"  read_store   '{"name":"nope.lua"}'
tool ok    "run_script code"             run_script   '{"code":"print(_VERSION, 1+1)"}'
tool ok    "run_script with args"        run_script   '{"code":"print(args.msg)","args":{"msg":"hello"}}'
tool ok    "run_script, Lua error"       run_script   '{"code":"error(\"boom\")"}'
tool ok    "run_script, write denied"    run_script   '{"code":"chat.sendChat(\"x\", \"y\")"}'
tool ok    "run_script, endless loop"    run_script   '{"code":"while true do end"}'
tool error "run_script, name + code"     run_script   "{\"name\":\"$s\",\"code\":\"x\"}"
tool ok    "get_logs"                    get_logs     '{"limit":5}'
tool ok    "list_events"                 list_events  '{}'
tool ok    "set_triggers"                set_triggers "{\"name\":\"$s\",\"enabled\":false,\"every\":{\"amount\":1,\"unit\":\"hours\"},\"events\":[\"friend_online\",\"chat_message\"],\"time_window\":{\"from\":\"08:00\",\"to\":\"22:00\"}}"
tool error "set_triggers, every 5 s"     set_triggers "{\"name\":\"$s\",\"every\":{\"amount\":5,\"unit\":\"seconds\"}}"
tool error "set_triggers, unknown event" set_triggers "{\"name\":\"$s\",\"events\":[\"nope\"]}"
tool error "set_triggers, once in past"  set_triggers "{\"name\":\"$s\",\"once\":\"2000-01-01T00:00:00\"}"
tool ok    "delete_script"               delete_script "{\"name\":\"$s\"}"
tool error "read deleted script"         read_script  "{\"name\":\"$s\"}"

# No SSE stream: GET is 405.
status=$(curl -s -o /dev/null -w '%{http_code}' -X GET "$url" -H "$auth")
[ "$status" = 405 ] && echo "ok   GET is 405" || { echo "FAIL GET: $status"; fail=1; }

rm -f "$tmp"
[ $fail = 0 ] && echo "all ok" || echo "some checks failed"
exit $fail
