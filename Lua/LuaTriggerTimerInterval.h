#ifndef LUATRIGGERTIMERINTERVAL_H
#define LUATRIGGERTIMERINTERVAL_H

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

protected:


};

#endif // LUATRIGGERTIMERINTERVAL_H
