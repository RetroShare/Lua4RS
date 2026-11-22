TEMPLATE = app
TARGET = tst_triggers
QT = core
CONFIG += console c++17
CONFIG -= app_bundle
INCLUDEPATH += ../../Lua
SOURCES += tst_triggers.cpp \
    ../../Lua/LuaContainer.cpp ../../Lua/LuaList.cpp ../../Lua/LuaCode.cpp ../../Lua/LuaConfig.cpp \
    ../../Lua/Trigger/LuaTriggerBase.cpp ../../Lua/Trigger/LuaTriggerTimerInterval.cpp \
    ../../Lua/Trigger/LuaTriggerStartup.cpp ../../Lua/Trigger/LuaTriggerShutdown.cpp \
    ../../Lua/Trigger/LuaTriggerOnce.cpp ../../Lua/Trigger/LuaTriggerEvent.cpp
