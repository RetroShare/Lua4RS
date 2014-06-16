#ifndef LUATRIGGERBASE_H
#define LUATRIGGERBASE_H

#include "../LuaEvent.h"
#include <QString>
#include <QSettings>


class LuaTriggerBase
{
public:
    LuaTriggerBase();

    virtual ~LuaTriggerBase();

    virtual bool isTriggered (LuaEvent luaevent);

    virtual void toSettings(QSettings& mySettings);

    virtual void fromSettings(const QSettings &mySettings);

    virtual const QString classname() { return "LuaTriggerBase";}

protected:
    QDateTime _lastRun;

};

#endif // LUATRIGGERBASE_H
