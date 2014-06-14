#include "LuaConfig.h"
#include "Trigger/LuaTriggerBase.h"

LuaConfig::LuaConfig()
{
}

LuaConfig::~LuaConfig()
{
}


// Test all triggers if one or more are triggered by <luaevent>
bool LuaConfig::isTriggered(const LuaEvent luaevent)
{
    if(isScriptEnabled())
    {
        if(isConstraintEnabled())
        {
            if ( (luaevent.timeStamp.time() < getConstraintFrom()) || (luaevent.timeStamp.time() > getConstraintTo()) )
            {
                // if the time constraint is enabled and we are outside of the
                // constraint window [from..to], then no trigger can happen.
                return false;
            }
        }

        LuaTriggerBase currentTrigger;

        for (int i = 0; i < _myTriggers.size(); ++i)
        {
            currentTrigger = _myTriggers.at(i);

            if ( currentTrigger.isTriggered (luaevent) )
            {
                // lets remember the time we've been triggered
                _lastTriggered = QDateTime::fromMSecsSinceEpoch(QDateTime::currentMSecsSinceEpoch());

                // if at least one trigger returns true then we *are* triggered.
                return true;
            }
        }
    }
    return false;
}

// addTrigger - Add a trigger to the trigger list of this LuaConfig
void LuaConfig::addTrigger(LuaTriggerBase& trigger)
{
    _myTriggers.append(trigger);
}


// load this luaconfig from QSettings data
void LuaConfig::fromSettings(QSettings &mySettings)
{

    for (int i=0 ; _myTriggers.count() ; i++)
    {
//        mySettings.("Trigger_%d", i);
        _myTriggers[i].fromSettings(mySettings);
//        mySettings.endGroup();
    }
}

// serialize this luaconfig to QSettings data
void LuaConfig::toSettings(QSettings &mySettings)
{
    QString group;

    for (int i=0 ; _myTriggers.count() ; i++)
    {
        group = "";
        group.append("Trigger_%1").arg(i);

        mySettings.beginGroup(group);
        _myTriggers[i].toSettings(mySettings);
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


