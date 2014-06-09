#include "LuaConfig.h"

LuaConfig::LuaConfig()
{
}

LuaConfig::~LuaConfig()
{
}



bool LuaConfig::TestTriggered()
{
    return false;
}

int LuaConfig::Save()
{
    return -1;
}

int LuaConfig::Load()
{
    return -1;
}


void LuaConfig::setTriggerType(TriggerType triggertype)
{
    _triggerType = triggertype;
}

TriggerType LuaConfig::getTriggerType()
{
    return _triggerType;
}

void LuaConfig::setScriptEnabled(bool scriptenabled)
{
    _scriptEnabled = scriptenabled;
}

bool LuaConfig::getScriptEnabled()
{
    return _scriptEnabled;
}

void LuaConfig::setEnableConstraintFrom(QTime& enableconstraintfrom)
{
    _enableConstraintFrom = enableconstraintfrom;
}

QTime& LuaConfig::getEnableConstraintFrom()
{
    return _enableConstraintFrom;
}

void LuaConfig::setEnableConstraintTo(QTime& enableconstraintto)
{
    _enableConstraintTo = enableconstraintto;
}

QTime& LuaConfig::getEnableConstraintTo()
{
    return _enableConstraintTo;
}

LuaConfigForTimer LuaConfig::getConfigForTimer()
{
    return _configForTimer;
}

void LuaConfig::setConfigForTimer(LuaConfigForTimer configfortimer)
{
    _configForTimer = configfortimer;
}

LuaConfigForEvent LuaConfig::getConfigForEvent()
{
    return _configForEvent;
}

void LuaConfig::setConfigForEvent(LuaConfigForEvent configforevent)
{
    _configForEvent = configforevent;
}


