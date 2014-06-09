#include "LuaConfig.h"

LuaConfig::LuaConfig()
{
}

LuaConfig::~LuaConfig()
{
}


//
bool LuaConfig::isTriggered(const QDateTime& dt)
{
    if (_triggerType != ByTimer)
    {
        return false; // ...we cannot be triggered by a timebased trigger
    }

    if (_enableConstraint)
    {
        if ((dt.time() < _enableConstraintFrom) &&
            (dt.time() > _enableConstraintTo))
        {
            return false; // ...we cannot be triggered outside of the constraint intervall
        }
    }

    switch (_configForTimer._timerType)
    {
        //----------------------------------------------------------------------
        case runEvery:
        {
            return false;
        }
        //----------------------------------------------------------------------
        case runOnce:
        {
            if( QDateTime::currentDateTime() != dt )
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        //----------------------------------------------------------------------
        case runStartup: // f*c: runStartup should be a event type trigger!
        {
            return false;
        }
        //----------------------------------------------------------------------
        default:
        {
            return false;
        }
    }
}


// isTriggered
bool LuaConfig::isTriggered(int event)
{
    if (event == 0)
    {
        return false;
    }
    else
    {
        return false;
    }
}

//
int LuaConfig::Save()
{
    return -1;
}

//
int LuaConfig::Load()
{
    return -1;
}


// setTriggerType
void LuaConfig::setTriggerType(TriggerType triggertype)
{
    _triggerType = triggertype;
}

// getTriggerType
TriggerType LuaConfig::getTriggerType()
{
    return _triggerType;
}

// setScriptEnabled
void LuaConfig::setScriptEnabled(bool scriptenabled)
{
    _scriptEnabled = scriptenabled;
}

// getScriptEnabled
bool LuaConfig::getScriptEnabled()
{
    return _scriptEnabled;
}

// setEnableConstraint
void LuaConfig::setEnableConstraint(bool enableconstraint)
{
    _enableConstraint = enableconstraint;
}

// getEnableConstraint
bool LuaConfig::getEnableConstraint()
{
    return _enableConstraint;
}

// setEnableConstraintFrom
void LuaConfig::setEnableConstraintFrom(QTime& enableconstraintfrom)
{
    _enableConstraintFrom = enableconstraintfrom;
}

// getEnableConstraintFrom
QTime& LuaConfig::getEnableConstraintFrom()
{
    return _enableConstraintFrom;
}

// setEnableConstraintTo
void LuaConfig::setEnableConstraintTo(QTime& enableconstraintto)
{
    _enableConstraintTo = enableconstraintto;
}

// getEnableConstraintTo
QTime& LuaConfig::getEnableConstraintTo()
{
    return _enableConstraintTo;
}

// getConfigForTimer
LuaConfigForTimer LuaConfig::getConfigForTimer()
{
    return _configForTimer;
}

// setConfigForTimer
void LuaConfig::setConfigForTimer(LuaConfigForTimer configfortimer)
{
    _configForTimer = configfortimer;
}

// getConfigForEvent
LuaConfigForEvent LuaConfig::getConfigForEvent()
{
    return _configForEvent;
}

// setConfigForEvent
void LuaConfig::setConfigForEvent(LuaConfigForEvent configforevent)
{
    _configForEvent = configforevent;
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

