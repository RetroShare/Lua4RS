TEMPLATE = app
TARGET = lua4rs-jsonapi
QT = core network
CONFIG += console c++11
CONFIG -= app_bundle
SOURCES += main.cpp ../Lua/LuaJsonApi.cpp
HEADERS += ../Lua/LuaJsonApi.h
win32 {
    isEmpty(LUA_LIBS): LUA_LIBS = -llua
    LIBS += $$LUA_LIBS
    INCLUDEPATH += $$LUA_INCLUDEPATH
} else {
    CONFIG += link_pkgconfig
    isEmpty(LUA_PKGCONFIG): LUA_PKGCONFIG = lua
    PKGCONFIG += $$LUA_PKGCONFIG
    LIBS += -pthread
}
