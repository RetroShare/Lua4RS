#include "LuaCore.h"
#include "LuaToRS.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}


extern "C" {

    int peers_getOwnId(lua_State* L)
    {
        std::string pgpID = LuaCore::getInstance()->peers()->getOwnId();
        lua_pushstring(L, pgpID.c_str());
        return 1;
    }

    int peers_getOnlineList(lua_State* L)
    {
        std::list<std::string> ids;
        LuaCore::getInstance()->peers()->getOnlineList(ids);

        lua_newtable(L);
        int top = lua_gettop(L);

        std::list<std::string>::iterator it = ids.begin();
        for (size_t i = 1; it != ids.end(); i++, ++it)
            pushArray(L, top, i, it->c_str());
        return 1;
    }

    int peers_getFriendList(lua_State* L)
    {
        std::list<std::string> ids;
        LuaCore::getInstance()->peers()->getFriendList(ids);

        lua_newtable(L);
        int top = lua_gettop(L);

        std::list<std::string>::iterator it = ids.begin();
        for (size_t i = 1; it != ids.end(); i++, ++it)
            pushArray(L, top, i, it->c_str());

        return 1;
    }

    int peers_getPeerCount(lua_State* L)
    {
        unsigned int online, all;
        LuaCore::getInstance()->peers()->getPeerCount(&all, &online, false);
        lua_pushinteger(L, all);
        lua_pushinteger(L, online);
        return 2;
    }

    int peers_getPeerName(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

        const std::string id = lua_tostring(L, 1);
        const std::string name = LuaCore::getInstance()->peers()->getPeerName(id);
        lua_pushstring(L, name.c_str());
        return 1;
    }

    int peers_getPeerDetails(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

        const std::string id = lua_tostring(L, 1);
        RsPeerDetails details;
        bool r = LuaCore::getInstance()->peers()->getPeerDetails(id, details);

        if(!r)
            return 0;

        lua_newtable(L);
        int t1 = lua_gettop(L);

        pushTable(L, t1, "id", details.id.c_str());
        pushTable(L, t1, "gpg_id", details.gpg_id.c_str());
        pushTable(L, t1, "name", details.name.c_str());
        pushTable(L, t1, "email", details.email.c_str());
        pushTable(L, t1, "location", details.location.c_str());
        pushTable(L, t1, "org", details.org.c_str());
        pushTable(L, t1, "state", details.state);
        pushTable(L, t1, "connectAddr", details.connectAddr.c_str());
        pushTable(L, t1, "connectPort", details.connectPort);
        pushTable(L, t1, "extAddr", details.extAddr.c_str());
        pushTable(L, t1, "extPort", details.extPort);

        lua_pushstring(L, "ipAddressList");
        {
            lua_newtable(L);
            int t2 = lua_gettop(L);
            int i = 0;
            for(std::list<std::string>::iterator it = details.ipAddressList.begin(); it != details.ipAddressList.end(); ++it, i++)
                pushArray(L, t2, i, it->c_str());

            /* this doesn't fit to an array
            lua_pushstring(L, "size");
            lua_pushunsigned(L, details.ipAddressList.size());
            lua_settable(L, t2);
            */
        }
        lua_settable(L, t1);

        pushTable(L, t1, "lastConnect", details.lastConnect);
        pushTable(L, t1, "lastUsed", details.lastUsed);
        pushTable(L, t1, "connectState", details.connectState);
        pushTable(L, t1, "connectStateString", details.connectStateString.c_str());

        return 1;
    }

    // last but not least - namespace
    int peers_initNamespace(lua_State* L)
    {
        lua_newtable(L);
        int top = lua_gettop(L);

        pushTable(L, top, "getOwnId", peers_getOwnId);
        pushTable(L, top, "getOnlineList", peers_getOnlineList);
        pushTable(L, top, "getFriendList", peers_getFriendList);
        pushTable(L, top, "getPeerCount", peers_getPeerCount);
        pushTable(L, top, "getPeerName", peers_getPeerName);
        pushTable(L, top, "getPeerDetails", peers_getPeerDetails);

        return 1;
    }
}
