TEMPLATE = app
TARGET = tst_mcp
QT = core
CONFIG += console c++17
CONFIG -= app_bundle
INCLUDEPATH += ../../mcp
SOURCES += tst_mcp.cpp ../../mcp/McpProtocol.cpp ../../mcp/McpHttp.cpp ../../mcp/McpSettings.cpp
HEADERS += ../../mcp/McpProtocol.h ../../mcp/McpHttp.h ../../mcp/McpSettings.h
