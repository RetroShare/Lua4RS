#include "LuaTriggerBase.h"
//#include "../LuaEvent.h"

LuaTriggerBase::LuaTriggerBase() {}
LuaTriggerBase::~LuaTriggerBase() {}

bool LuaTriggerBase::isTriggered (LuaEvent luaevent) {return false;}

QString LuaTriggerBase::toSettings() {return "";}

void LuaTriggerBase::fromSettings(QString& str) {}
