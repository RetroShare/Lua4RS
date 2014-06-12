#ifndef LUATORS_H
#define LUATORS_H

#include "LuaCore.h"

extern "C" {
    int rs_print(lua_State* L);
    int rs_clear(lua_State* L);
}

int inline getArgCount(lua_State* L);

void pushTable(lua_State* L, int tableTop, const std::string& name, int value);
void pushTable(lua_State* L, int tableTop, const std::string& name, uint value);
void pushTable(lua_State* L, int tableTop, const std::string& name, const std::string& value);
void pushTable(lua_State* L, int tableTop, const std::string& name, int (*f)(lua_State*));

void pushArray(lua_State* L, int tableTop, int index, int value);
void pushArray(lua_State* L, int tableTop, int index, uint value);
void pushArray(lua_State* L, int tableTop, int index, const std::string& value);

#endif // LUATORS_H
