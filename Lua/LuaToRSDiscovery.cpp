#include <retroshare/rsconfig.h>
#include <retroshare/rsdisc.h>

#include "LuaCore.h"
#include "LuaToRS.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

extern "C" {
    //virtual bool	getDiscFriends(std::string id, std::list<std::string>& friends) = 0;
    int disc_getDiscFriends(lua_State* L) {
        if( getArgCount(L) != 1)
            return 0;

        luaL_checktype(L, 1, LUA_TSTRING);

        const std::string id = luaL_checkstring(L, 1);
        std::list<std::string> friends;
        rsDisc->getDiscFriends(id, friends);

        lua_newtable(L);
        int top = lua_gettop(L);

        std::list<std::string>::iterator it = friends.begin();
        for (size_t i = 1; it != friends.end(); i++, ++it)
            pushArray(L, top, i, it->c_str());

        return 1;
    }

    //virtual bool	getDiscGPGFriends(std::string id, std::list<std::string>& gpg_friends) = 0;
    int disc_getDiscGPGFriends(lua_State* L) {
        if( getArgCount(L) != 1)
            return 0;

        luaL_checktype(L, 1, LUA_TSTRING);

        const std::string id = luaL_checkstring(L, 1);
        std::list<std::string> gpg_friends;
        rsDisc->getDiscGPGFriends(id, gpg_friends);

        lua_newtable(L);
        int top = lua_gettop(L);

        std::list<std::string>::iterator it = gpg_friends.begin();
        for (size_t i = 1; it != gpg_friends.end(); i++, ++it)
            pushArray(L, top, i, it->c_str());

        return 1;
    }

    //virtual bool 	getDiscVersions(std::map<std::string, std::string> &versions) = 0;
    int disc_getDiscVersions(lua_State* L)
    {
        std::map<std::string, std::string> versions;
        if(!rsDisc->getDiscVersions(versions))
            return 0;

        lua_newtable(L);
        int top = lua_gettop(L);

        std::map<std::string, std::string>::iterator it = versions.begin();
        for (it = versions.begin(); it != versions.end(); ++it) {
            std::cout << "[Lua] getDiscVersions: adding " << it->first << " (" << it->second << ")" << std::endl;
            pushTable(L, top, it->first, it->second);
        }

        return 1;
    }

    //virtual bool 	getWaitingDiscCount(unsigned int *sendCount, unsigned int *recvCount) = 0;
    int disc_getWaitingDiscCount(lua_State* L)
    {
        unsigned int sendCount, recvCount;
        rsDisc->getWaitingDiscCount(&sendCount, &recvCount);
        lua_pushnumber(L, recvCount);
        lua_pushnumber(L, sendCount);
        return 2;
    }
}
