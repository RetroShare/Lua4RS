#ifndef LUATRIGGERBASE_H
#define LUATRIGGERBASE_H

#include "../LuaEvent.h"
#include <QString>


class LuaTriggerBase
{
public:
    LuaTriggerBase();
    virtual ~LuaTriggerBase();

    virtual bool isTriggered (LuaEvent luaevent);

    virtual QString& toString();

    virtual void fromString(QString& str);

};

#endif // LUATRIGGERBASE_H
