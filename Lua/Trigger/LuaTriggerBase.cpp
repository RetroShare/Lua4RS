#include "LuaTriggerBase.h"

#define INI_KEY_TRIGGER_CLASSNAME   "Classname"
#define INI_KEY_TRIGGER_LASTRUN     "LastRun"

LuaTriggerBase::LuaTriggerBase()
{
    _classname = "LuaTriggerBase";
    _lastRun = QDateTime(QDate(1970, 1, 1), QTime(0, 0, 0));
}

LuaTriggerBase::~LuaTriggerBase() {}

bool LuaTriggerBase::isTriggered (const LuaEvent& /*luaevent*/)
{
    std::cerr << "[Lua] isTriggered() called on LuaTriggerBase" << std::endl;
    return false;
}

void LuaTriggerBase::toSettings(QSettings& mySettings)
{
    mySettings.setValue(INI_KEY_TRIGGER_CLASSNAME, classname());
    mySettings.setValue(INI_KEY_TRIGGER_LASTRUN, _lastRun);
}

void LuaTriggerBase::fromSettings(const QSettings& mySettings)
{
    _classname = mySettings.value(INI_KEY_TRIGGER_CLASSNAME, "").toString();
    _lastRun = mySettings.value(INI_KEY_TRIGGER_LASTRUN, QDateTime(QDate(1970, 1, 1), QTime(0, 0, 0))).toDateTime();
}

QString LuaTriggerBase::classname()
{
    return _classname;
}
