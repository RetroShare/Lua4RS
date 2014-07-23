#include "LuaToRS.h"
#include "interface/L4RInterface.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

// helper
int getArgCount(lua_State* L)
{
    return lua_gettop(L);
}

// no extern "C" for these function since you can overload them like this in C
// table
void pushTable(lua_State* L, int tableTop, const std::string& name, int value)
{
    if(name == "")
        return;

    lua_pushstring(L, name.c_str());
    lua_pushinteger(L, value);
    lua_settable(L, tableTop);
}

void pushTable(lua_State* L, int tableTop, const std::string& name, uint value)
{
    if(name == "")
        return;

    lua_pushstring(L, name.c_str());
    lua_pushunsigned(L, value);
    lua_settable(L, tableTop);
}

void pushTable(lua_State* L, int tableTop, const std::string& name, const std::string& value)
{
    if(name == "" || value == "")
        return;

    lua_pushstring(L, name.c_str());
    lua_pushstring(L, value.c_str());
    lua_settable(L, tableTop);
}

void pushTable(lua_State* L, int tableTop, const std::string& name, int (*f)(lua_State*))
{
    if(name == "" || f == NULL)
        return;

    lua_pushstring(L, name.c_str());
    lua_pushcfunction(L, f);
    lua_settable(L, tableTop);
}

// array
void pushArray(lua_State* L, int tableTop, int index, int value)
{
    if(index < 1)
        return;

    lua_pushinteger(L, index);
    lua_pushinteger(L, value);
    lua_settable(L, tableTop);
}

void pushArray(lua_State* L, int tableTop, int index, uint value)
{
    if(index < 1)
        return;

    lua_pushinteger(L, index);
    lua_pushunsigned(L, value);
    lua_settable(L, tableTop);
}

void pushArray(lua_State* L, int tableTop, int index, const std::string& value)
{
    if(index < 1 || value == "")
        return;

    lua_pushinteger(L, index);
    lua_pushstring(L, value.c_str());
    lua_settable(L, tableTop);
}

extern "C" {
    // functions

    int rs_print(lua_State* L)
    {
        int argc = getArgCount(L);

        if(argc > 0)
            for ( int n=1; n<=argc; ++n )
            {
                const std::string s(lua_tostring(L, n));
                emit L4R::L4RConfig->getCore()->emitAppendOutput(QString::fromStdString(s));
            }

        return 0;
    }

    int rs_clear(lua_State* /*L*/)
    {
        emit L4R::L4RConfig->getCore()->emitClearOutput();
        return 0;
    }
}
