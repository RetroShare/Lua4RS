Lua4RS scripts
==============

| Folder | Content |
|---|---|
| `examples/` | Bundled with the plugin: installed into a new profile once and listed under *Example → Bundled scripts*. Each one runs as it is. |
| `jsonapi/` | Scripts that use the JSON API (`jsonapi.call`): for the standalone runner, or as `trusted` scripts in the plugin. See [JSON_API.md](../JSON_API.md). |
| `more/` | Useful scripts that need configuration or `trusted` access before they do anything. Copy them into your `Lua4RS` folder and edit them there. |

Function and event reference: [docs/lua-api.md](../docs/lua-api.md).

## examples/

| Script | What it does | Access |
|---|---|---|
| `friendsVersionOverview.lua` | Lists the RetroShare versions your friends report | read-only |
| `ipOverview.lua` | Counts your friends' IPv4 and IPv6 addresses and connections | read-only |
| `offlineChecker.lua` | Lists friend locations offline for 14+ days | read-only |
| `onlineHistory.lua` | Records how many friends are online (timer), using the store | read-only |
| `chatBot.lua` | Answers chat messages by simple rules (event: chat message) | write |
| `speedControl.lua` | Lowers bandwidth limits during the day (timer) | write |

Read-only scripts work in the sandbox. Scripts that need *write* access print
an "access denied" message until you approve them.

## jsonapi/

| Script | What it does |
|---|---|
| `jsonapi-friends.lua` | Minimal JSON API example: list friends |
| `jsonapi-events.lua` | Prints RetroShare events (`jsonapi.subscribe`, runner only) |
| `login.lua` | Lists profiles, logs in or shuts down a RetroShare service (runner only) |
| `resetPermissions.lua` | Resets all service permissions to the defaults |
| `servicePermission.lua` | Switches service permissions on a daily schedule |

The two permission scripts need RetroShare's development service-control
endpoints. For rule-based permissions without the JSON API, see
`more/autoPermissions.lua`.

## more/

| Script | What it does | Needs |
|---|---|---|
| `autoPermissions.lua` | Sets which friends may use which service (files, chat, forums, ...) by rules: group, hidden node, address, name. Unmatched friends get the defaults; previews first | write |
| `discOverviewBot.lua` | Answers `!versions` in a lobby with your friends' versions | write |
| `simpleDownloadBot.lua` | Starts downloads requested in chat by allowed friends | an allow list; write |
| `chatWrapper.lua` | Runs a local command and sends its output to a chat | trusted (runs shell commands) |

More scripts that are useful on their own live in
[Lua4RS-Scripts](https://github.com/sehraf/Lua4RS-Scripts), e.g.
**SyncFriends** (keeps the friend lists of your own nodes in sync via
RetroShare mail) and **buryTheDead** (moves friends offline for a long time
into a separate group).
