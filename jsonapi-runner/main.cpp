#include <QCoreApplication>
#include <cstdio>
#include "../Lua/LuaJsonApi.h"
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    if(argc < 2)
    {
        std::fprintf(stderr, "Usage: lua4rs-jsonapi script.lua [arguments...]\n"
            "Connection: RETROSHARE_API_URL, RETROSHARE_API_USER, RETROSHARE_API_PASSWORD\n");
        return 2;
    }
    lua_State* L = luaL_newstate();
    if(!L) return 1;
    luaL_openlibs(L);
    registerLuaJsonApi(L, true);
    lua_newtable(L);
    for(int i = 1; i < argc; ++i) { lua_pushstring(L, argv[i]); lua_rawseti(L, -2, i - 1); }
    lua_setglobal(L, "arg");
    int status = luaL_loadfile(L, argv[1]);
    if(status == LUA_OK) status = lua_pcall(L, 0, LUA_MULTRET, 0);
    if(status != LUA_OK) std::fprintf(stderr, "%s\n", lua_tostring(L, -1) ? lua_tostring(L, -1) : "Lua script failed");
    lua_close(L);
    return status == LUA_OK ? 0 : 1;
}
