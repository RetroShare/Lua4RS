TEMPLATE = app
TARGET = tst_truststore
QT = core
CONFIG += console c++17
CONFIG -= app_bundle
INCLUDEPATH += ../../Lua ../../gui
SOURCES += tst_truststore.cpp ../../Lua/LuaTrustStore.cpp ../../gui/LineDiff.cpp
HEADERS += ../../Lua/LuaTrustStore.h ../../Lua/LuaTrust.h ../../gui/LineDiff.h
