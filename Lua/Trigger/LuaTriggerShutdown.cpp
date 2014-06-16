#include "LuaTriggerShutdown.h"

LuaTriggerShutdown::LuaTriggerShutdown() {}
LuaTriggerShutdown::~LuaTriggerShutdown() {}


bool LuaTriggerShutdown::isTriggered (LuaEvent luaevent)
{
    if (luaevent.eventId == L4R_SHUTDOWN)
    {
        return true;
    }
    return false;
}

void LuaTriggerShutdown::toSettings (QSettings& mySettings)
{
    mySettings.setValue("TriggerClass"  , classname());
    mySettings.setValue("LastRun"       , _lastRun);
}

void LuaTriggerShutdown::fromSettings (const QSettings &mySettings)
{
    mySettings.value   ("TriggerClass"  , classname());
    mySettings.value   ("LastRun"       , _lastRun);
}

