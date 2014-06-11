#include "LuaTriggerTimerInterval.h"
#include "LuaEvent.h"

LuaTriggerTimerInterval::LuaTriggerTimerInterval() {}
LuaTriggerTimerInterval::~LuaTriggerTimerInterval() {}

bool LuaTriggerTimerInterval::isTriggered(LuaEvent luaevent)
{
    if (luaevent.eventId == L4R_TIMERTICK)
    {
        return false;
    }
    return false;
}
