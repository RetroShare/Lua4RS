#ifndef LUATRIGGERBASE_H
#define LUATRIGGERBASE_H

#include <iostream>

#include "../LuaEvent.h"
#include <QString>
#include <QSettings>


class LuaTriggerBase
{
public:
    LuaTriggerBase();

    virtual ~LuaTriggerBase();

    virtual bool isTriggered (const LuaEvent& luaevent);

    virtual void toSettings(QSettings& mySettings);

    virtual void fromSettings(const QSettings &mySettings);

    virtual QString classname();

protected:
    QString _classname;
    QDateTime _lastRun;

};

#endif // LUATRIGGERBASE_H
