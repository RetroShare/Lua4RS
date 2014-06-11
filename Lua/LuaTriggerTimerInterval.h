#ifndef LUATRIGGERTIMERINTERVAL_H
#define LUATRIGGERTIMERINTERVAL_H

#include <QString>
#include "LuaEvent.h"
#include "LuaTriggerBase.h"

/*

    Trigger for a Timer Interval (Run Every section in GUI)

*/


class LuaTriggerTimerInterval : public LuaTriggerBase
{
public:
    LuaTriggerTimerInterval();
    ~LuaTriggerTimerInterval();

    bool isTriggered (LuaEvent luaevent);

    QString& toString();

    void fromString(QString &str);

protected:
    int _timeramount;
    int _timerunit;

};

#endif // LUATRIGGERTIMERINTERVAL_H
