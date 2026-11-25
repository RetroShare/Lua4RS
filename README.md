Lua4RS
======

A RetroShare plugin to automate and script things on RetroShare using the Lua language.

> **Transparency note: this revival is vibe-coded.** All changes since the
> project was picked up again (Lua 5.4/5.5 support, sandbox, approvals, MCP
> endpoint, see the commit history from `b179395` on) were written by an AI
> coding agent (Claude), directed, reviewed and tested by the maintainer.

What is RetroShare?
-------------------
RetroShare is a secure decentralised communication platform.
https://github.com/RetroShare

What is Lua4RS?
---------------
This plugin lets you interact with RetroShare from Lua. It exposes RetroShare
functions to Lua and provides a trigger system: scripts can run on a timer, at
startup/shutdown or when something happens in RetroShare, like _a friend comes
online_ or _a chat message arrives_.

Lua4RS can also be driven by a coding agent such as Claude Code over **MCP**
(Model Context Protocol): the agent reads, writes and tests scripts, which then
run on their own inside RetroShare. Everything an agent writes runs in a
read-only sandbox until you approve it. See [MCP](#mcp-let-a-coding-agent-write-scripts).

<img width="1263" height="754" alt="image" src="https://github.com/user-attachments/assets/d7ca0467-6b32-43c0-b2ed-1bf2250390a5" />


Requirements
------------
* RetroShare (current master), built with Qt 5 or 6
* Lua 5.4 or 5.5
* For the MCP endpoint: RetroShare built with the JSON API (`CONFIG+=rs_jsonapi`)

### MSYS2 (Windows):

`pacman -S mingw-w64-x86_64-lua` (MINGW64 shell) or
`pacman -S mingw-w64-ucrt-x86_64-lua` (UCRT64), matching RetroShare's toolchain

### Ubuntu:
`sudo apt install liblua5.4-dev` (or `liblua5.5-dev` where available)

### Arch Linux:
`sudo pacman -S lua54` (or `lua` for 5.5)

Build & install
---------------

Put/clone `Lua4RS` into `RetroShare/plugins/` and build it with the same qmake
options as RetroShare itself. Pass every option as its own argument:

	cd ${YOUR_DIR}/RetroShare/plugins/Lua4RS/
	qmake CONFIG+=release CONFIG+=rs_jsonapi
	make

Then place `libLua4RS.so`/`Lua4RS.dll` in your extension folder
(e.g. `~/.retroshare/extensions6/`).

**Windows:** a plugin DLL links against `libretroshare` and `retroshare-gui`,
which qmake looks for relative to the plugin's build folder. Build it inside
RetroShare's build folder (with `build.bat` that is
`<RsBuildPath>/plugins/Lua4RS`), from the MSYS2 shell of RetroShare's toolchain,
after RetroShare itself:

	mkdir -p <RsBuildPath>/plugins/Lua4RS && cd <RsBuildPath>/plugins/Lua4RS
	qmake <RetroShare source>/plugins/Lua4RS/Lua4RS.pro -spec win32-g++ CONFIG+=release CONFIG+=rs_jsonapi
	mingw32-make

Copy `Lua4RS.dll` and Lua's DLL (`lua54.dll` from the MSYS2 `bin` folder) next
to `retroshare.exe` or into the extension folder.

Build options:

| Option | Meaning |
|---|---|
| `LUA_VERSION=5.5` | Use Lua 5.5 instead of the default 5.4 (see `lua.pri`) |
| `LUA_PKGCONFIG=lua5.4` | pkg-config name, if autodetection fails |
| `CONFIG+=rs_jsonapi` | Build the MCP endpoint (needs restbed headers from the RetroShare build) |
| `RESTBED_INCLUDE_PATH=...` | Where restbed's headers are, if not `<build>/supportlibs/restbed/include` or RetroShare's restbed sources |
| `LUA_LIBS=... LUA_INCLUDEPATH=...` | Windows: Lua library and headers, if not `-llua` from the toolchain |

qmake prints which Lua it uses and whether the MCP endpoint is built.


Access levels and approvals
---------------------------

Every script runs with one of three access levels:

| Level | What the script can do |
|---|---|
| `sandbox-readonly` | Lua sandbox plus RetroShare functions that only read. Default for everything not approved. |
| `sandbox-write` | Sandbox plus functions that change RetroShare (send chat or mail, add friends or groups, start downloads, ...) or read sensitive content (mail, your certificate). |
| `trusted` | Full Lua (`io`, `os`, `require`) and `jsonapi.call`. For scripts you wrote or checked yourself. |

Every native binding is tagged `read`, `write` or `sensitive` (see the
tooltips in the function browser or [docs/lua-api.md](docs/lua-api.md)); a
central check refuses `write` and `sensitive` calls below `sandbox-write`.
`sensitive` marks functions that only read, but read things you would not hand out lightly: mail (subjects and
bodies) and your certificate. Since the agent's own runs are always
`sandbox-readonly`, an agent cannot read your mail unless you approved a script
that does. The approval dialog lists the write and sensitive functions a script
mentions.

**Approvals are tied to the exact code.** An approval stores the level and the
SHA-256 of the code in RetroShare's own signed and encrypted config, never in a
file next to the script. If the code changes, no matter who changed it (the
agent, an editor, the shell), the script runs as `sandbox-readonly` until it is
approved again.

* **Approve…** (next to Save, or a click on the warning icon in the script
  list) shows a diff against the approved version and lets you choose the
  level. The approved copy (`<name>.approved`) is checked against the stored
  hash; if it was tampered with, the dialog warns and shows the whole script.
* Saving in the editor keeps the level only if the saved version was approved
  before. Scripts you create in the editor are trusted.
* The bundled examples are not approved either: the read-only ones work in the
  sandbox, the others (e.g. the chat bot) need an approval like any script.
* On first start, scripts from older versions are listed once and stay
  sandboxed unless you approve them.
* The **Access** column shows each script's level; a warning icon marks scripts
  that lost a higher level because they changed.

### The sandbox

Sandboxed scripts get a fresh environment of safe functions (no `io`, `require`,
`load`, `debug`, no bytecode, no `__gc`, only `os.time/date/clock/difftime`).
Each script keeps its globals between runs; they are dropped when the script is
renamed or deleted. Limits per run: 5 seconds and 32 MB of memory growth.

### Windows and Linux

Scripts are UTF-8 files and work on both. Some differences come from the C
library underneath Lua:

* `os.date`: Windows only knows the C89 conversions (`%a %A %b %B %c %d %H %I
  %j %m %M %p %S %U %w %W %x %X %y %Y %z %Z %%`) and fails on others such as
  `%e`, `%F` or `%T`; it also fails for times before 1970. Sandboxed scripts
  get `%Z` (time zone name) as UTF-8 on Windows too.
* `os.clock` measures CPU time on Linux, wall-clock time on Windows.
* On Linux, numbers are converted to text with the decimal separator of the
  system language (`tostring(2.5)` can be `"2,5"`); on Windows it is always a
  dot (`string.format("%f")` too). For machine-readable text use integers or
  `jsonapi.encode`, which always writes a dot.
* `trusted` scripts: `io.open`, `os.rename` and friends take file names in the
  Windows ANSI code page, so paths with characters outside it (for example a
  user name in another script) do not work; `io.popen` opens a console window.
* Script names are compared ignoring upper and lower case (on Windows and
  macOS `foo.lua` and `Foo.lua` are the same file), and names Windows cannot
  store (`con.lua`, `nul.lua`, `com1.lua`, names with `~`, ...) are refused.
* Windows saves scripts with CRLF line endings, Linux exactly as they are.
  Moving a script with CRLF line endings to Linux (or one with CR characters
  to Windows) changes its code in memory, so it needs a new approval.

### The store

Every script has a small key-value store that survives runs and restarts:

```lua
local runs = (store.get("runs") or 0) + 1
store.set("runs", runs)            -- strings, numbers, booleans, tables; nil deletes
for _, key in ipairs(store.keys()) do print(key) end
```

It is available at every access level (it only holds the script's own data)
and is written once after a run, at most 256 KB per script, to
`Lua4RS/.store/<script>.json`. **It is not encrypted**: anyone with access to
the folder can read it, so do not put passwords or keys in it. Test runs with
simulated event data see the store but their changes are dropped; ad-hoc agent
code gets an empty throwaway store.

Known gap: Lua pattern functions (`string.find/match/gsub`) and long native
calls run in C and are not interrupted by the time limit.


MCP: let a coding agent write scripts
-------------------------------------

Lua4RS mounts an MCP endpoint into RetroShare's JSON API server
(`http://127.0.0.1:9092/lua4rs/mcp` by default). It uses the JSON API's port,
but not its users:

* The endpoint has **its own token**. It is not a JSON API token and grants
  nothing outside the endpoint, so an agent never gets direct API access.
* Requests with a non-local `Origin` are rejected; no CORS headers are sent,
  so web pages cannot call it.

**Set up**

1. Enable the JSON API in RetroShare: *Settings → JSON API*.
2. In Lua4RS, tab **MCP**: tick *Enable MCP endpoint*. The status shows
   *Listening on …* (right after RetroShare starts this can take ~10 s: the
   JSON API is restarted to publish the endpoint).
3. *Copy command* and run it, e.g. for Claude Code:

   ```sh
   claude mcp add --transport http lua4rs http://127.0.0.1:9092/lua4rs/mcp --header "Authorization: Bearer <token>"
   ```

**Tools**

| Tool | Purpose |
|---|---|
| `get_environment` | Lua version, limits, access levels, what the sandbox provides |
| `list_functions`, `describe_function` | Native bindings with read/write tag and an example |
| `list_events` | Events scripts can react to and the fields of `args` |
| `list_scripts`, `read_script` | Scripts with access level, approval state, triggers, hash |
| `read_store` | What a script saved with `store.set` |
| `write_script`, `edit_script` | Create/replace a script, or replace one exact piece of text |
| `delete_script` | Delete a script that was never approved |
| `run_script` | Run a saved script or ad-hoc code once, returns its output |
| `get_logs` | Recent output, errors and run notices |
| `set_triggers` | Timer, once, startup/shutdown, events, time window |

**Rules**

* Only you approve scripts, in the GUI. Changing an approved script removes the
  approval (the old version stays in `<name>.approved`).
* The agent can only delete scripts that were never approved.
* Ad-hoc code and runs with simulated event data always run `sandbox-readonly`,
  so a test of a chat bot cannot send real messages.
* `expected_hash` protects against overwriting changes made in between; if you
  have unsaved changes to a script the agent changes, the editor asks.
* Timers set by the agent run at most every 10 seconds; scripts are limited to
  64 KB.
* Script output and event data can contain text from other people (chat
  messages, nicknames). The agent is told to treat it as data, never as
  instructions.

The protocol is MCP Streamable HTTP with JSON responses only (no SSE, no
sessions). The JSON API has a single worker, so a long `run_script` (up to the
5 s limit) blocks other JSON API requests meanwhile.


Execution and resource limits
-----------------------------

Scripts run on the plugin's worker thread, so the editor stays responsive.
Each run has a five-second cooperative Lua execution deadline; JSON API
requests respect the remaining deadline and shutdown cancellation.

The event queue holds at most 1,024 events and approximately 8 MiB of parameter
data. New arrivals are dropped when either limit is reached. Output is limited
to 200 messages and 65,536 characters per second across runs; excess output is
discarded. Invalid saved timer schedules are inactive, and the service tick
interval is always at least one second.

Scripts selected for an event are copied under a lock before execution. Saving
or deleting a script affects later events; an already selected copy may finish.
Shutdown cancels the active run and joins the worker before running shutdown
handlers and releasing the Lua state. Each shutdown handler has the same
execution deadline.


Tests
-----

None of these need a running RetroShare, except the MCP smoke test.

| Test | Build / run |
|---|---|
| Sandbox (via the standalone runner) | build `jsonapi-runner`, then `tests/sandbox/run.sh <path-to-lua4rs-jsonapi>` |
| Approvals and diff | `qmake tests/truststore/truststore.pro && make && ./tst_truststore` |
| Store | `qmake tests/store/store.pro && make && ./tst_store` |
| Triggers (timer schedule across saves) | `qmake tests/triggers/triggers.pro && make && ./tst_triggers` |
| Per-script environments | `qmake tests/env/env.pro && make && ./tst_env` (add `LUA_VERSION=5.5` to test 5.5) |
| Bundled scripts (unique names, header, size, syntax; install) | `qmake tests/bundled/bundled.pro && make && ./tst_bundled` |
| MCP protocol, HTTP rules, settings | `qmake tests/mcp/mcp.pro && make && ./tst_mcp` |
| MCP endpoint of a running instance | `tests/mcp/smoke.sh <token> [url]` |


Documentation
-------------

* [docs/lua-api.md](docs/lua-api.md): all functions and events available to
  scripts, with their access tags (generated from the code by
  `tools/generate-lua-docs.py`)
* [scripts/](scripts/README.md): the bundled scripts (ready to use, examples,
  trusted only) and scripts for the JSON API runner
* [JSON_API.md](JSON_API.md): the `jsonapi` Lua namespace and the standalone
  runner
* [ARCHITECTURE.md](ARCHITECTURE.md): how the plugin works inside, for
  contributors
