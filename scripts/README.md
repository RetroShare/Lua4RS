Lua4RS scripts
==============

All scripts in `ready/`, `examples/` and `trusted/` are compiled into the
plugin and listed under *Example* in the Lua4RS window, one submenu per folder
(`scripts.qrc`). Choosing one opens it as a new script; save it to keep it.
Only `examples/` is also installed into a new profile (once).

A script opened from the menu is a new script of your own: saved in the
editor, it is trusted like anything you create there. The copies installed
into a new profile are not approved: they run read-only until you approve
them. Scripts that need *write* access print an "access denied" message or
only show what they would do until then.

| Folder | Menu | Content |
|---|---|---|
| `ready/` | Ready to use | Maintained scripts for everyday use. Check the settings at the top, then approve them. |
| `examples/` | Examples | Small scripts that show how Lua4RS works. Each one runs as it is. |
| `trusted/` | Trusted only | Scripts that run shell commands or use the JSON API: they only work as `trusted`. |
| `jsonapi/` | – | Scripts for the standalone JSON API runner. See [JSON_API.md](../JSON_API.md). |

Each script says at the top how to run it. Where it needs an event (a chat
message, a mail), it names that trigger; otherwise it suggests a timer or
clicking Run. Do not add more triggers than that: each one is another run.

Function and event reference: [docs/lua-api.md](../docs/lua-api.md).

## ready/

| Script | What it does | Access |
|---|---|---|
| `autoPermissions.lua` | Sets which friends may use which service (files, chat, forums, ...) by rules: id, group, hidden node, address, profile or location name. Unmatched friends get the defaults. Starts with `DRY_RUN = true` | write |
| `buryTheDead.lua` | Moves friends offline for 30+ days into the group `Graveyard`, and back out when they connect again | write |
| `syncFriends.lua` | Keeps the friend lists of your own nodes in sync via RetroShare mail (see below). Starts with `DRY_RUN = true` | write |

### syncFriends.lua

Each node mails its **complete** friend list (a short invite for each
location) as JSON to the identities in `SEND_TO`, so late or duplicate mails
do no harm. Lists from identities in `ACCEPT_FROM` are applied: missing
friends are added and, with `REMOVE_FRIENDS = true`, friends the partner
removed are removed. Processed sync mails are moved to the trash; other mail
is not touched. Partners silent for `SILENT_AFTER` (3 days) count as offline
and only get a list once per `PROBE_SECONDS` (a week).

1. Set `SEND_AS`, `SEND_TO` and `ACCEPT_FROM` to real identity ids. With the
   placeholders nothing is sent and every incoming list is ignored.
2. Approve it as *Write*: it reads mail and invites and changes your friends.
3. Triggers: the event "A mail arrives", and a timer (e.g. every hour).
4. Check the log while `DRY_RUN` is enabled, then set it to `false`.

A test run with `args.selftest = { from = ..., fromType = ..., message = ... }`
simulates a received list. It is always read-only and never sends mail.

## examples/

| Script | What it does | Access |
|---|---|---|
| `friendsVersionOverview.lua` | Lists the RetroShare versions your friends report | read-only |
| `ipOverview.lua` | Counts your friends' IPv4 and IPv6 addresses and connections | read-only |
| `offlineChecker.lua` | Lists friend locations offline for 14+ days | read-only |
| `onlineHistory.lua` | Records how many friends are online, using the store | read-only |
| `chatBot.lua` | Answers chat messages by simple rules (event: chat message) | write |
| `discOverviewBot.lua` | Answers `!versions` in a chat with your friends' versions (event: chat message) | write |
| `simpleDownloadBot.lua` | Starts downloads requested in chat by allowed friends (event: chat message) | write |
| `speedControl.lua` | Lowers bandwidth limits during the day (timer) | write |

## trusted/

| Script | What it does |
|---|---|
| `chatWrapper.lua` | Runs a local command and sends its output to a chat |
| `resetPermissions.lua` | Resets all service permissions to the defaults (JSON API) |
| `servicePermission.lua` | Blocks services during set hours each day (JSON API, timer) |

The two permission scripts need RetroShare's development service-control
endpoints. For rule-based permissions without the JSON API, see
`ready/autoPermissions.lua`.

## jsonapi/

| Script | What it does |
|---|---|
| `jsonapi-friends.lua` | Minimal JSON API example: list friends |
| `jsonapi-events.lua` | Prints RetroShare events (`jsonapi.subscribe`, runner only) |
| `login.lua` | Lists profiles, logs in or shuts down a RetroShare service (runner only) |
