!include("../Common/retroshare_plugin.pri")::error( "Could not include file ../Common/retroshare_plugin.pri" )

CONFIG += qt resources uic qrc

HEADERS -= upnp/upnputil.h
SOURCES -= upnp/upnputil.c

linux-* {
    LIBS += -llua
}

win32 {
    LIBS += -llua52
    INCLUDEPATH += ../../../lua-5.2.3/src
}

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
    Lua/Trigger/LuaTriggerShutdown.h \
    Lua/Trigger/LuaTriggerOnce.h \
    Lua4RSConfig.h \
    service/p3Lua4RS.h \
    interface/Lua4RSInterface.h

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
    Lua/Trigger/LuaTriggerShutdown.cpp \
    Lua/Trigger/LuaTriggerOnce.cpp \
    Lua4RSConfig.cpp \
    service/p3Lua4RS.cpp

FORMS += \
    Lua4RSWidget.ui \
    Lua4RSConfig.ui

TARGET = Lua4RS

RESOURCES +=  \
    Lua4RS_images.qrc

TRANSLATIONS += \
    lang/Lua4RS_en.ts

XUP.QT_VERSION = Qt System (4.8.1)
