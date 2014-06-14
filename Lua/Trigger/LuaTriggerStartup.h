#ifndef LUATRIGGERSTARTUP_H
#define LUATRIGGERSTARTUP_H

#include "LuaTriggerBase.h"

class LuaTriggerStartup : public LuaTriggerBase
{
public:
    LuaTriggerStartup();
    ~LuaTriggerStartup();

    bool isTriggered (LuaEvent luaevent);

    void toSettings(QSettings& mySettings);

    void fromSettings(const QSettings& mySettings);

    const QString classname() { return "LuaTriggerStartup";}


};

#endif // LUATRIGGERSTARTUP_H
