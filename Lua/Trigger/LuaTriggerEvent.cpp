#include "LuaTriggerEvent.h"

LuaTriggerEvent::LuaTriggerEvent()
{
}



LuaTriggerEvent::~LuaTriggerEvent()
{
}



bool LuaTriggerEvent::isTriggered (LuaEvent luaevent)
{
    if (luaevent.eventId == _eventId)
    {
        _lastRun = QDateTime().currentDateTime();
        return true;
    }
    return false;
}



void LuaTriggerEvent::toSettings(QSettings &mySettings)
{
    mySettings.setValue("Classname"     , classname());
    mySettings.setValue("EventId"       , _eventId);
    mySettings.setValue("LastRun"       , _lastRun);
}



void LuaTriggerEvent::fromSettings (const QSettings& mySettings)
{
    mySettings.value   ("Classname"     , classname());
    mySettings.value   ("EventId"       , _eventId);
    mySettings.value   ("LastRun"       , _lastRun);
}
