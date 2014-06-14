#include "LuaTriggerTimerInterval.h"

LuaTriggerTimerInterval::LuaTriggerTimerInterval () {}
LuaTriggerTimerInterval::~LuaTriggerTimerInterval () {}

bool LuaTriggerTimerInterval::isTriggered (LuaEvent luaevent)
{
    if (luaevent.eventId == L4R_TIMERTICK)
    {
        return true;
    }
    return false;
}


void LuaTriggerTimerInterval::toSettings(QSettings &mySettings)
{
    mySettings.setValue("TriggerClass"  , classname());
    mySettings.setValue("TimerAmount"   , _timeramount);
    mySettings.setValue("TimerUnit"     , _timerunit);
    mySettings.setValue("LastRun"       , _lastrun);
}


void LuaTriggerTimerInterval::fromSettings (const QSettings& mySettings)
{
    mySettings.value   ("TriggerClass"     , classname());
    mySettings.value   ("TimerAmount"      , _timeramount);
    mySettings.value   ("TimerUnit"        , _timerunit);
    mySettings.value   ("LastRun"          , _lastrun);
}



