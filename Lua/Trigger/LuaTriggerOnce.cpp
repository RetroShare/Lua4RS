#include "LuaTriggerOnce.h"

LuaTriggerOnce::LuaTriggerOnce()
{
    _classname = "LuaTriggerOnce";
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
    LuaTriggerBase::toSettings(mySettings);

//    mySettings.setValue("Classname"     , classname());
//    mySettings.setValue("LastRun"       , _lastRun);
    mySettings.setValue("When"          , _onceDateTime);
}



void LuaTriggerOnce::fromSettings (const QSettings& mySettings)
{
    LuaTriggerBase::fromSettings(mySettings);

//    mySettings.value   ("Classname"     , classname());
//    mySettings.value   ("LastRun"       , _lastRun);
    mySettings.value   ("When"          , _onceDateTime);
}



QString LuaTriggerOnce::classname()
{
    return _classname;
}
