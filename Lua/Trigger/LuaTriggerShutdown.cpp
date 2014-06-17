#include "LuaTriggerShutdown.h"

LuaTriggerShutdown::LuaTriggerShutdown()
{
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
    mySettings.setValue("Classname"     , classname());
    mySettings.setValue("LastRun"       , _lastRun);
}



void LuaTriggerShutdown::fromSettings (const QSettings &mySettings)
{
    mySettings.value   ("Classname"     , classname());
    mySettings.value   ("LastRun"       , _lastRun);
}

