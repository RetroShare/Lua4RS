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

QString LuaTriggerStartup::toSettings ()
{
    return QString("");
}

void LuaTriggerStartup::fromSettings (QString &str)
{
    if (str == "")
    {
        return;
    }
}



