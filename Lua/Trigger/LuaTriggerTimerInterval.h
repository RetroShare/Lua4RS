#ifndef LUATRIGGERTIMERINTERVAL_H
#define LUATRIGGERTIMERINTERVAL_H

#include <QString>
#include "LuaTriggerBase.h"

class LuaTriggerTimerInterval : public LuaTriggerBase
{
public:
    LuaTriggerTimerInterval();

    ~LuaTriggerTimerInterval();

    bool isTriggered (LuaEvent luaevent);

    void toSettings(QSettings& mySettings);

    void fromSettings(const QSettings &mySettings);

    const QString classname() { return "LuaTriggerTimerInterval";}

protected:
    int _timeramount;
    int _timerunit;
    QDate _lastrun;


};

#endif // LUATRIGGERTIMERINTERVAL_H
