#include "LuaTriggerStartup.h"

LuaTriggerStartup::LuaTriggerStartup()
{
    _classname = "LuaTriggerStartup";
}



LuaTriggerStartup::~LuaTriggerStartup()
{
}



bool LuaTriggerStartup::isTriggered (LuaEvent luaevent)
{
    if ( luaevent.eventId == L4R_STARTUP )
    {
        _lastRun = QDateTime().currentDateTime();
        return true;
    }
    return false;
}



void LuaTriggerStartup::toSettings(QSettings &mySettings)
{
    LuaTriggerBase::toSettings(mySettings);

//    mySettings.setValue("Classname"     , classname());
//    mySettings.setValue("LastRun"       , _lastRun);
}



void LuaTriggerStartup::fromSettings (const QSettings& mySettings)
{
    LuaTriggerBase::fromSettings(mySettings);

//    mySettings.value   ("Classname"     , classname());
//    mySettings.value   ("LastRun"       , _lastRun);
}



QString LuaTriggerStartup::classname()
{
    return _classname;
}
