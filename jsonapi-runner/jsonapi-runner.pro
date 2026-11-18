TEMPLATE = app
TARGET = lua4rs-jsonapi
QT = core network
CONFIG += console c++11
CONFIG -= app_bundle
SOURCES += main.cpp ../Lua/LuaJsonApi.cpp
HEADERS += ../Lua/LuaJsonApi.h
include(../lua.pri)
!win32: LIBS += -pthread
