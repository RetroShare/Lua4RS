#include "LuaTriggerBase.h"

LuaTriggerBase::LuaTriggerBase()
{
}



LuaTriggerBase::~LuaTriggerBase()
{
}



bool LuaTriggerBase::isTriggered (LuaEvent luaevent)
{
    std::cerr << "[lua] isTriggered() called on abstract base class : LuaTriggerBase" << std::endl;
    return false;
}



void LuaTriggerBase::toSettings(QSettings& mySettings)
{
    std::cerr << "[lua] toSettings() called on abstract base class : LuaTriggerBase" << std::endl;
}



void LuaTriggerBase::fromSettings(const QSettings& mySettings)
{
    std::cerr << "[lua] fromSettings() called on abstract base class : LuaTriggerBase" << std::endl;
}


