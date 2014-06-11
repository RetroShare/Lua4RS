#include "LuaCore.h"
#include "LuaToRS.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}


extern "C" {
    // peers

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
        for (size_t i = 0; i < ids.size(); i++, it++)
        {
            lua_pushinteger(L, i);
            lua_pushstring(L, it->c_str());
            lua_settable(L, top);
        }

        return 1;
    }

    int peers_getFriendList(lua_State* L)
    {
        std::list<std::string> ids;
        LuaCore::getInstance()->peers()->getFriendList(ids);

        lua_newtable(L);
        int top = lua_gettop(L);

        std::list<std::string>::iterator it = ids.begin();
        for (size_t i = 0; it != ids.end(); i++, it++)
        {
            lua_pushinteger(L, i);
            lua_pushstring(L, it->c_str());
            lua_settable(L, top);
        }

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

        lua_pushstring(L, "id");
        lua_pushstring(L, details.id.c_str());
        lua_settable(L, t1);

        lua_pushstring(L, "gpg_id");
        lua_pushstring(L, details.gpg_id.c_str());
        lua_settable(L, t1);

        lua_pushstring(L, "name");
        lua_pushstring(L, details.name.c_str());
        lua_settable(L, t1);

        lua_pushstring(L, "email");
        lua_pushstring(L, details.email.c_str());
        lua_settable(L, t1);

        lua_pushstring(L, "location");
        lua_pushstring(L, details.location.c_str());
        lua_settable(L, t1);

        lua_pushstring(L, "org");
        lua_pushstring(L, details.org.c_str());
        lua_settable(L, t1);

        lua_pushstring(L, "state");
        lua_pushunsigned(L, details.state);
        lua_settable(L, t1);

        lua_pushstring(L, "connectAddr");
        lua_pushstring(L, details.connectAddr.c_str());
        lua_settable(L, t1);

        lua_pushstring(L, "connectPort");
        lua_pushunsigned(L, details.connectPort);
        lua_settable(L, t1);

        lua_pushstring(L, "extAddr");
        lua_pushstring(L, details.extAddr.c_str());
        lua_settable(L, t1);

        lua_pushstring(L, "extPort");
        lua_pushunsigned(L, details.extPort);
        lua_settable(L, t1);

        lua_pushstring(L, "ipAddressList");
        {
            lua_newtable(L);
            int t2 = lua_gettop(L);
            int i = 0;
            for(std::list<std::string>::iterator it = details.ipAddressList.begin(); it != details.ipAddressList.end(); ++it, i++)
            {
                lua_pushinteger(L, i);
                lua_pushstring(L, it->c_str());
                lua_settable(L, t2);
            }
            lua_pushstring(L, "size");
            lua_pushunsigned(L, details.ipAddressList.size());
            lua_settable(L, t2);
        }
        lua_settable(L, t1);

        // todo ip list
        // std::list<std::string>  ipAddressList;

        lua_pushstring(L, "lastConnect");
        lua_pushunsigned(L, details.lastConnect);
        lua_settable(L, t1);

        lua_pushstring(L, "lastUsed");
        lua_pushunsigned(L, details.lastUsed);
        lua_settable(L, t1);

        lua_pushstring(L, "connectState");
        lua_pushunsigned(L, details.connectState);
        lua_settable(L, t1);

        lua_pushstring(L, "connectStateString");
        lua_pushstring(L, details.connectStateString.c_str());
        lua_settable(L, t1);

        return 1;
    }
}
