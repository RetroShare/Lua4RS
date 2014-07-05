#include "LuaTriggerOnce.h"

#define INI_KEY_TRIGGER_WHEN    "When"

LuaTriggerOnce::LuaTriggerOnce()
{
    _classname = "LuaTriggerOnce";
    _onceDateTime = QDateTime(QDate(1970, 1, 1), QTime(0, 0, 0));
}

LuaTriggerOnce::LuaTriggerOnce(const QDateTime& onceDateTime)
{
    _classname = "LuaTriggerOnce";
    _onceDateTime = onceDateTime;
}

LuaTriggerOnce::~LuaTriggerOnce() {}

bool LuaTriggerOnce::isTriggered (const LuaEvent& luaevent)
{
    if (luaevent.eventId == L4R_TIMERTICK)
    {
        // not sure if this 5 second window is needed ....
        if (_lastRun < _onceDateTime && (                   // trigger wasn't triggered yet
                luaevent.timeStamp == _onceDateTime || (    // direct hit
                    // in case we missed the exact date add 5 seconds toleranz
                    luaevent.timeStamp > _onceDateTime &&
                    luaevent.timeStamp < _onceDateTime.addSecs(5)
                )))
        {
            _lastRun = QDateTime().currentDateTime();
            return true;
        }
    }
    return false;
}

void LuaTriggerOnce::toSettings(QSettings &mySettings)
{
    LuaTriggerBase::toSettings(mySettings);
    mySettings.setValue(INI_KEY_TRIGGER_WHEN, _onceDateTime);
}

void LuaTriggerOnce::fromSettings (const QSettings& mySettings)
{
    LuaTriggerBase::fromSettings(mySettings);
    _onceDateTime = mySettings.value(INI_KEY_TRIGGER_WHEN, QDateTime(QDate(1970, 1, 1), QTime(0, 0, 0))).toDateTime();
}

QString LuaTriggerOnce::classname()
{
    return _classname;
}
