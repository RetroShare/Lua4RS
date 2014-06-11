#ifndef LUATRIGGEREVENT_H
#define LUATRIGGEREVENT_H

#include "LuaTriggerBase.h"

class LuaTriggerEvent : public LuaTriggerBase
{
public:
    LuaTriggerEvent();
    ~LuaTriggerEvent();

    bool isTriggered (LuaEvent luaevent);

    QString toSettings();

    void fromSettings(QString &str);

protected:
    uint _eventid;
};

#endif // LUATRIGGEREVENT_H
