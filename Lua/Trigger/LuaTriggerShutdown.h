#ifndef LUATRIGGERSHUTDOWN_H
#define LUATRIGGERSHUTDOWN_H

//#include "../LuaEvent.h"
#include "LuaTriggerBase.h"

class LuaTriggerShutdown : public LuaTriggerBase
{
public:
    LuaTriggerShutdown();
    ~LuaTriggerShutdown();

    bool isTriggered (LuaEvent luaevent);

    QString toSettings();

    void fromSettings(QString &str);

};

#endif // LUATRIGGERSHUTDOWN_H
