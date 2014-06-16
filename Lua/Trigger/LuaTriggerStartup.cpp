#include "LuaTriggerStartup.h"

LuaTriggerStartup::LuaTriggerStartup() {}
LuaTriggerStartup::~LuaTriggerStartup() {}


bool LuaTriggerStartup::isTriggered (LuaEvent luaevent)
{
    if ( luaevent.eventId == L4R_STARTUP )
    {
        return true;
    }
    return false;
}

void LuaTriggerStartup::toSettings(QSettings &mySettings)
{
    mySettings.setValue("TriggerClass"  , classname());
    mySettings.setValue("LastRun"       , _lastRun);
}

void LuaTriggerStartup::fromSettings (const QSettings& mySettings)
{
    mySettings.value   ("TriggerClass"  , classname());
    mySettings.value   ("LastRun"       , _lastRun);
}

