!include("../Common/retroshare_plugin.pri")::error( "Could not include file ../Common/retroshare_plugin.pri" )

CONFIG += qt resources uic qrc
CONFIG += c++17
QT += network

greaterThan(QT_MAJOR_VERSION, 4) {
	# Qt 5
	QT += widgets
}

# Lua 5.4 or 5.5, see lua.pri (MSYS2: Lua comes from the toolchain dirs)
include(lua.pri)

HEADERS +=  \
    Lua/LuaExecutionGuard.h \
    Lua/LuaBindings.h \
    Lua/LuaSandbox.h \
    Lua/LuaTrust.h \
    Lua/LuaTrustStore.h \
    Lua/BundledScripts.h \
    gui/AccessText.h \
    gui/ApprovalDialog.h \
    gui/FunctionBrowser.h \
    gui/LineDiff.h \
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
    Lua/LuaSandbox.cpp \
    Lua/LuaTrustStore.cpp \
    Lua4RSPlugin.cpp \
    Lua4RSNotify.cpp \
    gui/AccessText.cpp \
    gui/ApprovalDialog.cpp \
    gui/LineDiff.cpp \
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
