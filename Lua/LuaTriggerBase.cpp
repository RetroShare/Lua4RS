#include "LuaTriggerBase.h"
#include "LuaEvent.h"

LuaTriggerBase::LuaTriggerBase() {}
LuaTriggerBase::~LuaTriggerBase() {}

bool LuaTriggerBase::isTriggered (LuaEvent luaevent) {}

QString& LuaTriggerBase::toString() {}

void LuaTriggerBase::fromString(QString& str) {}
