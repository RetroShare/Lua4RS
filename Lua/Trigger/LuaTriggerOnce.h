#ifndef LUATRIGGERONCE_H
#define LUATRIGGERONCE_H

#include "LuaTriggerBase.h"

#define INI_KEY_WHEN    "When"


class LuaTriggerOnce : public LuaTriggerBase
{
public:
    LuaTriggerOnce();
    ~LuaTriggerOnce();

    bool isTriggered (const LuaEvent& luaevent);

    void toSettings(QSettings& mySettings);

    void fromSettings(const QSettings &mySettings);

    QString classname();

protected:
    QDateTime _onceDateTime;
};

#endif // LUATRIGGERONCE_H
