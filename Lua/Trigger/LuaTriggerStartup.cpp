#include "LuaTriggerStartup.h"

LuaTriggerStartup::LuaTriggerStartup()
{
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
    mySettings.setValue("Classname"     , classname());
    mySettings.setValue("LastRun"       , _lastRun);
}



void LuaTriggerStartup::fromSettings (const QSettings& mySettings)
{
    mySettings.value   ("Classname"     , classname());
    mySettings.value   ("LastRun"       , _lastRun);
}

