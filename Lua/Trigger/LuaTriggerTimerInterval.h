#ifndef LUATRIGGERTIMERINTERVAL_H
#define LUATRIGGERTIMERINTERVAL_H

#include <QString>
#include "LuaTriggerBase.h"

class LuaTriggerTimerInterval : public LuaTriggerBase
{
public:
    LuaTriggerTimerInterval();
    LuaTriggerTimerInterval(uint timerAmount, uint timerUnit);

    ~LuaTriggerTimerInterval();

    bool isTriggered (const LuaEvent& luaevent);

    void toSettings(QSettings& mySettings);

    void fromSettings(const QSettings &mySettings);

    QString classname();

    void getValues(uint& timerAmount, uint& timerUnit);

protected:
    void calculateInterval();

    uint _timerAmount;
    uint _timerUnit;
    uint _timerInterval;
};

#endif // LUATRIGGERTIMERINTERVAL_H
