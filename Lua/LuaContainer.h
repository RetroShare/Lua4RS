#ifndef LUACONTAINER_H
#define LUACONTAINER_H

#include <QDateTime>
#include <QList>
#include <QSettings>
#include <QString>
#include <QTime>

#include "LuaEvent.h"

// When a script runs automatically. Plain settings, no run state: replacing
// them (GUI save, MCP set_triggers) keeps the timer's schedule.
struct LuaTriggers
{
    bool every = false;
    uint everyAmount = 5;
    uint everyUnit = 1;     // 0 seconds, 1 minutes, 2 hours, 3 days, 4 weeks
    bool once = false;
    QDateTime onceAt;
    bool startup = false;
    bool shutdown = false;
    QList<uint> eventIds;   // also ids that are no longer in the event catalog

    bool window = false;    // only run between windowFrom and windowTo
    QTime windowFrom = QTime(0, 0);
    QTime windowTo = QTime(0, 0);

    // seconds of the repeat interval; 0 if invalid (unit or overflow)
    static quint64 intervalSeconds(uint amount, uint unit);
};

class LuaContainer
{
public:
    QString getName() const { return _name; }
    void setName(const QString& name); // adds ".lua" if missing
    QString getCode() const { return _code; }
    void setCode(const QString& code) { _code = code; }
    QString getDesc() const { return _description; }
    void setDesc(const QString& desc) { _description = desc; }

    bool getEnabled() const { return _enabled; }
    void setEnabled(bool enable) { _enabled = enable; }

    const LuaTriggers& triggers() const { return _triggers; }
    void setTriggers(const LuaTriggers& triggers) { _triggers = triggers; }
    // e.g. "Event|Startup|TimerInterval", for tooltips
    QString getTriggerNames() const;

    // Whether the event starts the script; remembers the run if so.
    bool isTriggered(const LuaEvent& event);

    // Last automatic run (any trigger); invalid if the script never ran.
    QDateTime getLastRun() const { return _lastRun; }
    // Last run of the repeat timer, its schedule; invalid if it never ran.
    QDateTime getTimerLastRun() const { return _triggers.every ? _timerLastRun : QDateTime(); }

    // The .ini next to the script (format of earlier Lua4RS versions).
    void toSettings(QSettings& settings);
    void fromSettings(QSettings& settings);

private:
    QString _name;
    QString _code;
    QString _description;
    bool _enabled = false;
    LuaTriggers _triggers;

    QDateTime _lastRun;
    QDateTime _timerLastRun;
};

#endif // LUACONTAINER_H
