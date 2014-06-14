#ifndef LUATRIGGERONCE_H
#define LUATRIGGERONCE_H

#include "LuaTriggerBase.h"

class LuaTriggerOnce : public LuaTriggerBase
{
public:
    LuaTriggerOnce();
    ~LuaTriggerOnce();

    bool isTriggered (LuaEvent luaevent);

    void toSettings(QSettings& mySettings);

    void fromSettings(const QSettings &mySettings);

    const QString classname() { return "LuaTriggerOnce";}


protected:
    QDateTime _onceDateTime;
};

#endif // LUATRIGGERONCE_H
