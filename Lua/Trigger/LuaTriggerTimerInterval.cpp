#include "LuaTriggerTimerInterval.h"

LuaTriggerTimerInterval::LuaTriggerTimerInterval ()
{
    _classname = "LuaTriggerTimerInterval";
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
        if(!_lastRun.isValid())
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
    LuaTriggerBase::toSettings(mySettings);

//    mySettings.setValue("Classname"     , classname());
//    mySettings.setValue("LastRun"       , _lastRun);
    mySettings.setValue("TimerAmount"   , _timerAmount);
    mySettings.setValue("TimerUnit"     , _timerUnit);
}



void LuaTriggerTimerInterval::fromSettings (const QSettings& mySettings)
{
    LuaTriggerBase::fromSettings(mySettings);
//    mySettings.value   ("Classname"     , classname());
//    mySettings.value   ("LastRun"       , _lastRun);
    mySettings.value   ("TimerAmount"   , _timerAmount);
    mySettings.value   ("TimerUnit"     , _timerUnit);

    _timerInterval = _timerAmount * _timerUnit;

}



QString LuaTriggerTimerInterval::classname()
{
    return _classname;
}


