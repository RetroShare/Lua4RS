!include("../Common/retroshare_plugin.pri")::error( "Could not include file ../Common/retroshare_plugin.pri" )

CONFIG += qt resources uic qrc

LIBS += -llua

HEADERS +=  \
    Lua4RSPlugin.h \
    Lua4RSWidget.h \
    Lua/LuaCore.h \
    Lua/LuaToRS.h \
    Lua/LuaCode.h \
    Lua4RSNotify.h \
    Lua4RSTickThread.h \
    Lua/LuaScriptContainer.h \
    Lua/LuaConfig.h

SOURCES +=  \
    Lua4RSPlugin.cpp \
    Lua4RSWidget.cpp \
    Lua/LuaCore.cpp \
    Lua/LuaToRS.cpp \
    Lua/LuaToRSPeers.cpp \
    Lua/LuaCode.cpp \
    Lua4RSNotify.cpp \
    Lua4RSTickThread.cpp \
    Lua/LuaScriptContainer.cc \
    Lua/LuaConfig.cc

FORMS += \
    Lua4RSWidget.ui

TARGET = Lua4RS

RESOURCES +=  \
    Lua4RS_images.qrc

XUP.QT_VERSION = Qt System (4.8.1)
