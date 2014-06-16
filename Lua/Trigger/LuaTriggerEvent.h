#ifndef LUATRIGGEREVENT_H
#define LUATRIGGEREVENT_H

#include "LuaTriggerBase.h"

class LuaTriggerEvent : public LuaTriggerBase
{
public:
    LuaTriggerEvent();
    ~LuaTriggerEvent();

    bool isTriggered (LuaEvent luaevent);

    void toSettings(QSettings& mySettings);

    void fromSettings(const QSettings &mySettings);

    const QString classname() { return "LuaTriggerEvent";}

protected:
    uint _eventId;
};

#endif // LUATRIGGEREVENT_H
