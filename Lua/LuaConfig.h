#ifndef LUACONFIG_H
#define LUACONFIG_H

#include "Trigger/LuaTriggerBase.h"

#include <QList>
#include <QTime>
#include <QDateTime>
#include <QString>
#include <QSettings>


enum TimeUnit {
    Second      = 1,
    Minute      = Second * 60,
    Hour        = Minute * 60,
    Day         = Hour   * 24,
    Week        = Day    *  7,
    Month       = Week   *  4,
    Year        = Month  * 12
};



class LuaConfig
{
public:
    LuaConfig();
    ~LuaConfig();

    // test my triggers if triggered by <luaevent>
    bool isTriggered (LuaEvent luaevent);

    // add a trigger to our triggerlist
    void addTrigger(LuaTriggerBase& trigger);


    // load this luaconfig from QSettings data
    void fromSettings(QSettings& mySettings);

    // serialize this luaconfig to QSettings data
    void toSettings(QSettings& mySettings);


    // Getter/Setter

    void enableScript(bool enable);
    bool isScriptEnabled();

    void enableConstraint(bool enable);
    bool isConstraintEnabled();

    void setConstraintFrom(QTime enableconstraintfrom);
    QTime getConstraintFrom();

    void setConstraintTo(QTime constraintto);
    QTime getConstraintTo();

    QString getDescription();
    void setDescription(QString description);

protected:
    // is script enabled?
    bool _enableScript;

    // is constraint active?
    bool _constraint;

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
    QTime _constraintFrom;
    QTime _constraintTo;

    // the description field is stored in LuaConfig
    QString _description;

    // List of Trigger objects which may trigger this script
    QList <LuaTriggerBase> _myTriggers;

    QDateTime _lastTriggered;
};

#endif // LUACONFIG_H
