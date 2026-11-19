TEMPLATE = app
TARGET = tst_store
QT = core network
CONFIG += console c++17
CONFIG -= app_bundle
INCLUDEPATH += ../../Lua
SOURCES += tst_store.cpp ../../Lua/LuaStore.cpp ../../Lua/LuaJsonApi.cpp
HEADERS += ../../Lua/LuaStore.h ../../Lua/LuaJsonApi.h ../../Lua/LuaExecutionGuard.h
include(../../lua.pri)
!win32: LIBS += -pthread
