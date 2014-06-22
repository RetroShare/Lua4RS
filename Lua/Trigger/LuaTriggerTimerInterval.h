#ifndef LUATRIGGERTIMERINTERVAL_H
#define LUATRIGGERTIMERINTERVAL_H

#include <QString>
#include "LuaTriggerBase.h"


#define TIMER_UNIT_MULT_SEC     1
#define TIMER_UNIT_MULT_MIN     60
#define TIMER_UNIT_MULT_HOUR    3600
#define TIMER_UNIT_MULT_DAY     86400
#define TIMER_UNIT_MULT_WEEK    604800

#define INI_KEY_TIMER_AMOUNT    "TimerAmount"
#define INI_KEY_TIMER_UNIT      "TimerUnit"



class LuaTriggerTimerInterval : public LuaTriggerBase
{
public:
    LuaTriggerTimerInterval();

    ~LuaTriggerTimerInterval();

    bool isTriggered (LuaEvent luaevent);

    void toSettings(QSettings& mySettings);

    void fromSettings(const QSettings &mySettings);

    QString classname();

protected:
    uint _timerAmount;
    uint _timerUnit;
    uint _timerInterval;
};

#endif // LUATRIGGERTIMERINTERVAL_H
