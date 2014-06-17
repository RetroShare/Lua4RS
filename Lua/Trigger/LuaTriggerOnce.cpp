#include "LuaTriggerOnce.h"

LuaTriggerOnce::LuaTriggerOnce()
{
}



LuaTriggerOnce::~LuaTriggerOnce()
{
}



bool LuaTriggerOnce::isTriggered (LuaEvent luaevent)
{
    if (luaevent.eventId == L4R_TIMERTICK)
    {
        if (luaevent.timeStamp == _onceDateTime)
        {
            _lastRun = QDateTime().currentDateTime();
            return true;
        }
    }
    return false;
}



void LuaTriggerOnce::toSettings(QSettings &mySettings)
{
    mySettings.setValue("Classname"     , classname());
    mySettings.setValue("When"          , _onceDateTime);
    mySettings.setValue("LastRun"       , _lastRun);
}



void LuaTriggerOnce::fromSettings (const QSettings& mySettings)
{
    mySettings.value   ("Classname"     , classname());
    mySettings.value   ("When"          , _onceDateTime);
    mySettings.value   ("LastRun"       , _lastRun);
}


