#include "LuaConfig.h"

#include "Trigger/LuaTriggerBase.h"
#include "Trigger/LuaTriggerTimerInterval.h"
#include "Trigger/LuaTriggerStartup.h"
#include "Trigger/LuaTriggerShutdown.h"
#include "Trigger/LuaTriggerEvent.h"
#include "Trigger/LuaTriggerOnce.h"




LuaConfig::LuaConfig()
{
}



LuaConfig::~LuaConfig()
{
    for (int i=0 ; i<=_myTriggers.count() ; i++)
    {
        delete _myTriggers[i];
    }
}



// Test all triggers if one or more are triggered by <luaevent>
bool LuaConfig::isTriggered(const LuaEvent luaevent)
{
    // is the script enabled at all?
    if(isScriptEnabled())
    {
        // is there a constraint on the enabled script?
        if(isConstraintEnabled())
        {
            // is the event timestamp outside the set constraint of the enabled script?
            if ( (luaevent.timeStamp.time() < getConstraintFrom()) ||
                 (luaevent.timeStamp.time() > getConstraintTo()) )
            {
                // if the time constraint is enabled and we are outside of the
                // constraint window [from..to], then no trigger can happen.
                return false;
            }
        }

        // now test each trigger if it is triggered by the event
        for (int i=0 ; i<_myTriggers.size() ; i++)
        {
            if ( _myTriggers[i]->isTriggered (luaevent) )
            {
                // lets remember the time we've been triggered
                _lastTriggered = QDateTime().currentDateTime();

                // if at least one trigger returns true then we *are* triggered.
                return true;
            }
        }
    }
    return false;
}



// addTrigger - Add a trigger to the trigger list of this LuaConfig
void LuaConfig::addTrigger(LuaTriggerBase* trigger)
{
    if (trigger != NULL)
    {
        _myTriggers.append(trigger);
    }
}



// load this luaconfig from QSettings data
void LuaConfig::fromSettings(QSettings &mySettings)
{
    // first get description from ini
    mySettings.value("Description", _description);

    // now get each trigger from its ini group
    QString iniGroup;
    QString className;
    LuaTriggerBase* atrigger;

    for (int i=0 ; _myTriggers.count() ; i++)
    {
        iniGroup.clear();
        iniGroup.append("Trigger_%1").arg(i);

        mySettings.beginGroup(iniGroup);

        // Get the classname of the current trigger
        className.clear();
        mySettings.value("Classname", className);

        if (className == LUA_TRIGGER_TIMER_INTERVAL)
        {
            atrigger = new LuaTriggerTimerInterval();
            ((LuaTriggerTimerInterval*)atrigger)->fromSettings(mySettings);
            addTrigger(atrigger);
        }
        else if (className == LUA_TRIGGER_STARTUP)
        {
            atrigger = new LuaTriggerStartup();
            ((LuaTriggerStartup*)atrigger)->fromSettings(mySettings);
            addTrigger(atrigger);
        }
        else if (className == LUA_TRIGGER_SHUTDOWN)
        {
            atrigger = new LuaTriggerShutdown();
            ((LuaTriggerShutdown*)atrigger)->fromSettings(mySettings);
            addTrigger(atrigger);
        }
        else if (className == LUA_TRIGGER_ONCE)
        {
            atrigger = new LuaTriggerOnce();
            ((LuaTriggerOnce*)atrigger)->fromSettings(mySettings);
            addTrigger(atrigger);
        }
        else if (className == LUA_TRIGGER_EVENT)
        {
            atrigger = new LuaTriggerEvent();
            ((LuaTriggerEvent*)atrigger)->fromSettings(mySettings);
            addTrigger(atrigger);
        }
        else
        {
            ; // Unknown Trigger class name;
        }

        mySettings.endGroup();
    }
}



// serialize this luaconfig to QSettings data
void LuaConfig::toSettings(QSettings &mySettings)
{
    // first save description to ini
    mySettings.setValue("Description", _description);

    // now save each trigger in a group
    QString inigroup;
    for (int i=0 ; _myTriggers.count() ; i++)
    {
        inigroup.clear();
        inigroup.append("Trigger_%1").arg(i);
        mySettings.beginGroup(inigroup);

        if (_myTriggers[i]->classname() == LUA_TRIGGER_TIMER_INTERVAL)
        {
            ((LuaTriggerTimerInterval*)_myTriggers[i])->fromSettings(mySettings);
        }
        else if (_myTriggers[i]->classname() == LUA_TRIGGER_STARTUP)
        {
            ((LuaTriggerStartup*)_myTriggers[i])->fromSettings(mySettings);

        }
        else if (_myTriggers[i]->classname() == LUA_TRIGGER_SHUTDOWN)
        {
            ((LuaTriggerShutdown*)_myTriggers[i])->fromSettings(mySettings);

        }
        else if (_myTriggers[i]->classname() == LUA_TRIGGER_ONCE)
        {
            ((LuaTriggerOnce*)_myTriggers[i])->fromSettings(mySettings);

        }
        else if (_myTriggers[i]->classname() == LUA_TRIGGER_EVENT)
        {
            ((LuaTriggerEvent*)_myTriggers[i])->fromSettings(mySettings);
        }
        else
        {
            ; // Unknown Trigger class name;
        }

        mySettings.endGroup();
    }
}



// enableScript
void LuaConfig::enableScript(bool enable)
{
    _enableScript = enable;
}



// isScriptEnabled
bool LuaConfig::isScriptEnabled()
{
    return _enableScript;
}



// enableConstraint
void LuaConfig::enableConstraint(bool enable)
{
    _constraint = enable;
}



// isConstraintEnabled
bool LuaConfig::isConstraintEnabled()
{
    return _constraint;
}



// setConstraintFrom
void LuaConfig::setConstraintFrom(QTime constraintfrom)
{
    _constraintFrom = constraintfrom;
}



// getConstraintFrom
QTime LuaConfig::getConstraintFrom()
{
    return _constraintFrom;
}



// setConstraintTo
void LuaConfig::setConstraintTo(QTime constraintto)
{
    _constraintTo = constraintto;
}



// getConstraintTo
QTime LuaConfig::getConstraintTo()
{
    return _constraintTo;
}



// getDescription
QString LuaConfig::getDescription()
{
    return _description;
}



// setDescription
void LuaConfig::setDescription(QString description)
{
    _description = description;
}


