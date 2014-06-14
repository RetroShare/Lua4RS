#include "LuaTriggerOnce.h"

LuaTriggerOnce::LuaTriggerOnce() {}
LuaTriggerOnce::~LuaTriggerOnce() {}

bool LuaTriggerOnce::isTriggered (LuaEvent luaevent)
{
    if (luaevent.eventId == L4R_TIMERTICK)
    {
        if (luaevent.timeStamp == _onceDateTime)
        {
            return true;
        }
    }
    return false;
}

void LuaTriggerOnce::toSettings(QSettings &mySettings)
{
    return;
}

void LuaTriggerOnce::fromSettings (const QSettings& mySettings)
{
    return;
}
