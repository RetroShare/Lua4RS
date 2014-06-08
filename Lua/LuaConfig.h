#ifndef LUACONFIG_H
#define LUACONFIG_H


#include <QTime>
#include <QDateTime>

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
class LuaConfigForTimer
{
public:
    /*
    script enabled/disabled
    */
    bool        _scriptEnabled;

    /*
    Timer type (=runEvery, runOnce or runStartup)
    */
    TimerType   _timerType;


    /*
    Amount and time unit. Only valid if _timerType = runEvery
    */
    uint        _runEveryAmount;
    TimeUnit    _runEveryUnit;

    /*
    DateTime for "_runOnce". Only valid if _timerType = runOnce
    */
    QDateTime   _runOnceDateTime;

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
    */
    QTime       _enableConstraintFrom;
    QTime       _enableConstraintTo;




};

//------------------------------------------------------------------------------
class LuaConfigForEvent
{
public:


};

//------------------------------------------------------------------------------
class LuaConfig
{
public:
    LuaConfig();
    ~LuaConfig();

    bool TestTriggered();

protected:
    TriggerType _triggerType;
    LuaConfigForTimer _configForTimer;
    LuaConfigForEvent _configForEvent;
};

#endif // LUACONFIG_H
