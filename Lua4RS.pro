!include("../Common/retroshare_plugin.pri"): error("Could not include file ../Common/retroshare_plugin.pri")

CONFIG += qt uic qrc resources

LIBS += -llua

HEADERS += \
    Lua4RSPlugin.h \
    Lua4RSMainWidget.h \
    Lua/LuaCore.h \
    Lua/LuaToRS.h \
    Lua/LuaCode.h \
    Lua4RSNotify.h \
    p3Lua4RS.h

SOURCES += \
    Lua4RSPlugin.cpp \
    Lua4RSMainWidget.cpp \
    Lua/LuaCore.cpp \
    Lua/LuaToRS.cpp \
    Lua/LuaToRSPeers.cpp \
    Lua/LuaCode.cpp \
    Lua4RSNotify.cpp \
    p3Lua4RS.cpp

FORMS += \
    Lua4RSMainWidget.ui

TARGET = Lua4RS

RESOURCES += \
    Lua4RS_images.qrc
