#include <retroshare/rsmsgs.h>

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
        luaL_checktype(L, 1, LUA_TSTRING);

        const std::string s = luaL_checkstring(L, 1);
        const ChatId id = ChatId(s);
        const std::string msg = luaL_checkstring(L, 2);
        const bool ret = rsMsgs->sendChat(id, msg);
        lua_pushboolean(L, ret);
        return 1;
    }
}
