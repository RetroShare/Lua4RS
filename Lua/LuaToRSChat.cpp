#include <retroshare/rschats.h>

#include "LuaCore.h"
#include "LuaToRS.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

extern "C" {
    int chat_sendChat(lua_State* L)
    {
        // Check all arguments before creating C++ objects (Lua errors skip destructors).
        const char* chatId = luaL_checkstring(L, 1);
        const char* text = luaL_checkstring(L, 2);
        bool ok;
        {
            const ChatId id = ChatId(std::string(chatId));
            ok = rsChats->sendChat(id, std::string(text));
        }
        lua_pushboolean(L, ok);
        return 1;
    }
}
