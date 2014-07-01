#ifndef LUATRIGGEREVENT_H
#define LUATRIGGEREVENT_H

#include "LuaTriggerBase.h"

class LuaTriggerEvent : public LuaTriggerBase
{
public:
    LuaTriggerEvent();
    LuaTriggerEvent(uint eventId );
    ~LuaTriggerEvent();

    bool isTriggered (const LuaEvent& luaevent);

    void toSettings(QSettings& mySettings);

    void fromSettings(const QSettings &mySettings);

    QString classname();

protected:
    uint _eventId;
};

#endif // LUATRIGGEREVENT_H
