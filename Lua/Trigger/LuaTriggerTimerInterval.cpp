#include "LuaTriggerTimerInterval.h"

LuaTriggerTimerInterval::LuaTriggerTimerInterval () {}
LuaTriggerTimerInterval::~LuaTriggerTimerInterval () {}

bool LuaTriggerTimerInterval::isTriggered (LuaEvent luaevent)
{
    if (luaevent.eventId == L4R_TIMERTICK)
    {
        return false;
    }
    return false;
}

QString LuaTriggerTimerInterval::toSettings ()
{
    return QString("");
}

void LuaTriggerTimerInterval::fromSettings (QString &str)
{
    if (str == "")
    {
        return;
    }
}



