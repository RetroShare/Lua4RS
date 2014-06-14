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
    return;
}

void LuaTriggerShutdown::fromSettings (const QSettings &mySettings)
{
    return;
}
