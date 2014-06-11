#include "LuaConfig.h"
#include "LuaTriggerBase.h"

LuaConfig::LuaConfig() {}
LuaConfig::~LuaConfig() {}


// Test all triggers if one or more are triggered by <luaevent>
bool LuaConfig::isTriggered(const LuaEvent luaevent)
{
    return false;
}

// save - returns 0 if save was successful
int LuaConfig::save()
{
    return -1;
}

// load - returns 0 if load was successful
int LuaConfig::load()
{
    return -1;
}

void addTrigger(LuaTriggerBase& trigger)
{

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
void LuaConfig::setConstraintFrom(QTime& constraintfrom)
{
    _constraintFrom = constraintfrom;
}

// getConstraintFrom
QTime& LuaConfig::getConstraintFrom()
{
    return _constraintFrom;
}

// setConstraintTo
void LuaConfig::setConstraintTo(QTime& constraintto)
{
    _constraintTo = constraintto;
}

// getConstraintTo
QTime& LuaConfig::getConstraintTo()
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

