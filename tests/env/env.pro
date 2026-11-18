TEMPLATE = app
TARGET = tst_env
QT =
CONFIG += console c++17
CONFIG -= app_bundle qt
INCLUDEPATH += ../../Lua
SOURCES += tst_env.cpp ../../Lua/LuaSandbox.cpp
HEADERS += ../../Lua/LuaSandbox.h ../../Lua/LuaTrust.h ../../Lua/LuaExecutionGuard.h
include(../../lua.pri)
!win32: LIBS += -pthread
