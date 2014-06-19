#include "LuaConfig.h"



LuaConfig::LuaConfig()
{
}



LuaConfig::~LuaConfig()
{

// *** draft ***

    for (int i=0 ; i<_myTriggers.size() ; ++i)
    {
/*
        if (     _myTriggers[i]->classname() == LUA_TRIGGER_TIMER_INTERVAL) {
            delete ((LuaTriggerTimerInterval*)_myTriggers.at(i));
        }
        else if (_myTriggers[i]->classname() == LUA_TRIGGER_STARTUP) {
            delete ((LuaTriggerStartup*)_myTriggers.at(i));
        }
        else if (_myTriggers[i]->classname() == LUA_TRIGGER_SHUTDOWN) {
            delete ((LuaTriggerShutdown*)_myTriggers.at(i));
        }
        else if (_myTriggers[i]->classname() == LUA_TRIGGER_ONCE) {
            delete ((LuaTriggerOnce*)_myTriggers.at(i));
        }
        else if (_myTriggers[i]->classname() == LUA_TRIGGER_EVENT) {
            delete ((LuaTriggerEvent*)_myTriggers.at(i));
        }
        else {
            std::cerr << "[Lua] LuaConfig::dtor() : unknown Trigger class :" << _myTriggers[i]->classname().toStdString() << std::endl;
        }
*/
        delete _myTriggers.at(i);
    }
    _myTriggers.clear();
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
        for (int i=0 ; i<_myTriggers.size() ; ++i)
        {
            // f*c tbd : welches isTriggered wird hier aufgerufen? das
            // von LuaTriggerBase?? ich hoffe nicht!
            if ( _myTriggers.at(i)->isTriggered (luaevent) )
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
    else
    {
        std::cerr << "[Lua] LuaConfig::addTrigger() : tried to add an invalid trigger obj." << std::endl;
    }
}



// load this luaconfig from QSettings data
void LuaConfig::fromSettings(QSettings &mySettings)
{

// *** draft ***

    // first get description from ini
    mySettings.value("Description", _description);

    // now get each trigger from its ini group
    QString iniGroup;
    QString className;
    LuaTriggerBase* atrigger;

    for (int i=0 ; i<_myTriggers.size() ; ++i)
    {
        iniGroup.clear();
        iniGroup.append("Trigger_%1").arg(i);

        mySettings.beginGroup(iniGroup);

        // Get the classname of the current trigger
        className.clear();
        mySettings.value("Classname", className);
/*
        if (     className == LUA_TRIGGER_TIMER_INTERVAL) {
            atrigger = new LuaTriggerTimerInterval();
            if (atrigger) {
                ((LuaTriggerTimerInterval*)atrigger)->fromSettings(mySettings);
                addTrigger(atrigger);
            }
        }
        else if (className == LUA_TRIGGER_STARTUP) {
            atrigger = new LuaTriggerStartup();
            if (atrigger) {
                ((LuaTriggerStartup*)atrigger)->fromSettings(mySettings);
                addTrigger(atrigger);
            }
        }
        else if (className == LUA_TRIGGER_SHUTDOWN) {
            atrigger = new LuaTriggerShutdown();
            if (atrigger) {
                ((LuaTriggerShutdown*)atrigger)->fromSettings(mySettings);
                addTrigger(atrigger);
            }
        }
        else if (className == LUA_TRIGGER_ONCE) {
            atrigger = new LuaTriggerOnce();
            if (atrigger) {
                ((LuaTriggerOnce*)atrigger)->fromSettings(mySettings);
                addTrigger(atrigger);
            }
        }
        else if (className == LUA_TRIGGER_EVENT) {
            atrigger = new LuaTriggerEvent();
            if (atrigger) {
                ((LuaTriggerEvent*)atrigger)->fromSettings(mySettings);
                addTrigger(atrigger);
            }
        }
        else {
            std::cerr << "[Lua] LuaConfig::fromSettings() : unknown Trigger class :" << className.toStdString() << std::endl;
        }
*/

        LuaTriggerBase* atrigger = 0;

        if (     className == LUA_TRIGGER_TIMER_INTERVAL) { atrigger = new LuaTriggerTimerInterval(); }
        else if (className == LUA_TRIGGER_STARTUP)        { atrigger = new LuaTriggerStartup(); }
        else if (className == LUA_TRIGGER_SHUTDOWN)       { atrigger = new LuaTriggerShutdown(); }
        else if (className == LUA_TRIGGER_ONCE)           { atrigger = new LuaTriggerOnce(); }
        else if (className == LUA_TRIGGER_EVENT)          { atrigger = new LuaTriggerEvent(); }
        else {
            std::cerr << "[Lua] LuaConfig::fromSettings() : unknown Trigger class :" << className.toStdString() << std::endl;
        }

        if (atrigger)
        {
            atrigger->fromSettings(mySettings);
            addTrigger(atrigger);
        }

        mySettings.endGroup();
    }
}



// serialize this luaconfig to QSettings data
void LuaConfig::toSettings(QSettings &mySettings)
{

// *** draft ***

    // first save description to ini
    if (_description != "")
    {
        mySettings.setValue("Description", _description);
    }
    else
    {
        mySettings.setValue("Description", "It enters a description. It does so whenever it is told to.");
    }

    // now save each trigger in a group
    QString inigroup;
    for (int i=0 ; i<_myTriggers.size() ; ++i)
    {
        inigroup.clear();
        inigroup.append("Trigger_%1").arg(i);

        mySettings.beginGroup(inigroup);
/*
        if (     _myTriggers[i]->classname() == LUA_TRIGGER_TIMER_INTERVAL) {
            ((LuaTriggerTimerInterval*)_myTriggers.at(i))->fromSettings(mySettings);
        }
        else if (_myTriggers[i]->classname() == LUA_TRIGGER_STARTUP) {
            ((LuaTriggerStartup*)_myTriggers.at(i))->fromSettings(mySettings);
        }
        else if (_myTriggers[i]->classname() == LUA_TRIGGER_SHUTDOWN) {
            ((LuaTriggerShutdown*)_myTriggers.at(i))->fromSettings(mySettings);
        }
        else if (_myTriggers[i]->classname() == LUA_TRIGGER_ONCE) {
            ((LuaTriggerOnce*)_myTriggers.at(i))->fromSettings(mySettings);
        }
        else if (_myTriggers[i]->classname() == LUA_TRIGGER_EVENT) {
            ((LuaTriggerEvent*)_myTriggers.at(i))->fromSettings(mySettings);
        }
        else {
            std::cerr << "[Lua] LuaConfig::toSettings() : unknown Trigger class :" << _myTriggers[i]->classname().toStdString() << std::endl;
        }
*/
        _myTriggers.at(i)->fromSettings(mySettings);

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


