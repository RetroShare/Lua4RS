#ifndef LUATRIGGERSTARTUP_H
#define LUATRIGGERSTARTUP_H

#include "LuaTriggerBase.h"

class LuaTriggerStartup : public LuaTriggerBase
{
public:
    LuaTriggerStartup();
    ~LuaTriggerStartup();

    bool isTriggered (const LuaEvent& luaevent);

    void toSettings(QSettings& mySettings);

    void fromSettings(const QSettings& mySettings);

    QString classname();
};

#endif // LUATRIGGERSTARTUP_H
