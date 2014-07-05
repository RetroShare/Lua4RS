#include "LuaTriggerTimerInterval.h"

#define INI_KEY_TRIGGER_TIMER_AMOUNT    "TimerAmount"
#define INI_KEY_TRIGGER_TIMER_UNIT      "TimerUnit"

LuaTriggerTimerInterval::LuaTriggerTimerInterval()
{
    _classname = "LuaTriggerTimerInterval";
    _timerAmount = 0;
    _timerUnit = 0;
    _timerInterval = 0;
}

//LuaTriggerTimerInterval::LuaTriggerTimerInterval(uint timerInterval)
//{
//    _classname = "LuaTriggerTimerInterval";
//    _timerAmount = 0;
//    _timerUnit = 0;
//    _timerInterval = timerInterval;
//}

LuaTriggerTimerInterval::LuaTriggerTimerInterval(uint timerAmount, uint timerUnit)
{
    _classname = "LuaTriggerTimerInterval";
    _timerAmount = timerAmount;
    _timerUnit = timerUnit;

    calculateInterval();
}

LuaTriggerTimerInterval::~LuaTriggerTimerInterval() {}

bool LuaTriggerTimerInterval::isTriggered (const LuaEvent& luaevent)
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
            _lastRun = QDateTime::currentDateTime();
            return true;
        }
    }
    return false;
}

void LuaTriggerTimerInterval::toSettings(QSettings& mySettings)
{
    LuaTriggerBase::toSettings(mySettings);

    mySettings.setValue(INI_KEY_TRIGGER_TIMER_AMOUNT,_timerAmount);
    mySettings.setValue(INI_KEY_TRIGGER_TIMER_UNIT,_timerUnit);
}

void LuaTriggerTimerInterval::fromSettings (const QSettings& mySettings)
{
    LuaTriggerBase::fromSettings(mySettings);
    _timerAmount = mySettings.value(INI_KEY_TRIGGER_TIMER_AMOUNT, 0).toUInt();
    _timerUnit = mySettings.value(INI_KEY_TRIGGER_TIMER_UNIT, 0).toUInt();

    calculateInterval();
}

QString LuaTriggerTimerInterval::classname()
{
    return _classname;
}

void LuaTriggerTimerInterval::getValues(uint& timerAmount, uint& timerUnit)
{
    timerAmount = _timerAmount;
    timerUnit = _timerUnit;
}

void LuaTriggerTimerInterval::calculateInterval()
{
    //                  sec, min, hour, day, week
    uint timeUnits[5] = {1, 60, 3600, 86400, 604800};
    _timerInterval = _timerAmount * timeUnits[_timerUnit];;
}


