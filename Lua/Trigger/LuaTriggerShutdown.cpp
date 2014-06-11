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

QString LuaTriggerShutdown::toSettings ()
{
    return QString("");
}

void LuaTriggerShutdown::fromSettings (QString &str)
{
    if (str == "")
    {
        return;
    }
}
