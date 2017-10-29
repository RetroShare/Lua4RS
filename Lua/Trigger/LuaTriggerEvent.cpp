#include "LuaTriggerEvent.h"

LuaTriggerEvent::LuaTriggerEvent()
{
    _classname = LUA_TRIGGER_EVENT;
}

LuaTriggerEvent::LuaTriggerEvent(uint eventId)
{
    _classname = LUA_TRIGGER_EVENT;
    _eventId = eventId;
}

LuaTriggerEvent::~LuaTriggerEvent()
{
}

bool LuaTriggerEvent::isTriggered (const LuaEvent& luaevent)
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
    LuaTriggerBase::toSettings(mySettings);
    mySettings.setValue("EventId", _eventId);
}

void LuaTriggerEvent::fromSettings (const QSettings& mySettings)
{
    LuaTriggerBase::fromSettings(mySettings);
    _eventId = mySettings.value   ("EventId", "").toUInt();
}

QString LuaTriggerEvent::classname()
{
    return _classname;
}

uint LuaTriggerEvent::getEventId()
{
    return _eventId;
}
