TEMPLATE = app
TARGET = tst_bundled
QT = core
CONFIG += console c++17
CONFIG -= app_bundle
INCLUDEPATH += ../../Lua
SOURCES += tst_bundled.cpp
HEADERS += ../../Lua/BundledScripts.h
RESOURCES += ../../scripts/scripts.qrc
include(../../lua.pri)
