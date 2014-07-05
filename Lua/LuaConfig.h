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

class LuaConfig
{
public:
    LuaConfig();
    ~LuaConfig();

    // test my triggers if triggered by <luaevent>
    bool isTriggered(const LuaEvent& luaevent);

    // add a trigger to our triggerlist
    void addTrigger(LuaTriggerBase* trigger);

    // gets number of triggers in _myTriggers
    uint getTriggerCount();

    // remove all trigger
    void removeAllTrigger();

    // load this luaconfig from QSettings data
    void fromSettings(QSettings& mySettings);

    // serialize this luaconfig to QSettings data
    void toSettings(QSettings& mySettings);

    // to access trigger from config
    QList<LuaTriggerBase*>::const_iterator triggersBegin();
    QList<LuaTriggerBase*>::const_iterator triggersEnd();


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
    // the description field is stored in LuaConfig
    QString _description;

    // is script enabled?
    bool _enableScript;

    // is constraint active?
    bool _constraint;

    // constraint window
    QTime _constraintFrom;
    QTime _constraintTo;

    // List of Trigger objects which may trigger this script
    QList <LuaTriggerBase*> _myTriggers;

    QDateTime _lastTriggered;
};

#endif // LUACONFIG_H
