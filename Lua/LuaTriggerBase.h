#ifndef LUATRIGGERBASE_H
#define LUATRIGGERBASE_H

#include "LuaEvent.h"


class LuaTriggerBase
{
public:
    LuaTriggerBase();
    virtual ~LuaTriggerBase();

    virtual bool isTriggered (LuaEvent luaevent);

};

#endif // LUATRIGGERBASE_H
