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

QString LuaTriggerOnce::toSettings ()
{
    return QString("");
}

void LuaTriggerOnce::fromSettings (QString &str)
{
    if (str == "")
    {
        return;
    }
}
