#include "LuaConfig.h"

#define INI_KEY_CLASSNAME   "Classname"
#define INI_KEY_DESC        "Description"
#define INI_KEY_TRIGGER     "Trigger"

LuaConfig::LuaConfig() :
    _description("")
{
}

LuaConfig::~LuaConfig()
{
    for (int i=0 ; i<_myTriggers.size() ; ++i)
    {
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
            // if at least one trigger returns true then we *are* triggered.
            if ( _myTriggers.at(i)->isTriggered (luaevent) )
            {
                // lets remember the time we've been triggered
                _lastTriggered = QDateTime().currentDateTime();
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
    LuaTriggerBase* atrigger;
    QStringList     childGroups;
    QString         childGroup;
    QString         className;

    // first get description from ini
    _description = mySettings.value(INI_KEY_DESC, "").toString();

    // then fetch all groups of ini file
    childGroups = mySettings.childGroups();

    // for all groups of ini file do ...
    foreach (childGroup, childGroups)
    {
        // open current group
        mySettings.beginGroup(childGroup);

        // is it a trigger?
        if (childGroup.startsWith(INI_KEY_TRIGGER))
        {
            className = mySettings.value(INI_KEY_CLASSNAME).toString();
            if(className == "")
            {
                std::cerr << "[Lua] LuaConfig::fromSettings() : empty trigger classname found in : [" << childGroup.toStdString() << "]" << std::endl;
                continue;
            }

            if (     className == LUA_TRIGGER_TIMER_INTERVAL) { atrigger = new LuaTriggerTimerInterval(); }
            else if (className == LUA_TRIGGER_STARTUP)        { atrigger = new LuaTriggerStartup(); }
            else if (className == LUA_TRIGGER_SHUTDOWN)       { atrigger = new LuaTriggerShutdown(); }
            else if (className == LUA_TRIGGER_ONCE)           { atrigger = new LuaTriggerOnce(); }
            else if (className == LUA_TRIGGER_EVENT)          { atrigger = new LuaTriggerEvent(); }
            else
            {
                std::cerr << "[Lua] LuaConfig::fromSettings() : unknown trigger class : '" << className.toStdString() << "' found in [" << childGroup.toStdString() << "]" << std::endl;
                atrigger = 0;
            }

            // if a trigger has been created, tell him to load his parms
            // from mySettings now and add him to our trigger list
            if (atrigger != 0)
            {
                std::cerr << "[Lua] LuaConfig::fromSettings() : trigger : '" << className.toStdString() << "' added." << std::endl;
                atrigger->fromSettings(mySettings);
                addTrigger(atrigger);
            }
        }
        // close current group
        mySettings.endGroup();
    }
}




// serialize this luaconfig to QSettings data
void LuaConfig::toSettings(QSettings &mySettings)
{
    // first save description to ini
    if (_description != "")
    {
        mySettings.setValue(INI_KEY_DESC, _description);
    }
    else
    {
        mySettings.setValue(INI_KEY_DESC, "It enters a description. It does so whenever it is told to.");
    }


    // now save each trigger in a group
    QString inigroup;
    for (int i = 0 ; i < _myTriggers.size() ; ++i)
    {
        inigroup.clear();
        inigroup.append("%1_%2").arg(INI_KEY_TRIGGER).arg(i);

        mySettings.beginGroup(inigroup);

        _myTriggers.at(i)->fromSettings(mySettings);

        mySettings.endGroup();
    }
}


// getter/setter
void    LuaConfig::enableScript(bool enable)                  { _enableScript = enable; }
bool    LuaConfig::isScriptEnabled()                          { return _enableScript; }

void    LuaConfig::enableConstraint(bool enable)              { _constraint = enable; }
bool    LuaConfig::isConstraintEnabled()                      { return _constraint; }

void    LuaConfig::setConstraintFrom(QTime constraintfrom)    { _constraintFrom = constraintfrom; }
QTime   LuaConfig::getConstraintFrom()                        { return _constraintFrom; }

void    LuaConfig::setConstraintTo(QTime constraintto)        { _constraintTo = constraintto; }
QTime   LuaConfig::getConstraintTo()                          { return _constraintTo; }

QString LuaConfig::getDescription()                           { return _description; }
void    LuaConfig::setDescription(const QString& description) { _description = description; }


