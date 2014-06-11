#ifndef LUATRIGGERONCE_H
#define LUATRIGGERONCE_H

#include "LuaTriggerBase.h"

class LuaTriggerOnce : public LuaTriggerBase
{
public:
    LuaTriggerOnce();
    ~LuaTriggerOnce();

    bool isTriggered (LuaEvent luaevent);

    QString toSettings();

    void fromSettings(QString &str);

protected:
    QDateTime _onceDateTime;
};

#endif // LUATRIGGERONCE_H
