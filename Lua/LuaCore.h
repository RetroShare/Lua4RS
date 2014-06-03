#ifndef LUACORE_H
#define LUACORE_H

#include <iostream>
#include <string.h>
#include <map>

#include <retroshare/rsplugin.h>

#include "LuaCode.h"
#include "../Lua4RSNotify.h"
#include "../Lua4RSTickThread.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

typedef std::map<std::string, std::string>  parameterMap;
typedef std::map<std::string, LuaCode>      codeMap;

class Lua4RSMainWidget;

class LuaCore
{
public:
    LuaCore();
    ~LuaCore();

    static LuaCore* getInstance();
    static void shutDown();

    // invoke lua
    void runLuaByString(const std::string& code);
    void runLuaByName(const std::string& name);
    void runLuaByNameWithParams(const std::string& name, parameterMap paramMap);

    // getter & setter
    Lua4RSMainWidget* getUI();
    void setUi(Lua4RSMainWidget* ui);
    RsPeers *peers() const;
    void setPeers(RsPeers *peers);
    Lua4RSNotify *notify() const;

private:
    void reportLuaErrors(lua_State *L, int status);

    static LuaCore* _instance;
    lua_State* L;

    Lua4RSMainWidget* _ui;
    Lua4RSNotify* _notify;

    Lua4RSTickThread* _thread;

    RsPeers* _peers;

    codeMap _codeMap;
};

#endif // LUACORE_H
