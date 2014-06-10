#ifndef LUATRIGGERBASE_H
#define LUATRIGGERBASE_H

#include "LuaEvent.h"

class LuaTriggerBase
{
public:
    LuaTriggerBase();
    virtual bool isTriggered (LuaEvent luaevent) = 0;
};

#endif // LUATRIGGERBASE_H
