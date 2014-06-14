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
    return;
}

void LuaTriggerStartup::fromSettings (const QSettings& mySettings)
{
    return;
}



