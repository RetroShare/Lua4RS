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
            _lastRun = QDateTime(QDate(1970, 1, 1),QTime(0, 0, 0));
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

    mySettings.setValue(INI_KEY_TIMER_AMOUNT,_timerAmount);
    mySettings.setValue(INI_KEY_TIMER_UNIT,_timerUnit);
}



void LuaTriggerTimerInterval::fromSettings (const QSettings& mySettings)
{
    LuaTriggerBase::fromSettings(mySettings);
    _timerAmount = mySettings.value(INI_KEY_TIMER_AMOUNT, "").toUInt();
    _timerUnit = mySettings.value(INI_KEY_TIMER_UNIT, "").toUInt();

//    _timerInterval = _timerAmount * _timerUnit;

}



QString LuaTriggerTimerInterval::classname()
{
    return _classname;
}


