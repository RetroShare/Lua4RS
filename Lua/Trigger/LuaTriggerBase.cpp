#include "LuaTriggerBase.h"

LuaTriggerBase::LuaTriggerBase() {}

LuaTriggerBase::~LuaTriggerBase() {}

bool LuaTriggerBase::isTriggered (LuaEvent luaevent)
{
    return false;
}

void LuaTriggerBase::toSettings(QSettings& mySettings)
{
}

void LuaTriggerBase::fromSettings(const QSettings& mySettings)
{
}


