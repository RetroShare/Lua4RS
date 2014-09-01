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
    Lua4RSNotify.h \
    gui/Lua4RSWidget.h \
    gui/Lua4RSConfig.h \
    Lua/LuaCore.h \
    Lua/LuaToRS.h \
    Lua/LuaCode.h \
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
    service/p3Lua4RS.h \
    interface/L4RInterface.h

SOURCES +=  \
    Lua4RSPlugin.cpp \
    Lua4RSNotify.cpp \
    gui/Lua4RSWidget.cpp \
    gui/Lua4RSConfig.cpp \
    Lua/LuaCore.cpp \
    Lua/LuaToRS.cpp \
    Lua/LuaToRSPeers.cpp \
    Lua/LuaCode.cpp \
    Lua/LuaList.cpp \
    Lua/LuaConfig.cpp \
    Lua/LuaContainer.cpp \
    Lua/Trigger/LuaTriggerBase.cpp \
    Lua/Trigger/LuaTriggerTimerInterval.cpp \
    Lua/Trigger/LuaTriggerStartup.cpp \
    Lua/Trigger/LuaTriggerEvent.cpp \
    Lua/Trigger/LuaTriggerShutdown.cpp \
    Lua/Trigger/LuaTriggerOnce.cpp \
    service/p3Lua4RS.cpp \
    Lua/LuaToRSServerConfig.cpp \
    Lua/LuaToRSDiscovery.cpp

FORMS += \
    gui/Lua4RSWidget.ui \
    gui/Lua4RSConfig.ui

TARGET = Lua4RS

RESOURCES +=  \
    Lua4RS_images.qrc \
    Lua4RS_lang.qrc

TRANSLATIONS += \
    lang/Lua4RS_en.ts \
    lang/Lua4RS_de.ts

XUP.QT_VERSION = Qt System (4.8.1)
