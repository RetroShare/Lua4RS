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

QString LuaTriggerEvent::toSettings ()
{
    return QString("");
}

void LuaTriggerEvent::fromSettings (QString &str)
{
    if (str == "")
    {
        return;
    }
}
