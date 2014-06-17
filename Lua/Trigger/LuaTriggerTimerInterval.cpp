#include "LuaTriggerTimerInterval.h"

LuaTriggerTimerInterval::LuaTriggerTimerInterval ()
{
}



LuaTriggerTimerInterval::~LuaTriggerTimerInterval ()
{
}



bool LuaTriggerTimerInterval::isTriggered (LuaEvent luaevent)
{
    if (luaevent.eventId == L4R_TIMERTICK)
    {
        // if _lastRun was never set before,
        // then we need some very old datetime first
        if(_lastRun.isValid() == false)
        {
            _lastRun.setTime_t(0);
        }

        // is it our big moment?
        QDateTime timeWindow = _lastRun;
        timeWindow.addSecs(_timerInterval);

        if( timeWindow <= luaevent.timeStamp  )
        {
            _lastRun = QDateTime().currentDateTime();
            return true;
        }
    }
    return false;
}



void LuaTriggerTimerInterval::toSettings(QSettings& mySettings)
{
    mySettings.setValue("Classname"     , classname());
    mySettings.setValue("TimerAmount"   , _timerAmount);
    mySettings.setValue("TimerUnit"     , _timerUnit);
    mySettings.setValue("LastRun"       , _lastRun);
}



void LuaTriggerTimerInterval::fromSettings (const QSettings& mySettings)
{
    mySettings.value   ("Classname"     , classname());
    mySettings.value   ("TimerAmount"   , _timerAmount);
    mySettings.value   ("TimerUnit"     , _timerUnit);
    mySettings.value   ("LastRun"       , _lastRun);

    _timerInterval = _timerAmount * _timerUnit;

}



