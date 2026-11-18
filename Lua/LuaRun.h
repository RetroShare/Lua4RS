#ifndef LUA_RUN_H
#define LUA_RUN_H

#include <atomic>
#include <future>

#include <QDateTime>
#include <QJsonObject>
#include <QString>
#include <QStringList>

#include "LuaTrust.h"

// Outcome of one run requested by the MCP agent.
struct LuaRunResult
{
    bool ok = false;
    QString error;            // Lua error text if !ok
    QStringList output;       // print / rs.print of this run
    bool outputTruncated = false;
    qint64 durationMs = 0;
};

// A run requested by the MCP agent; the Lua worker executes it. The requester
// may abandon it before it starts (phase Pending -> Abandoned), so a timed out
// request never runs later.
struct LuaAgentRun
{
    enum Phase { Pending, Running, Abandoned };

    QString name;             // script name, or the ad-hoc environment name
    QString code;
    LuaTrust trust = LuaTrust::SandboxReadOnly;
    bool hasArgs = false;     // simulated event data for `args`
    QJsonObject args;

    std::atomic<int> phase{ Pending };
    std::promise<LuaRunResult> result;
};

// One line of the plugin log kept for get_logs.
struct LuaLogEntry
{
    quint64 id;
    QDateTime time;
    QString script;           // empty if not from a script run
    QString kind;             // "output", "error" or "info"
    QString text;
};

#endif // LUA_RUN_H
