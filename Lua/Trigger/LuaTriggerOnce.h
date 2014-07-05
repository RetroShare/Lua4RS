#ifndef LUATRIGGERONCE_H
#define LUATRIGGERONCE_H

#include "LuaTriggerBase.h"

class LuaTriggerOnce : public LuaTriggerBase
{
public:
    LuaTriggerOnce();
    LuaTriggerOnce(const QDateTime& onceDateTime);
    ~LuaTriggerOnce();

    bool isTriggered (const LuaEvent& luaevent);

    void toSettings(QSettings& mySettings);

    void fromSettings(const QSettings &mySettings);

    QString classname();

    QDateTime getValues();

protected:
    QDateTime _onceDateTime;
};

#endif // LUATRIGGERONCE_H
