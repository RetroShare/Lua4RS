#include "LuaTriggerShutdown.h"

LuaTriggerShutdown::LuaTriggerShutdown()
{
    _classname = LUA_TRIGGER_SHUTDOWN;
}

LuaTriggerShutdown::~LuaTriggerShutdown() {}

bool LuaTriggerShutdown::isTriggered (const LuaEvent& luaevent)
{
    if (luaevent.eventId == L4R_SHUTDOWN)
    {
        _lastRun = QDateTime().currentDateTime();
        return true;
    }
    return false;
}

void LuaTriggerShutdown::toSettings (QSettings& mySettings)
{
    LuaTriggerBase::toSettings(mySettings);
}

void LuaTriggerShutdown::fromSettings (const QSettings &mySettings)
{
    LuaTriggerBase::fromSettings(mySettings);
}

QString LuaTriggerShutdown::classname()
{
    return _classname;
}


