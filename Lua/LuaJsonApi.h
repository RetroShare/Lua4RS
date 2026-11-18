#pragma once

struct lua_State;
class QJsonValue;
class QString;

// Independent of RetroShare headers and libraries; also used by the CLI runner.
void registerLuaJsonApi(lua_State* state, bool standalone = false);

// Pushes a JSON value as Lua value (as jsonapi.decode does). Returns false and
// pushes nothing if it is nested too deeply or has inexact numbers.
bool pushLuaJson(lua_State* state, const QJsonValue& value, QString& error);
