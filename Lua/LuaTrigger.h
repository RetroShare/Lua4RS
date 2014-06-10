#ifndef LUATRIGGER_H
#define LUATRIGGER_H

#include "LuaEvent.h"
#include "LuaTriggerBase.h"

class LuaTrigger : public LuaTriggerBase
{
public:
    LuaTrigger();
    bool isTriggered (LuaEvent luaevent);
};

#endif // LUATRIGGER_H
