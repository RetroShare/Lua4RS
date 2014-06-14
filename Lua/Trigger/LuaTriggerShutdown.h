#ifndef LUATRIGGERSHUTDOWN_H
#define LUATRIGGERSHUTDOWN_H

#include "LuaTriggerBase.h"

class LuaTriggerShutdown : public LuaTriggerBase
{
public:
    LuaTriggerShutdown();
    ~LuaTriggerShutdown();

    bool isTriggered (LuaEvent luaevent);

    void toSettings(QSettings& mySettings);

    void fromSettings(const QSettings& mySettings);

    const QString classname() { return "LuaTriggerShutdown";}

};

#endif // LUATRIGGERSHUTDOWN_H
