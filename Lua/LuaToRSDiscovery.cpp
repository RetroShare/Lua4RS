#include <retroshare/rsconfig.h>
#include <retroshare/rsdisc.h>

#include "LuaCore.h"
#include "LuaToRS.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

extern "C" {
    //virtual bool	getDiscFriends(const RsPeerId &id, std::list<RsPeerId>& friends) = 0;
    int disc_getDiscFriends(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsPeerId id = RsPeerId(luaL_checkstring(L, 1));
        std::list<RsPeerId> friends;
        rsDisc->getDiscFriends(id, friends);

        lua_newtable(L);
        int top = lua_gettop(L);

        std::list<RsPeerId>::iterator it = friends.begin();
        for (size_t i = 1; it != friends.end(); i++, ++it)
            pushArray(L, top, i, it->toStdString());

        return 1;
    }

    //virtual bool	getDiscPgpFriends(const RsPgpId &pgpid, std::list<RsPgpId>& gpg_friends) = 0;
    int disc_getDiscPgpFriends(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsPgpId id = RsPgpId(luaL_checkstring(L, 1));
        std::list<RsPgpId> pgp_friends;
        rsDisc->getDiscPgpFriends(id, pgp_friends);

        lua_newtable(L);
        int top = lua_gettop(L);

        std::list<RsPgpId>::iterator it = pgp_friends.begin();
        for (size_t i = 1; it != pgp_friends.end(); i++, ++it)
            pushArray(L, top, i, it->toStdString());

        return 1;
    }

    //virtual bool 	getPeerVersion(const RsPeerId &id, std::string &versions) = 0;
    int disc_getPeerVersion(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsPeerId id = RsPeerId(luaL_checkstring(L, 1));
        std::string version;
        if(!rsDisc->getPeerVersion(id, version))
            return 0;

        lua_pushstring(L, version.c_str());

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
