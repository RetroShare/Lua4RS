#include "LuaTriggerStartup.h"

LuaTriggerStartup::LuaTriggerStartup()
{
    _classname = "LuaTriggerStartup";
}

LuaTriggerStartup::~LuaTriggerStartup() {}

bool LuaTriggerStartup::isTriggered (const LuaEvent& luaevent)
{
    if ( luaevent.eventId == L4R_STARTUP )
    {
        _lastRun = QDateTime().currentDateTime();
        return true;
    }
    return false;
}


void LuaTriggerStartup::toSettings(QSettings &mySettings)
{
    LuaTriggerBase::toSettings(mySettings);
}

void LuaTriggerStartup::fromSettings (const QSettings& mySettings)
{
    LuaTriggerBase::fromSettings(mySettings);
}

QString LuaTriggerStartup::classname()
{
    return _classname;
}
