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
    Lua/LuaList.h \
    Lua/LuaConfig.h \
    Lua/LuaContainer.h \
    Lua/LuaEvent.h \
    Lua/Trigger/LuaTriggerBase.h \
    Lua/Trigger/LuaTriggerTimerInterval.h \
    Lua/Trigger/LuaTriggerStartup.h \
    Lua/Trigger/LuaTriggerEvent.h \
    Lua/Trigger/LuaTriggerShutdown.h

SOURCES +=  \
    Lua4RSPlugin.cpp \
    Lua4RSWidget.cpp \
    Lua/LuaCore.cpp \
    Lua/LuaToRS.cpp \
    Lua/LuaToRSPeers.cpp \
    Lua/LuaCode.cpp \
    Lua4RSNotify.cpp \
    Lua4RSTickThread.cpp \
    Lua/LuaList.cpp \
    Lua/LuaConfig.cpp \
    Lua/LuaContainer.cpp \
    Lua/Trigger/LuaTriggerBase.cpp \
    Lua/Trigger/LuaTriggerTimerInterval.cpp \
    Lua/Trigger/LuaTriggerStartup.cpp \
    Lua/Trigger/LuaTriggerEvent.cpp \
    Lua/Trigger/LuaTriggerShutdown.cpp

FORMS += \
    Lua4RSWidget.ui

TARGET = Lua4RS

RESOURCES +=  \
    Lua4RS_images.qrc

XUP.QT_VERSION = Qt System (4.8.1)
