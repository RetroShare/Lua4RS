!include("../Common/retroshare_plugin.pri")::error( "Could not include file ../Common/retroshare_plugin.pri" )

CONFIG += qt resources uic qrc
CONFIG += c++17
QT += network

# Qt 5.11 or later, Qt 6
QT += widgets

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
    Lua/LuaFiles.h \
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
    Lua/LuaList.h \
    Lua/LuaContainer.h \
    Lua/LuaEvent.h \
    Lua/LuaEventCatalog.h \
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
    Lua/LuaList.cpp \
    Lua/LuaContainer.cpp \
    service/p3Lua4RS.cpp \
    helper.cpp

# MCP endpoint, mounted into RetroShare's JSON API server (needs restbed headers;
# override with RESTBED_INCLUDE_PATH=...).
rs_jsonapi {
    DEFINES += LUA4RS_MCP
    isEmpty(RESTBED_INCLUDE_PATH) {
        # The RetroShare build generates <build>/supportlibs/restbed/include only
        # at make time, so a first "qmake -r" does not see it yet: then the same
        # headers from the restbed sources, else a system restbed.
        RESTBED_INCLUDE_PATH = $$clean_path($$OUT_PWD/../../supportlibs/restbed/include)
        !exists($$RESTBED_INCLUDE_PATH/restbed): RESTBED_INCLUDE_PATH = $$clean_path($$PWD/../../supportlibs/restbed/source)
        !exists($$RESTBED_INCLUDE_PATH/restbed) {
            warning("Lua4RS: restbed headers not found in the RetroShare tree, expecting a system restbed")
            RESTBED_INCLUDE_PATH =
        }
    } else {
        !exists($$RESTBED_INCLUDE_PATH/restbed): error("Lua4RS: restbed headers not found in $$RESTBED_INCLUDE_PATH")
    }
    INCLUDEPATH += $$RESTBED_INCLUDE_PATH
    # Same as libretroshare/src/use_libretroshare.pri: without this, restbed's
    # inline "= default" members get __declspec(dllimport) and MinGW rejects them.
    win32-g++|win32-clang-g++: DEFINES *= WIN_DLL_EXPORT
    # restbed pulls in asio, which calls WSAStartup/WSACleanup (Winsock).
    win32: LIBS *= -lws2_32
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
    scripts/scripts.qrc

TRANSLATIONS += \
    lang/Lua4RS_en.ts \
    lang/Lua4RS_de.ts

