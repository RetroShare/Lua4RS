#include "LuaTriggerEvent.h"

LuaTriggerEvent::LuaTriggerEvent() {}
LuaTriggerEvent::~LuaTriggerEvent() {}


bool LuaTriggerEvent::isTriggered (LuaEvent luaevent)
{
    if (luaevent.eventId == _eventId)
    {
        return true;
    }
    return false;
}

void LuaTriggerEvent::toSettings(QSettings &mySettings)
{
    mySettings.setValue("TriggerClass"  , classname());
    mySettings.setValue("EventId"       , _eventId);
    mySettings.setValue("LastRun"       , _lastRun);
}

void LuaTriggerEvent::fromSettings (const QSettings& mySettings)
{
    mySettings.value   ("TriggerClass"  , classname());
    mySettings.value   ("EventId"       , _eventId);
    mySettings.value   ("LastRun"       , _lastRun);
}
