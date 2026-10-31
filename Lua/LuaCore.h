#ifndef LUACORE_H
#define LUACORE_H

#include <iostream>
#include <string.h>
#include <map>
#include <atomic>
#include <mutex>
#include <chrono>

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

class LuaCore : public QObject
{
    Q_OBJECT

public:
    LuaCore();
    ~LuaCore();

    void shutDown();
    void requestStop();
    bool queueManualRun(const QString& code);
    // Protect script containers and triggers, never hold this while executing Lua.
    std::recursive_mutex& scriptsMutex() { return _scriptsMutex; }

    void setupRsFunctionsAndTw(QTreeWidget* tw);

    void processEvent(LuaEvent *e);
    void processEvent(LuaEventList &lel);
    bool processEventQueue();

    // invoke lua
    bool runLuaByString(const QString& code);
    bool configureJsonApi(const QString& url, const QString& username,
                          const QString& password, QString& error);
    void runLuaByName(const QString& name);
    //void runLuaByNameWithParams(const QString& name, parameterMap paramMap);
    void runLuaByEvent(const QString& code, const LuaEvent& event);

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
    bool executeLocked(const QString& code);
    void dispatchEvent(const LuaEvent& event);
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

    std::recursive_mutex _scriptsMutex;
    QString _manualCode;
    bool _manualPending = false;
    std::atomic<bool> _ready{false};
    std::atomic<bool> _cancel{false};
    size_t _outputRemaining = 0;
    unsigned _outputMessages = 0;
    std::chrono::steady_clock::time_point _outputWindow;
    LuaEventQueue _events;
    // disable trigger on shutdown
    std::atomic<bool> _shutDownImminent{false};

signals:
    void manualRunFinished(bool success);
    void appendLog(const QString& s);
    void appendOutput(const QString& s);
    void clearOutput();
};

#endif // LUACORE_H
