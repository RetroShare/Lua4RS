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
    mySettings.setValue("TimerAmount"   , _timerAmount);
    mySettings.setValue("TimerUnit"     , _timerUnit);
    mySettings.setValue("LastRun"       , _lastRun);
}


void LuaTriggerTimerInterval::fromSettings (const QSettings& mySettings)
{
    mySettings.value   ("TriggerClass"  , classname());
    mySettings.value   ("TimerAmount"   , _timerAmount);
    mySettings.value   ("TimerUnit"     , _timerUnit);
    mySettings.value   ("LastRun"       , _lastRun);

    _timerInterval = _timerAmount * _timerUnit;

}



