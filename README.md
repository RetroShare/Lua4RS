Lua4RS
======

A RetroShare plugin to automate and script things on RetroShare using the Lua language.

What is RetroShare?
-------------------
RetroShare is a secure decentralised communication platform.
https://github.com/RetroShare

What is Lua4RS?
---------------
This plugin let you interact with RetroShare. It exposes most RetroShare functions to Lua and provides a trigger system. You can write scripts that react to events (from RetroShare) like _a friends comes online_ or _received a new mail_. With access to most RetroShare functions, you can do the same in the Lua language as you can do in C++.

Requirements
------------
* RetroShare v0.6
* Lua 5.5

### MSYS2:

`pacman -S mingw-w64-x86_64-lua`

### Ubuntu:
`sudo apt install lua5.5`

### Arch Linux:
`sudo pacman -S lua`

# Build & install
------------

put/clone `Lua4RS` to `RetroShare/plugins/` recommend

	cd ${YOUR_DIR}/RetroShare/plugins/Lua4RS/
	qmake
	make 

Then place libLua4RS.so/Lua4RS.dll in your extension folder.

## Execution and resource limits

Manual runs execute on the plugin worker so the editor stays responsive.
Each plugin script has a five-second cooperative Lua execution deadline.
JSON API requests respect the remaining deadline and shutdown cancellation.
Native functions outside the JSON API may still block; this is not a sandbox
for untrusted scripts, and Lua hooks are not a hard process-level timeout.
The standalone JSON API runner keeps its existing execution behavior.

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

