#ifndef LUACORE_H
#define LUACORE_H

#include <iostream>
#include <string.h>
#include <map>

#include <QObject>

#include <retroshare/rsplugin.h>
#include <util/rsthreads.h>

#include "LuaEvent.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

typedef std::map<std::string, std::string>  parameterMap;

class QTreeWidget;
class QTreeWidgetItem;

class Lua4RSWidget;
class Lua4RSNotify;
class Lua4RSTickThread;
class LuaList;
class LuaContainer;

class LuaCore : QObject
{
    Q_OBJECT

public:
    LuaCore();
    ~LuaCore();

    static LuaCore* getInstance();
    static void shutDown();

    bool sane();

    void setupRsFunctionsAndTw(QTreeWidget* tw);

    void processEvent(const LuaEvent& e);

    // invoke lua
    void runLuaByString(const QString& code);
    void runLuaByName(const QString& name);
    //void runLuaByNameWithParams(const QString& name, parameterMap paramMap);
    void runLuaByEvent(LuaContainer* container, const LuaEvent& event);

    // getter & setter
    Lua4RSWidget* getUI();
    void setUi(Lua4RSWidget* ui);
    RsPeers *peers() const;
    void setPeers(RsPeers *peers);
    Lua4RSNotify *notify() const;
    LuaList* codeList() const;

    // signals
    void emitAppendOutput(const QString& s);
    void emitClearOutput();

private:
    void reportLuaErrors(lua_State *L, int status);
    void addFunctionToLuaAndTw(int tableTop, const std::string &namespc, QTreeWidgetItem* item, int (*f)(lua_State*), const std::string& name, const QString& hint);

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

    // disable trigger on shutdown
    bool _shutDownImminent;

signals:
    void appendLog(const QString& s);
    void appendOutput(const QString& s);
    void clearOutput();
};

#endif // LUACORE_H
