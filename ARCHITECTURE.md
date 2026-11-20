Lua4RS architecture
===================

For people working on the plugin itself. For using it, see the
[README](README.md). For the functions and events scripts get, see
[docs/lua-api.md](docs/lua-api.md).

Overview
--------

```
retroshare-gui
 ├─ Lua4RSPlugin            plugin entry point, creates everything below
 ├─ p3Lua4RS (service)      RsTickingThread = the Lua worker; p3Config storage
 │   └─ LuaCore             one lua_State, queues, bindings, run/log/store logic
 │       ├─ LuaList         scripts (LuaContainer = code + LuaConfig with triggers)
 │       ├─ LuaTrustStore   approvals (level + SHA-256 per script)
 │       ├─ LuaStore        per-script key/value store
 │       └─ Lua4RSNotify    rsEvents handler -> LuaEvent
 ├─ Lua4RSWidget (GUI)      editor, script list, Automation tab, approval dialog, MCP tab
 └─ McpServer               JsonApiResourceProvider on RetroShare's JSON API server
     └─ Lua4RSTools         the MCP tools
```

| Folder | Content |
|---|---|
| `Lua/` | Core: `LuaCore`, sandbox, trust, store, event catalog, bindings (`LuaToRS*.cpp`), triggers (`Lua/Trigger/`) |
| `gui/` | Qt widgets: editor, approval dialog with line diff, MCP panel |
| `mcp/` | MCP protocol (`McpProtocol`), HTTP rules (`McpHttp`), restbed glue (`McpServer`), tools (`Lua4RSTools`) |
| `service/` | `p3Lua4RS`: the worker thread and config persistence |
| `jsonapi-runner/` | Standalone Lua runner for `jsonapi.*` scripts and the sandbox tests |
| `tests/` | Unit tests (no RetroShare needed) and the MCP smoke test |
| `tools/` | Generators: `generate-lua-docs.py` (docs/lua-api.md), `generate-api-catalog.py` (JSON API browser) |


Threads
-------

There is one `lua_State`. Only the worker thread runs Lua.

| Thread | Does |
|---|---|
| **Lua worker** (`p3Lua4RS::threadTick`) | Every tick: queue a timer event once per interval, then take *one* item from `LuaCore::processEventQueue`: a manual run from the editor first, then agent runs, then RetroShare events. Sleeps 250 ms when idle. |
| **GUI thread** | Editor, script list, saving scripts and approvals. All changes to scripts on disk happen here, including those requested over MCP. |
| **RetroShare event threads** | `Lua4RSNotify::handleEvent` turns an `RsEvent` into a `LuaEvent` and queues it. Never runs Lua. |
| **JSON API thread** (restbed) | Runs MCP requests. Read tools answer directly. Write tools hand their work to the GUI thread (`mcpRunOnThreadOf`, 5 s, then abandoned and reported as not applied). `run_script` queues an agent run and waits for the worker (15 s, abandoned if it has not started yet). |

Locks in `LuaCore`:

* `_mutex`: the event queue, the manual run and the agent runs.
* `_mutexRunLua`: held for the whole time Lua runs.
* `_scriptsMutex`: script containers and triggers. Never held while Lua runs.
  `dispatchEvent` copies name and code of the matching scripts under this lock,
  then runs the copies.
* `_logMutex`: the log ring buffer (1000 entries, read by `get_logs`).

The JSON API server has a single worker: a long tool call blocks other JSON API
requests meanwhile.


From event to script run
------------------------

1. RetroShare emits an `RsEvent`. `Lua4RSNotify` maps it to an `L4R_*` id
   (`Lua/LuaEvent.h`) with string/number parameters and calls
   `LuaCore::processEvent`.
2. The event waits in a bounded queue (1024 events, ~8 MB of parameters; new
   arrivals are dropped when full). Events never survive a restart.
3. The worker takes the event. `dispatchEvent` asks every script's triggers
   (`LuaContainer::isTriggered`, `Lua/Trigger/*`) and runs the matching ones.
4. `executeLocked` runs one script: it picks the access level
   (`trustForScript`), sets up the sandbox environment, the time and memory
   limits, the store, and an `args` table built from the event parameters.

`Lua/LuaEventCatalog.h` lists the events scripts can use, with a stable name
and the fields of `args`. The GUI's Automation tab, the MCP tools
`list_events`/`set_triggers` and the generated reference all read it. An event
that `Lua4RSNotify` delivers but the catalog does not list is not selectable.


Where access is enforced
------------------------

| Layer | Where | What it stops |
|---|---|---|
| Sandbox | `Lua/LuaSandbox.*` | Each sandboxed script gets its own `_ENV` with a copy of safe functions (no `io`, `require`, `load`, `debug`, bytecode, `__gc`). A hook enforces 5 s per run, a custom allocator 32 MB of growth. `trusted` scripts run in the global environment instead. |
| Binding check | `LuaSandbox::pushGuardedFunction` | Every native binding is wrapped. Before the call, its tag (`read`/`write`/`sensitive`) is checked against `LuaExecutionGuard::currentTrust()`. |
| Approval | `Lua/LuaTrustStore.*` | A script gets a level above `sandbox-readonly` only if the SHA-256 of its current code matches the approved hash. Approvals live in `p3Lua4RS`'s p3Config (signed and encrypted by RetroShare), so editing files on disk cannot create one. Only the GUI calls `approve`. |
| Agent runs | `Lua4RSTools` (`run_script`) | Ad-hoc code and runs with simulated `args` always run `sandbox-readonly`. Ad-hoc code gets an empty throwaway store; simulated runs drop their store changes. |
| MCP endpoint | `mcp/McpHttp.*` | Origin check (local origins only, no CORS), then the endpoint's own Bearer token, then the JSON-RPC method. The token is not a JSON API token. |

`jsonapi.call` reaches the whole JSON API. That is why it is only available
to `trusted` scripts.


Files
-----

In the `Lua4RS` folder of the RetroShare profile:

| File | Content |
|---|---|
| `<name>.lua` | Script code |
| `<name>.lua.ini` | Description, triggers, enabled flag (`LuaConfig`, QSettings) |
| `<name>.lua.approved` | Copy of the approved code, for the diff. Checked against the stored hash before use. |
| `.store/<name>.lua.json` | The script's store. Not encrypted. |

In RetroShare's encrypted config (via `p3Lua4RS`): approvals
(`L4R_TRUST:<name>` = `level;sha256;time`, `L4R_TRUST_MIGRATED`) and MCP
settings (`L4R_MCP_ENABLED`, `L4R_MCP_TOKEN`).

The bundled example scripts are compiled into the plugin
(`Lua/BundledScripts.h`). An unchanged example counts as approved.


MCP endpoint
------------

```
restbed (JSON API thread)
  McpServer      registers /lua4rs/mcp with RetroShare's JSON API server
  McpHttp        origin -> token -> HTTP method; no RetroShare or restbed types
  McpProtocol    JSON-RPC / MCP: initialize, ping, tools/list, tools/call
  Lua4RSTools    the tools; talk to LuaCore, the GUI thread or the worker
```

retroshare-gui creates its JSON API server only after the main window, and
creates a new one when the JSON API settings change. A provider is only
published when the server starts. So `McpServer::startWatching` checks every
2 s whether the provider is registered with the current instance; if not, it
registers it and restarts the JSON API.

`McpProtocol` and `McpHttp` have no RetroShare dependency, so `tests/mcp/`
tests them on their own.


Adding things
-------------

**A native function.** Write the `lua_CFunction` in the matching
`Lua/LuaToRS*.cpp` and add one line to `kBindings` in `Lua/LuaCore.cpp`:
namespace, signature with example arguments, access tag, function,
description. The tag decides who may call it:

* `R` (read): no effect on RetroShare or the network.
* `W` (write): changes something, sends something or starts network activity.
* `S` (sensitive): only reads, but things you would not hand out lightly
  (mail, certificates).

When in doubt, choose the stricter tag. The GUI's function browser, the MCP
tools and the approval dialog's list of write/sensitive calls all pick the new
entry up from the table.

**An event.** Map the `RsEvent` in `Lua4RSNotify::handleEvent` to an `L4R_*`
id, then add an entry to `Lua/LuaEventCatalog.h`: stable name, id, GUI label,
description and the `args` fields. Each RetroShare change should be delivered
by exactly one catalog event.

After either, run `python3 tools/generate-lua-docs.py` and commit the updated
`docs/lua-api.md` (`--check` tells whether it is outdated).


Tests
-----

See the [README](README.md#tests). Units under `tests/` build with plain qmake
and need no RetroShare: sandbox (through the standalone runner), approvals and
diff, store, per-script environments, MCP protocol and HTTP rules.
`tests/mcp/smoke.sh` exercises the endpoint of a running instance.
