#ifndef LUACORE_H
#define LUACORE_H

#include <iostream>
#include <string.h>
#include <map>
#include <atomic>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <chrono>

#include <QObject>
#include <QStringList>

#include <util/rsthreads.h> // for RsMutex

#include "LuaBindings.h"
#include "LuaEvent.h"
#include "LuaRun.h"
#include "LuaSandbox.h"
#include "LuaTrust.h"
#include "LuaTrustStore.h"
#include "mcp/McpSettings.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

// The sandbox relies on _ENV; 5.1/LuaJIT/5.2/5.3 are not supported.
#if LUA_VERSION_NUM != 504 && LUA_VERSION_NUM != 505
#error "Lua4RS requires Lua 5.4 or 5.5"
#endif

typedef std::map<std::string, std::string>  parameterMap;


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
    // name: script the editor shows (env of sandboxed runs), trust: see
    // LuaTrustStore::forEditor
    bool queueManualRun(const QString& code, const QString& name, LuaTrust trust);
    // Run for the MCP agent, after the current run. Returns nullptr if the
    // queue is full or Lua is not ready. See LuaAgentRun for abandoning.
    std::shared_ptr<LuaAgentRun> queueAgentRun(const QString& name, const QString& code,
                                               LuaTrust trust, const QJsonObject* args);
    // Plugin log for get_logs: entries with id > since, optionally of one script,
    // at most limit (newest last).
    std::vector<LuaLogEntry> logEntries(quint64 since, const QString& script, int limit) const;
    // Protect script containers and triggers, never hold this while executing Lua.
    std::recursive_mutex& scriptsMutex() { return _scriptsMutex; }

    // All native bindings (registered at startup), for the GUI and MCP.
    const std::vector<LuaBinding>& bindings() const;

    void processEvent(LuaEvent *e);
    void processEvent(LuaEventList &lel);
    bool processEventQueue();

    // invoke lua
    bool runLuaByString(const QString& code, const QString& name, LuaTrust trust);
    bool configureJsonApi(const QString& url, const QString& username,
                          const QString& password, QString& error);
    void runLuaByName(const QString& name);
    //void runLuaByNameWithParams(const QString& name, parameterMap paramMap);
    void runLuaByEvent(const QString& name, const QString& code, const LuaEvent& event);

    // Trust a stored script runs with (approval + hash rule, see LuaTrustStore).
    LuaTrust trustForScript(const QString& name, const QString& code) const;
    LuaTrustStore& trustStore() { return _trust; }
    McpSettings& mcpSettings() { return _mcpSettings; }
    // Forget the sandbox globals of a script that was renamed or deleted, so a
    // later script of that name starts clean. Applied by the worker before and
    // after the next run (never blocks on a running script).
    void forgetScriptState(const QString& name);
    // Full names (e.g. "chat.sendChat") of all bindings tagged write.
    const QStringList& writeFunctions() const { return _writeFunctions; }

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
    void applyForgottenStatesLocked(); // with _mutexRunLua held
    // args: event data, simulated data (jsonArgs) or nil
    bool executeLocked(const QString& name, const QString& code, LuaTrust trust, const LuaEvent* event,
                       const QJsonObject* jsonArgs = nullptr);
    void runAgent(LuaAgentRun& run);
    // Log line for the GUI and get_logs (kind: info, error, output).
    // script: owner of the line, default: the script running now
    void logLine(const QString& kind, const QString& text, const QString& script = QString());
    void logInfo(const QString& text) { logLine("info", text); }
    void recordLog(const QString& kind, const QString& text, const QString& script = QString());
    void dispatchEvent(const LuaEvent& event);
    void reportLuaErrors(lua_State *L, int status);
    void registerBindings();

    const std::string _folderName;
    std::string _path;

    LuaSandbox::MemoryLimiter _memory;
    lua_State* L;

    RsMutex _mutex;
    RsMutex _mutexRunLua;

    LuaList* _luaList;
    Lua4RSNotify* _notify;
    Lua4RSWidget* _ui;

    std::recursive_mutex _scriptsMutex;
    LuaTrustStore _trust;
    McpSettings _mcpSettings;
    QStringList _writeFunctions;
    std::mutex _forgetMutex;
    std::vector<std::string> _forget; // script names, see forgetScriptState
    QString _manualCode;
    QString _manualName;
    LuaTrust _manualTrust = LuaTrust::SandboxReadOnly;
    bool _manualPending = false;
    std::atomic<bool> _ready{false};
    std::atomic<bool> _cancel{false};
    size_t _outputRemaining = 0;
    unsigned _outputMessages = 0;
    std::chrono::steady_clock::time_point _outputWindow;
    LuaEventQueue _events;
    std::deque<std::shared_ptr<LuaAgentRun>> _agentRuns; // under _mutex

    // Only touched by the Lua worker during a run (under _mutexRunLua).
    LuaRunResult* _capture = nullptr;
    int _captureBytes = 0;

    mutable std::mutex _logMutex;
    std::deque<LuaLogEntry> _log;
    quint64 _logNextId = 1;
    QString _runningScript; // under _logMutex
    // disable trigger on shutdown
    std::atomic<bool> _shutDownImminent{false};

signals:
    void manualRunFinished(bool success);
    void appendLog(const QString& s);
    void appendOutput(const QString& s);
    // GUI thread, for changes made by the MCP agent: before a script is
    // deleted (drop pointers to it) and after scripts changed.
    void scriptAboutToBeRemoved(const QString& name);
    void scriptChangedExternally(const QString& name);
    void clearOutput();
};

#endif // LUACORE_H
