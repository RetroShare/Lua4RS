#include "LuaTrigger.h"
#include "LuaEvent.h"

LuaTrigger::LuaTrigger()
{
}

bool isTriggered(LuaEvent luaevent)
{
    if (luaevent.eventId == L4R_NOEVENT)
    {
        return false;
    }
}
