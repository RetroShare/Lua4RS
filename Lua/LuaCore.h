#ifndef LUACORE_H
#define LUACORE_H

#include <iostream>
#include <string.h>
#include <map>

#include <retroshare/rsplugin.h>

#include "LuaCode.h"

extern "C" {
#include "lua5.2/lua.h"
#include "lua5.2/lualib.h"
#include "lua5.2/lauxlib.h"
}

class Lua4RSWidget;

class LuaCore
{
public:
    LuaCore();
    ~LuaCore();

    static LuaCore* getInstance();
    void runLua(const std::string& code);

    // getter & setter
    Lua4RSWidget* getUI();
    void setUi(Lua4RSWidget* ui);
    RsPeers *peers() const;
    void setPeers(RsPeers *peers);

private:
    void reportLuaErrors(lua_State *L, int status);

    static LuaCore* _instance;
    Lua4RSWidget* _ui;
    lua_State* L;

    std::map<std::string, LuaCode> _codeMap;

    RsPeers* _peers;
};

#endif // LUACORE_H
