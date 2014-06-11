#ifndef LUATRIGGERSTARTUP_H
#define LUATRIGGERSTARTUP_H

//#include "../LuaEvent.h"
#include "LuaTriggerBase.h"

class LuaTriggerStartup : public LuaTriggerBase
{
public:
    LuaTriggerStartup();
    ~LuaTriggerStartup();

    bool isTriggered (LuaEvent luaevent);

    QString toSettings();

    void fromSettings(QString &str);

};

#endif // LUATRIGGERSTARTUP_H
