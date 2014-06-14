#include "LuaTriggerEvent.h"

LuaTriggerEvent::LuaTriggerEvent() {}
LuaTriggerEvent::~LuaTriggerEvent() {}


bool LuaTriggerEvent::isTriggered (LuaEvent luaevent)
{
    if (luaevent.eventId == _eventid)
    {
        return true;
    }
    return false;
}

void LuaTriggerEvent::toSettings(QSettings &mySettings)
{
    return;
}

void LuaTriggerEvent::fromSettings (const QSettings& mySettings)
{
    return;
}
