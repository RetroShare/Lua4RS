#include "LuaTriggerShutdown.h"

LuaTriggerShutdown::LuaTriggerShutdown()
{
    _classname = "LuaTriggerShutdown";
}



LuaTriggerShutdown::~LuaTriggerShutdown()
{
}



bool LuaTriggerShutdown::isTriggered (LuaEvent luaevent)
{
    if (luaevent.eventId == L4R_SHUTDOWN)
    {
        _lastRun = QDateTime().currentDateTime();
        return true;
    }
    return false;
}



void LuaTriggerShutdown::toSettings (QSettings& mySettings)
{
    LuaTriggerBase::toSettings(mySettings);

//    mySettings.setValue("Classname"     , classname());
//    mySettings.setValue("LastRun"       , _lastRun);
}



void LuaTriggerShutdown::fromSettings (const QSettings &mySettings)
{
    LuaTriggerBase::fromSettings(mySettings);

//    mySettings.value   ("Classname"     , classname());
//    mySettings.value   ("LastRun"       , _lastRun);
}



QString LuaTriggerShutdown::classname()
{
    return _classname;
}


