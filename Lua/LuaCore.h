#ifndef LUACORE_H
#define LUACORE_H

#include <iostream>
#include <string.h>
#include <map>

#include <retroshare/rsplugin.h>

#include "LuaCode.h"

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

private:
    static LuaCore* _instance;
    Lua4RSMainWidget* _ui;
    lua_State* L;

    std::map<std::string, LuaCode> _codeMap;

    RsPeers* _peers;
};

#endif // LUACORE_H
