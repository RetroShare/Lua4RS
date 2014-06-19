#include "LuaTriggerBase.h"

LuaTriggerBase::LuaTriggerBase()
{
    _classname = "LuaTriggerBase";
}



LuaTriggerBase::~LuaTriggerBase()
{
    ;
}



bool LuaTriggerBase::isTriggered (LuaEvent luaevent)
{
    std::cerr << "[Lua] isTriggered() called on LuaTriggerBase" << std::endl;
    return false;
}



void LuaTriggerBase::toSettings(QSettings& mySettings)
{
    mySettings.setValue("Classname"     , classname());
    mySettings.setValue("LastRun"       , _lastRun);

//    std::cerr << "[Lua] toSettings() called on LuaTriggerBase" << std::endl;
}



void LuaTriggerBase::fromSettings(const QSettings& mySettings)
{
    mySettings.value   ("Classname"     , classname());
    mySettings.value   ("LastRun"       , _lastRun);

//    std::cerr << "[Lua] fromSettings() called LuaTriggerBase" << std::endl;
}



QString LuaTriggerBase::classname()
{
    return _classname;
}
