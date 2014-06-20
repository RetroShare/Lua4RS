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
}



void LuaTriggerBase::fromSettings(const QSettings& mySettings)
{
    _classname = mySettings.value("Classname", "").toString();
    _lastRun = mySettings.value("LastRun", "").toDateTime();
}



QString LuaTriggerBase::classname()
{
    return _classname;
}
