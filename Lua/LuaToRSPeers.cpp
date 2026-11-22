#include <retroshare/rspeers.h>

#include "LuaCore.h"
#include "LuaToRS.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

extern "C" {

    /* Peer Details (Net & Auth) */

    int peers_getOwnId(lua_State* L)
    {
        RsPeerId peerId = rsPeers->getOwnId();
        lua_pushstring(L, peerId.toStdString().c_str());
        return 1;
    }

    int peers_getOnlineList(lua_State* L)
    {
        std::list<RsPeerId> ids;
        if(!rsPeers->getOnlineList(ids))
            return 0;

        lua_newtable(L);
        int top = lua_gettop(L);

        std::list<RsPeerId>::iterator it = ids.begin();
        for (size_t i = 1; it != ids.end(); i++, ++it)
            pushArray(L, top, i, it->toStdString().c_str());
        return 1;
    }

    int peers_getFriendList(lua_State* L)
    {
        std::list<RsPeerId> ids;
        if(!rsPeers->getFriendList(ids))
            return 0;

        lua_newtable(L);
        int top = lua_gettop(L);

        std::list<RsPeerId>::iterator it = ids.begin();
        for (size_t i = 1; it != ids.end(); i++, ++it)
            pushArray(L, top, i, it->toStdString().c_str());

        return 1;
    }

    int peers_getPeerCount(lua_State* L)
    {
        unsigned int online, all;
        if(!rsPeers->getPeerCount(&all, &online, false))
            return 0;
        lua_pushinteger(L, all);
        lua_pushinteger(L, online);
        return 2;
    }

    int peers_isOnline(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsPeerId sslid = RsPeerId(luaL_checkstring(L, 1));
        const bool isOnline = rsPeers->isOnline(sslid);
        lua_pushboolean(L, isOnline);
        return 1;
    }

    int peers_isFriend(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsPeerId sslid = RsPeerId(luaL_checkstring(L, 1));
        const bool isFriend = rsPeers->isFriend(sslid);
        lua_pushboolean(L, isFriend);
        return 1;
    }

    int peers_isGPGAccepted(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsPgpId gpgid = RsPgpId(luaL_checkstring(L, 1));
        const bool isAccepted = rsPeers->isGPGAccepted(gpgid);
        lua_pushboolean(L, isAccepted);
        return 1;
    }

    int peers_getPeerName(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsPeerId id = RsPeerId(luaL_checkstring(L, 1));
        const std::string name = rsPeers->getPeerName(id);
        lua_pushstring(L, name.c_str());
        return 1;
    }

    int peers_getGPGName(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsPgpId pgpId = RsPgpId(luaL_checkstring(L, 1));
        const std::string name = rsPeers->getGPGName(pgpId);
        lua_pushstring(L, name.c_str());
        return 1;
    }

    int peers_getPeerDetails(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsPeerId id = RsPeerId(luaL_checkstring(L, 1));
        RsPeerDetails details;
        if(!rsPeers->getPeerDetails(id, details))
            return 0;

        lua_newtable(L);
        int t1 = lua_gettop(L);

        pushTable(L, t1, "id", details.id.toStdString().c_str());
        pushTable(L, t1, "gpg_id", details.gpg_id.toStdString().c_str());
        pushTable(L, t1, "name", details.name.c_str());
        pushTable(L, t1, "email", details.email.c_str());
        pushTable(L, t1, "location", details.location.c_str());
        pushTable(L, t1, "org", details.org.c_str());
        pushTable(L, t1, "state", details.state);
        pushTable(L, t1, "connectAddr", details.connectAddr.c_str());
        pushTable(L, t1, "connectPort", details.connectPort);
        pushTable(L, t1, "extAddr", details.extAddr.c_str());
        pushTable(L, t1, "extPort", details.extPort);
        lua_pushboolean(L, details.isHiddenNode); lua_setfield(L, t1, "hiddenNode");
        pushTable(L, t1, "hiddenNodeAddress", details.hiddenNodeAddress); // Tor/I2P address, "" otherwise

        lua_pushstring(L, "ipAddressList");
        {
            lua_newtable(L);
            int t2 = lua_gettop(L);
            int i = 1;
            for(std::list<std::string>::iterator it = details.ipAddressList.begin(); it != details.ipAddressList.end(); ++it, i++)
                pushArray(L, t2, i, it->c_str());

            /* this doesn't fit to an array
            lua_pushstring(L, "size");
            lua_pushinteger(L, details.ipAddressList.size());
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



    /* Using PGP Ids */

    int peers_getGPGOwnId(lua_State* L)
    {
        const RsPgpId pgpId = rsPeers->getGPGOwnId();
        lua_pushstring(L, pgpId.toStdString().c_str());
        return 1;
    }

    int peers_getGPGId(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsPeerId id = RsPeerId(luaL_checkstring(L, 1));
        const RsPgpId gpgId = rsPeers->getGPGId(id);
        lua_pushstring(L, gpgId.toStdString().c_str());
        return 1;
    }



    /* Group Stuff */

    int peers_addGroup(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TTABLE);
        // Read all fields before creating C++ objects (Lua errors skip destructors).
        lua_getfield(L, 1, "flag");
        const lua_Integer flag = luaL_checkinteger(L, -1);
        lua_getfield(L, 1, "id");
        const char* id = luaL_checkstring(L, -1);
        lua_getfield(L, 1, "name");
        const char* name = luaL_checkstring(L, -1);
        // peerIds are ignored for now
        {
            RsGroupInfo grpInfo;
            grpInfo.flag = uint32_t(flag);
            grpInfo.id = RsNodeGroupId(std::string(id));
            grpInfo.name = name;
            rsPeers->addGroup(grpInfo);
        }
        return 0;
    }

    int peers_editGroup(lua_State* L)
    {
        const char* groupId = luaL_checkstring(L, 1);
        luaL_checktype(L, 2, LUA_TTABLE);
        // Read all fields before creating C++ objects (Lua errors skip destructors).
        lua_getfield(L, 2, "flag");
        const lua_Integer flag = luaL_checkinteger(L, -1);
        lua_getfield(L, 2, "id");
        const char* id = luaL_checkstring(L, -1);
        lua_getfield(L, 2, "name");
        const char* name = luaL_checkstring(L, -1);
        // peerIds are ignored for now
        {
            const RsNodeGroupId grpId = RsNodeGroupId(std::string(groupId));
            RsGroupInfo grpInfo;
            grpInfo.flag = uint32_t(flag);
            grpInfo.id = RsNodeGroupId(std::string(id));
            grpInfo.name = name;
            rsPeers->editGroup(grpId, grpInfo);
        }
        return 0;
    }

    int peers_removeGroup(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsNodeGroupId grpId = RsNodeGroupId(luaL_checkstring(L, 1));
        rsPeers->removeGroup(grpId);
        return 0;
    }

    int peers_getGroupInfo(lua_State* L)
    {

        luaL_checktype(L, 1, LUA_TSTRING);

        const RsNodeGroupId grpId = RsNodeGroupId(luaL_checkstring(L, 1));
        RsGroupInfo grpInfo;
        if(!rsPeers->getGroupInfo(grpId, grpInfo))
            return 0;

        lua_newtable(L);
        int t1 = lua_gettop(L);
        pushTable(L, t1, "flag", grpInfo.flag);
        pushTable(L, t1, "id", grpInfo.id.toStdString());
        pushTable(L, t1, "name", grpInfo.name);

        lua_pushstring(L, "peerIds");
        {
            lua_newtable(L);
            int t2 = lua_gettop(L);
            int i = 1;
            foreach (RsPgpId peerId, grpInfo.peerIds) {
                pushArray(L, t2, i, peerId.toStdString());
                ++i;
            }
        }
        lua_settable(L, t1);
        return 1;
    }

    int peers_getGroupInfoList(lua_State* L)
    {
        std::list<RsGroupInfo> groupInfoList;
        if(!rsPeers->getGroupInfoList(groupInfoList))
            return 0;

        lua_newtable(L);
        int t1 = lua_gettop(L);
        uint i = 1;
        foreach (RsGroupInfo grpInfo, groupInfoList) {
            lua_pushinteger(L, i);
            {
                lua_newtable(L);
                int t2 = lua_gettop(L);
                pushTable(L, t2, "flag", grpInfo.flag);
                pushTable(L, t2, "falg", grpInfo.flag); // misspelled until 2026, kept for old scripts
                pushTable(L, t2, "id", grpInfo.id.toStdString());
                pushTable(L, t2, "name", grpInfo.name);

                lua_pushstring(L, "peerIds");
                {
                    lua_newtable(L);
                    int t3 = lua_gettop(L);
                    int j = 1;
                    foreach (RsPgpId peerId, grpInfo.peerIds) {
                        pushArray(L, t3, j, peerId.toStdString());
                        ++j;
                    }
                }
                lua_settable(L, t2);
            }
            lua_settable(L, t1);
            ++i;
        }
        return 1;
    }

    // groupId == "" && assign == false -> remove from all groups
    int peers_assignPeerToGroup(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);
        luaL_checktype(L, 2, LUA_TSTRING);
        luaL_checktype(L, 3, LUA_TBOOLEAN);

        const RsNodeGroupId grpId = RsNodeGroupId(luaL_checkstring(L, 1));
        const RsPgpId peerId = RsPgpId(luaL_checkstring(L, 2));
        const bool assign = lua_toboolean(L, 3);
        rsPeers->assignPeerToGroup(grpId, peerId, assign);
        return 0;
    }

}

// TODO: not bound yet, possibly useful (see retroshare/rspeers.h):
//   getGPGDetails, getGPGAllList, removeFriendLocation, connectAttempt, setLocation,
//   signGPGCertificate, trustGPGCertificate, getProxyServer/setProxyServer

// ---------------------------------------------------------------------------
// Friends (added for SyncFriends-style scripts)

#include "LuaToRSIds.h"
#include <util/rsurl.h>

extern "C" {

    // peers.getGPGAcceptedList() -> array of PGP ids of all friends
    int peers_getGPGAcceptedList(lua_State* L)
    {
        std::list<RsPgpId> ids;
        if(!rsPeers->getGPGAcceptedList(ids)) return 0;
        luaPushIdList(L, ids);
        return 1;
    }

    // peers.getAssociatedSSLIds("PGP_ID") -> array of SSL ids (locations) of that friend
    int peers_getAssociatedSSLIds(lua_State* L)
    {
        RsPgpId pgp;
        if(!luaIdArg(L, 1, pgp)) return luaL_argerror(L, 1, "invalid PGP id");
        std::list<RsPeerId> ids;
        if(!rsPeers->getAssociatedSSLIds(pgp, ids)) return 0;
        luaPushIdList(L, ids);
        return 1;
    }

    // peers.getRetroshareInvite(["SSL_ID"]) -> certificate (own node without argument)
    int peers_getRetroshareInvite(lua_State* L)
    {
        RsPeerId ssl;
        if(!lua_isnoneornil(L, 1) && !luaIdArg(L, 1, ssl)) return luaL_argerror(L, 1, "invalid SSL id");
        {
            const std::string invite = rsPeers->GetRetroshareInvite(ssl);
            if(invite.empty()) lua_pushnil(L);
            else lua_pushlstring(L, invite.data(), invite.size());
        }
        return 1;
    }

    // peers.getShortInvite(["SSL_ID"]) -> short invite (radix) of a node, own node without argument.
    // A few hundred bytes instead of ~2 KB, but without the PGP key: a node added from it
    // is an SSL-only friend until the key arrives on the first connection.
    int peers_getShortInvite(lua_State* L)
    {
        RsPeerId ssl;
        if(!lua_isnoneornil(L, 1) && !luaIdArg(L, 1, ssl)) return luaL_argerror(L, 1, "invalid SSL id");
        {
            std::string invite;
            const RetroshareInviteFlags flags = RetroshareInviteFlags::RADIX_FORMAT | RetroshareInviteFlags::DNS |
                RetroshareInviteFlags::CURRENT_LOCAL_IP | RetroshareInviteFlags::CURRENT_EXTERNAL_IP;
            if(!rsPeers->getShortInvite(invite, ssl, flags) || invite.empty()) lua_pushnil(L);
            else lua_pushlstring(L, invite.data(), invite.size());
        }
        return 1;
    }

    // peers.acceptInvite("INVITE") -> true if the node was added as friend.
    // Takes full certificates (getRetroshareInvite) and short invites (getShortInvite),
    // also as https://...?radix=... link.
    int peers_acceptInvite(lua_State* L)
    {
        size_t length = 0;
        const char* text = luaL_checklstring(L, 1, &length);
        bool ok;
        {
            std::string invite(text, length);
            ok = rsPeers->acceptInvite(invite);
            if(!ok)
            {
                RsUrl url(invite);
                const auto query = url.query();
                const auto radix = query.find("radix");
                if(radix != query.end()) invite = radix->second;
                RsPeerDetails details;
                uint32_t error = 0;
                if(rsPeers->parseShortInvite(invite, details, error) && !details.id.isNull() && !details.gpg_id.isNull())
                    ok = rsPeers->addSslOnlyFriend(details.id, details.gpg_id, details);
            }
        }
        lua_pushboolean(L, ok);
        return 1;
    }

    // peers.addFriend("SSL_ID", "PGP_ID") -> true on success
    int peers_addFriend(lua_State* L)
    {
        RsPeerId ssl;
        RsPgpId pgp;
        if(!luaIdArg(L, 1, ssl)) return luaL_argerror(L, 1, "invalid SSL id");
        if(!luaIdArg(L, 2, pgp)) return luaL_argerror(L, 2, "invalid PGP id");
        lua_pushboolean(L, rsPeers->addFriend(ssl, pgp));
        return 1;
    }

    // peers.removeFriend("PGP_ID") -> true on success (revokes trust in all locations)
    int peers_removeFriend(lua_State* L)
    {
        RsPgpId pgp;
        if(!luaIdArg(L, 1, pgp)) return luaL_argerror(L, 1, "invalid PGP id");
        lua_pushboolean(L, rsPeers->removeFriend(pgp));
        return 1;
    }
}
