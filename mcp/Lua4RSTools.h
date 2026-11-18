#ifndef LUA4RS_TOOLS_H
#define LUA4RS_TOOLS_H

#include <memory>

class LuaCore;
class McpProtocol;

// The Lua4RS MCP server: instructions plus all tools, backed by LuaCore.
// Tools run on the JSON API thread.
std::shared_ptr<McpProtocol> createLua4RSProtocol(LuaCore* core);

#endif // LUA4RS_TOOLS_H
