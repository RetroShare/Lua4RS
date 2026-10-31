!include("../Common/retroshare_plugin.pri")::error( "Could not include file ../Common/retroshare_plugin.pri" )

CONFIG += qt resources uic qrc
CONFIG += c++17
QT += network

greaterThan(QT_MAJOR_VERSION, 4) {
	# Qt 5
	QT += widgets
}

linux-* {
    CONFIG += link_pkgconfig

    # Test for Lua5.2
    lua52 = $$system(pkg-config --exists lua5.2 && echo ok)
    isEmpty(lua52) {
        # Test for Lua5.3
        lua53 = $$system(pkg-config --exists lua5.3 && echo ok)
        isEmpty(lua53) {
            PKGCONFIG *= lua
        } else {
            PKGCONFIG *= lua5.3
        }
    } else {
        PKGCONFIG *= lua5.2
    }
}

win32 {
    # MSYS2 provides Lua through the toolchain include/library directories.
    # Override these for a separately installed Lua version.
    isEmpty(LUA_LIBS): LUA_LIBS = -llua
    LIBS += $$LUA_LIBS
    INCLUDEPATH += $$LUA_INCLUDEPATH
}

HEADERS +=  \
    Lua/LuaExecutionGuard.h \
    Lua/BundledScripts.h \
    gui/FunctionBrowser.h \
    gui/JsonApiCatalog.h \
    Lua/LuaJsonApi.h \
    Lua4RSPlugin.h \
    Lua4RSNotify.h \
    gui/Lua4RSWidget.h \
    gui/Lua4RSConfig.h \
    gui/Lua4RSTextEdit.h \
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
    interface/L4RInterface.h \
    helper.h

SOURCES +=  \
    Lua/LuaJsonApi.cpp \
    Lua4RSPlugin.cpp \
    Lua4RSNotify.cpp \
    gui/Lua4RSWidget.cpp \
    gui/Lua4RSConfig.cpp \
    gui/Lua4RSTextEdit.cpp \
    Lua/LuaCore.cpp \
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
    helper.cpp

# These bindings are included by LuaCore.cpp and must not be linked twice.
OTHER_FILES += \
    Lua/LuaToRS.cpp \
    Lua/LuaToRSPeers.cpp \
    Lua/LuaToRSServerConfig.cpp \
    Lua/LuaToRSDiscovery.cpp \
    Lua/LuaToRSChat.cpp \
    Lua/LuaToRSFile.cpp

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
