#ifndef LUACORE_H
#define LUACORE_H

#include <iostream>
#include <string.h>
#include <map>

#include <QObject>

#include <util/rsthreads.h> // for RsMutex

#include "LuaEvent.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
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

    void shutDown();

    void setupRsFunctionsAndTw(QTreeWidget* tw);

    void processEvent(LuaEvent *e);
    void processEvent(LuaEventList &lel);
    bool processEventQueue();

    // invoke lua
    void runLuaByString(const QString& code);
    void runLuaByName(const QString& name);
    //void runLuaByNameWithParams(const QString& name, parameterMap paramMap);
    void runLuaByEvent(LuaContainer* container, const LuaEvent& event);

    // getter & setter
    Lua4RSWidget* getUI();
    void setUi(Lua4RSWidget* ui);
    Lua4RSNotify *notify() const;
    LuaList* codeList() const;
    std::string getPath() const;

    // signals
    void emitAppendOutput(const QString& s);
    void emitClearOutput();

private:
    void reportLuaErrors(lua_State *L, int status);
    void addFunctionToLuaAndTw(int tableTop, const std::string &namespc, QTreeWidgetItem* item, int (*f)(lua_State*), const std::string& name, const QString& hint);

    const std::string _folderName;
    std::string _path;

    lua_State* L;

    RsMutex _mutex;
    RsMutex _mutexRunLua;

    LuaList* _luaList;
    Lua4RSNotify* _notify;
    Lua4RSWidget* _ui;

    bool _processingEvent;
    LuaEventList _eventList;
    // disable trigger on shutdown
    bool _shutDownImminent;

signals:
    void appendLog(const QString& s);
    void appendOutput(const QString& s);
    void clearOutput();
};

#endif // LUACORE_H
