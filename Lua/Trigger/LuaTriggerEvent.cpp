#include "LuaTriggerEvent.h"

LuaTriggerEvent::LuaTriggerEvent()
{
    _classname = "LuaTriggerEvent";
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
    LuaTriggerBase::toSettings(mySettings);

//    mySettings.setValue("Classname"     , classname());
//    mySettings.setValue("LastRun"       , _lastRun);
    mySettings.setValue("EventId"       , _eventId);
}



void LuaTriggerEvent::fromSettings (const QSettings& mySettings)
{
    LuaTriggerBase::fromSettings(mySettings);

//    mySettings.value   ("Classname"     , classname());
//    mySettings.value   ("LastRun"       , _lastRun);
    mySettings.value   ("EventId"       , _eventId);
}



QString LuaTriggerEvent::classname()
{
    return _classname;
}
