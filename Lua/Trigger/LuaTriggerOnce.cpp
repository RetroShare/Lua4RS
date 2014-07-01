#include "LuaTriggerOnce.h"

LuaTriggerOnce::LuaTriggerOnce()
{
    _classname = "LuaTriggerOnce";
}

LuaTriggerOnce::LuaTriggerOnce(QDateTime &onceDateTime)
{
    _classname = "LuaTriggerOnce";
    _onceDateTime = onceDateTime;
}


LuaTriggerOnce::~LuaTriggerOnce() {}



bool LuaTriggerOnce::isTriggered (const LuaEvent& luaevent)
{
    if (luaevent.eventId == L4R_TIMERTICK)
    {
        if (luaevent.timeStamp == _onceDateTime) // f*c: this is too strict. we have to hit the exact second here.
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
    mySettings.setValue(INI_KEY_WHEN, _onceDateTime);
}



void LuaTriggerOnce::fromSettings (const QSettings& mySettings)
{
    LuaTriggerBase::fromSettings(mySettings);
    _onceDateTime = mySettings.value(INI_KEY_WHEN, "").toDateTime();
}



QString LuaTriggerOnce::classname()
{
    return _classname;
}
