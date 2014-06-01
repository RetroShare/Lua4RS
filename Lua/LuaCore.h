#ifndef LUACORE_H
#define LUACORE_H

#include <iostream>
#include <string.h>
#include <map>

#include <retroshare/rsplugin.h>

#include "LuaCode.h"
#include "../Lua4RSNotify.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class Lua4RSMainWidget;

class LuaCore
{
public:
    LuaCore();
    ~LuaCore();

    static LuaCore* getInstance();
    void runLua(const std::string& code);

    // getter & setter
    Lua4RSMainWidget* getUI();
    void setUi(Lua4RSMainWidget* ui);
    RsPeers *peers() const;
    void setPeers(RsPeers *peers);
    Lua4RSNotify *notify() const;
    // void setNotify(Lua4RSNotify *notify); no need for a setter

private:
    void reportLuaErrors(lua_State *L, int status);

    static LuaCore* _instance;
    lua_State* L;

    Lua4RSMainWidget* _ui;
    Lua4RSNotify* _notify;

    RsPeers* _peers;

    std::map<std::string, LuaCode> _codeMap;
};

#endif // LUACORE_H
