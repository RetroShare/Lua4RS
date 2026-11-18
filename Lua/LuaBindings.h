#ifndef LUA_BINDINGS_H
#define LUA_BINDINGS_H

#include <string>

#include <lua.hpp>

#include "LuaTrust.h"

// One native binding (e.g. peers.getOwnId). The table of all bindings lives in
// LuaCore.cpp; LuaCore registers them at startup, the GUI shows them in its
// function browser and MCP lists them for the agent.
struct LuaBinding
{
    const char* ns;          // namespace, e.g. "peers"
    const char* signature;   // with example arguments, e.g. "isOnline(\"SSL_ID\")"
    LuaAccess access;
    lua_CFunction function;
    const char* doc;         // English; translate with QCoreApplication::translate("LuaBindings", doc)

    // "isOnline" for "isOnline(\"SSL_ID\")"
    std::string name() const
    {
        const std::string s(signature);
        return s.substr(0, s.find('('));
    }
    std::string fullName() const { return std::string(ns) + "." + name(); }
};

#endif // LUA_BINDINGS_H
