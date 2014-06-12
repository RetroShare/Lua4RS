#include "LuaCore.h"
#include "LuaToRS.h"

extern "C" {
    // helper
    int getArgCount(lua_State* L)
    {
        return lua_gettop(L);
    }

    // functions

    int rs_print(lua_State* L)
    {
        int argc = getArgCount(L);

        if(argc > 0)
            for ( int n=1; n<=argc; ++n )
            {
                const std::string s(lua_tostring(L, n));
                LuaCore::getInstance()->getUI()->appendOutput(s);
            }

        return 0;
    }

    int rs_clear(lua_State* /*L*/)
    {
        LuaCore::getInstance()->getUI()->clearOutput();
        return 0;
    }

    int rs_initRsNamespace(lua_State* L)
    {
        lua_newtable(L);
        int top = lua_gettop(L);

        lua_pushstring(L, "print");
        lua_pushcfunction(L, rs_print);
        lua_settable(L, top);

        lua_pushstring(L, "clear");
        lua_pushcfunction(L, rs_clear);
        lua_settable(L, top);

        return 1;
    }
}
