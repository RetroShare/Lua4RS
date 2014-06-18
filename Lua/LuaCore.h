#ifndef LUACORE_H
#define LUACORE_H

#include <iostream>
#include <string.h>
#include <map>

#include <retroshare/rsplugin.h>

#include "LuaList.h"
#include "../Lua4RSNotify.h"
#include "../Lua4RSTickThread.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

typedef std::map<std::string, std::string>  parameterMap;

class Lua4RSWidget;
class QTreeWidget;
class QTreeWidgetItem;

class LuaCore
{
public:
    LuaCore();
    ~LuaCore();

    static LuaCore* getInstance();
    static void shutDown();

    bool sane() { return _ui != NULL && _peers != NULL; }

    void setupRsFunctionsAndTw(QTreeWidget* tw);

    void processEvent(LuaEvent& e);

    // invoke lua
    void runLuaByString(const std::string& code);
    void runLuaByName(const std::string& name);
    void runLuaByNameWithParams(const std::string& name, parameterMap paramMap);

    // getter & setter
    Lua4RSWidget* getUI();
    void setUi(Lua4RSWidget* ui);
    RsPeers *peers() const;
    void setPeers(RsPeers *peers);
    Lua4RSNotify *notify() const;
    LuaList* codeList() const;

private:
    void reportLuaErrors(lua_State *L, int status);
    void addFunctionToLuaAndTw(int tableTop, int (*f)(lua_State*), const std::string& name, const std::string& hint, const std::string &namespc, QTreeWidgetItem* item);

    static LuaCore* _instance;

    const std::string _folderName;
    std::string _path;

    lua_State* L;

    RsMutex _mutex;
    RsPeers* _peers;

    LuaList* _luaList;
    Lua4RSNotify* _notify;
    Lua4RSTickThread* _thread;
    Lua4RSWidget* _ui;
};

#endif // LUACORE_H
