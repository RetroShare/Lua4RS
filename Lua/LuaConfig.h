#ifndef LUACONFIG_H
#define LUACONFIG_H

#include "LuaEvent.h"
#include "LuaTriggerBase.h"

#include <QList>
#include <QTime>
#include <QDateTime>
#include <QString>


enum TriggerType {
    ByTimer     = 10,
    ByEvent     = 20
};

enum TimeUnit {
    Second      = 1,
    Minute      = Second * 60,
    Hour        = Minute * 60,
    Day         = Hour   * 24,
    Week        = Day    *  7,
    Month       = Week   *  4,
    Year        = Month  * 12
};

enum TimerType {
    runEvery    = 10,
    runOnce     = 20,
    runStartup  = 30
};


//------------------------------------------------------------------------------
struct LuaConfigForTimer
{
    // Timer type (=runEvery, runOnce or runStartup)
    TimerType   _timerType;

    // Amount and time unit. Only valid if _timerType = runEvery
    uint        _runEveryAmount;
    TimeUnit    _runEveryUnit;

    // DateTime for "_runOnce". Only valid if _timerType = runOnce
    QDateTime   _runOnceDateTime;
};

struct LuaConfigForEvent
{
};



//------------------------------------------------------------------------------
class LuaConfig
{
public:
    LuaConfig();
    ~LuaConfig();

    // test if triggered by this event
    bool isTriggered (LuaEvent luaevent);

    // save this LuaConfig
    int save();

    // load this LuaConfig
    int load();

    void addTrigger(LuaTriggerBase& trigger);


    // Getter/Setter

    void enableScript(bool enable);
    bool isScriptEnabled();

    void enableConstraint(bool enable);
    bool isConstraintEnabled();

    void setConstraintFrom(QTime& enableconstraintfrom);
    QTime& getConstraintFrom();

    void setConstraintTo(QTime& constraintto);
    QTime& getConstraintTo();

    QString getDescription();
    void setDescription(QString description);

protected:
    // the description field is stored in LuaConfig
    QString _description;

    // List of Trigger objects which may trigger this script
    QList <LuaTriggerBase> _mytriggers;


    // is script enabled?
    bool        _enableScript;

    // is enableConstraint active?
    bool        _constraint;

    /*
    _scriptEnabledConstraint. this overrules all TimerType's operations.

    e.g.:
        _scriptEnabled = true
        _timerType = runEvery
        _runEveryAmount = 2
        _runEveryUnit = Hour
        _enableConstraintFrom = "09:00:00"
        _enableConstraintTo = "17:00:00"

    then the script will run every 2 hours during the time from 09:00:00
    to 17:00:00. from 17:00:01 to 08:59:59 the script will not run.
    This constraint applies to event based triggers as well.
    */
    QTime       _constraintFrom;
    QTime       _constraintTo;

};

#endif // LUACONFIG_H
