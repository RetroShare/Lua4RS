#ifndef LUATORS_H
#define LUATORS_H

#include "LuaCore.h"

extern "C" {
    int inline getArgCount(lua_State* L);
    int rs_print(lua_State* L);
    int rs_clear(lua_State* L);
}
#endif // LUATORS_H
