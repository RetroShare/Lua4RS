#ifndef LUACONFIG_H
#define LUACONFIG_H

#include <iostream>

#include "Trigger/LuaTriggerBase.h"
#include "Trigger/LuaTriggerTimerInterval.h"
#include "Trigger/LuaTriggerStartup.h"
#include "Trigger/LuaTriggerShutdown.h"
#include "Trigger/LuaTriggerEvent.h"
#include "Trigger/LuaTriggerOnce.h"

#include <QList>
#include <QTime>
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QSettings>


#define LUA_TRIGGER_TIMER_INTERVAL      "LuaTriggerTimerInterval"
#define LUA_TRIGGER_STARTUP             "LuaTriggerStartup"
#define LUA_TRIGGER_SHUTDOWN            "LuaTriggerShutdown"
#define LUA_TRIGGER_ONCE                "LuaTriggerTimerOnce"
#define LUA_TRIGGER_EVENT               "LuaTriggerEvent"

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
    bool isTriggered(const LuaEvent& luaevent);

    // add a trigger to our triggerlist
    void addTrigger(LuaTriggerBase* trigger);

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
    void setDescription(const QString &description);

    QDateTime getLastTriggered() const;
    void setLastTriggered(const QDateTime &lastTriggered);

protected:
    // is script enabled?
    bool _enableScript;

    // is constraint active?
    bool _constraint;

    // constraint window
    QTime _constraintFrom;
    QTime _constraintTo;

    // the description field is stored in LuaConfig
    QString _description;

    // List of Trigger objects which may trigger this script
    QList <LuaTriggerBase*> _myTriggers;

    QDateTime _lastTriggered;
};

#endif // LUACONFIG_H
