#pragma once

struct lua_State;

// Independent of RetroShare headers and libraries; also used by the CLI runner.
void registerLuaJsonApi(lua_State* state, bool standalone = false);
