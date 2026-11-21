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
    mcp/Lua4RSTools.h \
    mcp/McpGuiThread.h \
    mcp/McpHttp.h \
    mcp/McpProtocol.h \
    mcp/McpSettings.h \
    gui/McpPanel.h \
    Lua/LuaExecutionGuard.h \
    Lua/LuaBindings.h \
    Lua/LuaRun.h \
    Lua/LuaSandbox.h \
    Lua/LuaStore.h \
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
    Lua/LuaToRSIds.h \
    Lua/LuaCode.h \
    Lua/LuaList.h \
    Lua/LuaConfig.h \
    Lua/LuaContainer.h \
    Lua/LuaEvent.h \
    Lua/LuaEventCatalog.h \
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
    mcp/Lua4RSTools.cpp \
    mcp/McpHttp.cpp \
    mcp/McpProtocol.cpp \
    mcp/McpSettings.cpp \
    gui/McpPanel.cpp \
    Lua/LuaJsonApi.cpp \
    Lua/LuaSandbox.cpp \
    Lua/LuaStore.cpp \
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

# MCP endpoint, mounted into RetroShare's JSON API server (needs restbed headers
# from the RetroShare build; override with RESTBED_INCLUDE_PATH=...).
rs_jsonapi {
    DEFINES += LUA4RS_MCP
    isEmpty(RESTBED_INCLUDE_PATH): RESTBED_INCLUDE_PATH = $$clean_path($$OUT_PWD/../../supportlibs/restbed/include)
    !exists($$RESTBED_INCLUDE_PATH/restbed): error("Lua4RS: restbed headers not found in $$RESTBED_INCLUDE_PATH (set RESTBED_INCLUDE_PATH)")
    INCLUDEPATH += $$RESTBED_INCLUDE_PATH
    HEADERS += mcp/McpServer.h
    SOURCES += mcp/McpServer.cpp
    message("Lua4RS: MCP endpoint enabled")
} else {
    message("Lua4RS: built without rs_jsonapi, no MCP endpoint")
}

# These bindings are included by LuaCore.cpp and must not be linked twice.
OTHER_FILES += \
    Lua/LuaToRS.cpp \
    Lua/LuaToRSPeers.cpp \
    Lua/LuaToRSServerConfig.cpp \
    Lua/LuaToRSDiscovery.cpp \
    Lua/LuaToRSChat.cpp \
    Lua/LuaToRSFile.cpp \
    Lua/LuaToRSIdentity.cpp \
    Lua/LuaToRSMail.cpp \
    Lua/LuaToRSServices.cpp

FORMS += \
    gui/Lua4RSWidget.ui \
    gui/Lua4RSConfig.ui

TARGET = Lua4RS

RESOURCES +=  \
    Lua4RS_images.qrc \
    Lua4RS_lang.qrc \
    scripts/examples/examples.qrc

TRANSLATIONS += \
    lang/Lua4RS_en.ts \
    lang/Lua4RS_de.ts

XUP.QT_VERSION = Qt System (4.8.1)
