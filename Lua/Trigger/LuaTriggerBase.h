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

    virtual QString toSettings();

    virtual void fromSettings(QString& str);

};

#endif // LUATRIGGERBASE_H
