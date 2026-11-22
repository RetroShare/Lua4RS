TEMPLATE = app
TARGET = tst_triggers
QT = core
CONFIG += console c++17
CONFIG -= app_bundle
INCLUDEPATH += ../../Lua
SOURCES += tst_triggers.cpp \
    ../../Lua/LuaContainer.cpp ../../Lua/LuaList.cpp
# RetroShare logging (util/rsdebug.h, header-only)
INCLUDEPATH += $$PWD/../../../../libretroshare/src
QMAKE_CXXFLAGS += -isystem $$PWD/../../../../supportlibs/rapidjson/include
DEFINES += SRCDIR=\\\"$$PWD\\\"
