#include "LuaTriggerBase.h"

LuaTriggerBase::LuaTriggerBase()
{
}



LuaTriggerBase::~LuaTriggerBase()
{
}



bool LuaTriggerBase::isTriggered (LuaEvent luaevent)
{
    return false;
}



void LuaTriggerBase::toSettings(QSettings& mySettings)
{
    std::cerr << "toSettings() called on abstract base class : LuaTriggerBase" << std::endl;
}



void LuaTriggerBase::fromSettings(const QSettings& mySettings)
{
    std::cerr << "fromSettings() called on abstract base class : LuaTriggerBase" << std::endl;
}


