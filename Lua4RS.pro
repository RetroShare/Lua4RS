!include("../Common/retroshare_plugin.pri"): error("Could not include file ../Common/retroshare_plugin.pri")

CONFIG += qt uic qrc resources

LIBS += -llua

HEADERS += \
    Lua4RSPlugin.h \
    Lua4RSMainWidget.h \
    LuaCore.h \
    LuaToRS.h

SOURCES += \
    Lua4RSPlugin.cpp \
    Lua4RSMainWidget.cpp \
    LuaCore.cpp \
    LuaToRS.cpp \
    LuaToRSPeers.cpp

FORMS += \
    Lua4RSMainWidget.ui

TARGET = Lua4RS

RESOURCES += \
    Lua4RS_images.qrc
