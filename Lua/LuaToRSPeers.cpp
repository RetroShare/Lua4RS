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

    // virtual const RsPeerId& getOwnId()
    int peers_getOwnId(lua_State* L)
    {
        RsPeerId peerId = rsPeers->getOwnId();
        lua_pushstring(L, peerId.toStdString().c_str());
        return 1;
    }

    // virtual bool	getOnlineList(std::list<RsPeerId> &ssl_ids)
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

    // virtual bool	getFriendList(std::list<RsPeerId> &ssl_ids)
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

    // virtual bool    getPeerCount (unsigned int *pnFriendCount, unsigned int *pnnOnlineCount, bool ssl)
    int peers_getPeerCount(lua_State* L)
    {
        unsigned int online, all;
        if(!rsPeers->getPeerCount(&all, &online, false))
            return 0;
        lua_pushinteger(L, all);
        lua_pushinteger(L, online);
        return 2;
    }

    // virtual bool    isOnline(const RsPeerId &ssl_id)
    int peers_isOnline(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsPeerId sslid = RsPeerId(luaL_checkstring(L, 1));
        const bool isOnline = rsPeers->isOnline(sslid);
        lua_pushboolean(L, isOnline);
        return 1;
    }

    // virtual bool    isFriend(const RsPeerId &ssl_id)
    int peers_isFriend(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsPeerId sslid = RsPeerId(luaL_checkstring(L, 1));
        const bool isFriend = rsPeers->isFriend(sslid);
        lua_pushboolean(L, isFriend);
        return 1;
    }

    // virtual bool    isGPGAccepted(const RsPgpId &gpg_id_is_friend)
    int peers_isGPGAccepted(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsPgpId gpgid = RsPgpId(luaL_checkstring(L, 1));
        const bool isAccepted = rsPeers->isGPGAccepted(gpgid);
        lua_pushboolean(L, isAccepted);
        return 1;
    }

    // virtual std::string getPeerName(const RsPeerId &ssl_id)
    int peers_getPeerName(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsPeerId id = RsPeerId(luaL_checkstring(L, 1));
        const std::string name = rsPeers->getPeerName(id);
        lua_pushstring(L, name.c_str());
        return 1;
    }

    // virtual std::string getGPGName(const RsPgpId& gpg_id)
    int peers_getGPGName(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsPgpId pgpId = RsPgpId(luaL_checkstring(L, 1));
        const std::string name = rsPeers->getGPGName(pgpId);
        lua_pushstring(L, name.c_str());
        return 1;
    }

    // virtual bool	 getPeerDetails(const RsPeerId& ssl_id, RsPeerDetails &d)
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

        lua_pushstring(L, "ipAddressList");
        {
            lua_newtable(L);
            int t2 = lua_gettop(L);
            int i = 1;
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



    /* Using PGP Ids */

    // virtual const RsPgpId& getGPGOwnId()
    int peers_getGPGOwnId(lua_State* L)
    {
        const RsPgpId pgpId = rsPeers->getGPGOwnId();
        lua_pushstring(L, pgpId.toStdString().c_str());
        return 1;
    }

    // virtual RsPgpId getGPGId(const RsPeerId& sslid)
    int peers_getGPGId(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const RsPeerId id = RsPeerId(luaL_checkstring(L, 1));
        const RsPgpId gpgId = rsPeers->getGPGId(id);
        lua_pushstring(L, gpgId.toStdString().c_str());
        return 1;
    }



    /* Group Stuff */

    // virtual bool    addGroup(RsGroupInfo &groupInfo)
    int peers_addGroup(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TTABLE);

        RsGroupInfo grpInfo;
        lua_getfield(L, 1, "flag");
        grpInfo.flag = luaL_checkinteger(L, -1);
        lua_getfield(L, 1, "id");
        grpInfo.id = luaL_checkstring(L, -1);
        lua_getfield(L, 1, "name");
        grpInfo.name = luaL_checkstring(L, -1);
        // irgnore peerIds for now

        rsPeers->addGroup(grpInfo);
        return 0;
    }

    // virtual bool    editGroup(const std::string &groupId, RsGroupInfo &groupInfo)
    int peers_editGroup(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);
        luaL_checktype(L, 2, LUA_TTABLE);

        const std::string grpId = luaL_checkstring(L, 1);

        RsGroupInfo grpInfo;
        lua_getfield(L, 2, "flag");
        grpInfo.flag = luaL_checkinteger(L, -1);
        lua_getfield(L, 2, "id");
        grpInfo.id = luaL_checkstring(L, -1);
        lua_getfield(L, 2, "name");
        grpInfo.name = luaL_checkstring(L, -1);
        // irgnore peerIds for now

        rsPeers->editGroup(grpId, grpInfo);
        return 0;
    }

    // virtual bool    removeGroup(const std::string &groupId)
    int peers_removeGroup(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const std::string grpId = luaL_checkstring(L, 1);
        rsPeers->removeGroup(grpId);
        return 0;
    }

    // virtual bool    getGroupInfo(const std::string &groupId, RsGroupInfo &groupInfo)
    int peers_getGroupInfo(lua_State* L)
    {

        luaL_checktype(L, 1, LUA_TSTRING);

        const std::string grpId = luaL_checkstring(L, 1);
        RsGroupInfo grpInfo;
        if(!rsPeers->getGroupInfo(grpId, grpInfo))
            return 0;

        lua_newtable(L);
        int t1 = lua_gettop(L);
        pushTable(L, t1, "flag", grpInfo.flag);
        pushTable(L, t1, "id", grpInfo.id);
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

    // virtual bool    getGroupInfoList(std::list<RsGroupInfo> &groupInfoList)
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
                pushTable(L, t2, "falg", grpInfo.flag);
                pushTable(L, t2, "id", grpInfo.id);
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
    // virtual bool    assignPeerToGroup(const std::string &groupId, const RsPgpId& peerId, bool assign)
    int peers_assignPeerToGroup(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);
        luaL_checktype(L, 2, LUA_TSTRING);
        luaL_checktype(L, 3, LUA_TBOOLEAN);

        const std::string grpId = luaL_checkstring(L, 1);
        const RsPgpId peerId = RsPgpId(luaL_checkstring(L, 2));
        const bool assign = lua_toboolean(L, 3);
        rsPeers->assignPeerToGroup(grpId, peerId, assign);
        return 0;
    }

}

/// TODO
///* Peer Details (Net & Auth) */
//virtual bool   haveSecretKey(const RsPgpId& gpg_id) = 0 ;
//virtual bool	 getGPGDetails(const RsPgpId& gpg_id, RsPeerDetails &d) = 0;

///* Using PGP Ids */
//virtual bool    isKeySupported(const RsPgpId& gpg_ids)   = 0;
//virtual bool    getGPGAcceptedList(std::list<RsPgpId> &gpg_ids)   = 0;
//virtual bool    getGPGSignedList(std::list<RsPgpId> &gpg_ids)   = 0;//friends that we accpet to connect with but we don't want to sign their gpg key
//virtual bool    getGPGValidList(std::list<RsPgpId> &gpg_ids)   = 0;
//virtual bool    getGPGAllList(std::list<RsPgpId> &gpg_ids) 	= 0;
//virtual bool    getAssociatedSSLIds(const RsPgpId& gpg_id, std::list<RsPeerId>& ids) = 0;
//virtual bool    gpgSignData(const void *data, const uint32_t len, unsigned char *sign, unsigned int *signlen) = 0;

///* Add/Remove Friends */
//virtual	bool addFriend(const RsPeerId &ssl_id, const RsPgpId &gpg_id,ServicePermissionFlags flags = RS_SERVICE_PERM_ALL)    = 0;
//virtual	bool removeFriend(const RsPgpId& pgp_id)  			= 0;
//virtual bool removeFriendLocation(const RsPeerId& sslId) 			= 0;

///* keyring management */
//virtual bool removeKeysFromPGPKeyring(const std::list<RsPgpId>& pgp_ids,std::string& backup_file,uint32_t& error_code)=0 ;

///* Network Stuff */
//virtual	bool connectAttempt(const RsPeerId& ssl_id)			= 0;
//virtual bool setLocation(const RsPeerId &ssl_id, const std::string &location) = 0;//location is shown in the gui to differentiate ssl certs

//virtual bool setHiddenNode(const RsPeerId &id, const std::string &hidden_node_address) = 0;
//virtual bool setHiddenNode(const RsPeerId &id, const std::string &address, uint16_t port) = 0;

//virtual	bool setLocalAddress(const RsPeerId &ssl_id, const std::string &addr, uint16_t port) = 0;
//virtual	bool setExtAddress(  const RsPeerId &ssl_id, const std::string &addr, uint16_t port) = 0;
//virtual	bool setDynDNS(const RsPeerId &id, const std::string &addr) = 0;
//virtual	bool setNetworkMode(const RsPeerId &ssl_id, uint32_t netMode) 	= 0;
//virtual bool setVisState(const RsPeerId &ssl_id, uint16_t vs_disc, uint16_t vs_dht)	= 0;

//virtual bool getProxyServer(std::string &addr, uint16_t &port,uint32_t& status_flags) = 0;
//virtual bool setProxyServer(const std::string &addr, const uint16_t port) = 0;

//virtual void getIPServersList(std::list<std::string>& ip_servers) = 0;
//virtual void allowServerIPDetermination(bool) = 0;
//virtual bool getAllowServerIPDetermination() = 0 ;

///* Auth Stuff */
//virtual	std::string GetRetroshareInvite(const RsPeerId& ssl_id,bool include_signatures) 			= 0;
//virtual	std::string getPGPKey(const RsPgpId& pgp_id,bool include_signatures) 			= 0;
//virtual bool GetPGPBase64StringAndCheckSum(const RsPgpId& gpg_id,std::string& gpg_base64_string,std::string& gpg_base64_checksum) = 0 ;
//virtual	std::string GetRetroshareInvite(bool include_signatures) 			= 0;
//virtual  bool hasExportMinimal() = 0 ;

//// Add keys to the keyring
//virtual	bool loadCertificateFromString(const std::string& cert, RsPeerId& ssl_id,RsPgpId& pgp_id, std::string& error_string)  = 0;

//// Gets the GPG details, but does not add the key to the keyring.
//virtual	bool loadDetailsFromStringCert(const std::string& certGPG, RsPeerDetails &pd,uint32_t& error_code) = 0;

//// Certificate utils
//virtual	bool cleanCertificate(const std::string &certstr, std::string &cleanCert,int& error_code) = 0;
//virtual	bool saveCertificateToFile(const RsPeerId& id, const std::string &fname)  = 0;
//virtual	std::string saveCertificateToString(const RsPeerId &id)  	= 0;

//virtual	bool signGPGCertificate(const RsPgpId &gpg_id)                   	= 0;
//virtual	bool trustGPGCertificate(const RsPgpId &gpg_id, uint32_t trustlvl) 	= 0;

///* Group Stuff */
//virtual bool    assignPeersToGroup(const std::string &groupId, const std::list<RsPgpId> &peerIds, bool assign) = 0;

///* Group sharing permission */

//// Given
//// 	- the peer id
//// 	- the permission flags of a given hash, e.g. a combination of
//// 			RS_DIR_FLAGS_NETWORK_WIDE_OTHERS, RS_DIR_FLAGS_NETWORK_WIDE_GROUPS, RS_DIR_FLAGS_BROWSABLE_OTHERS and RS_DIR_FLAGS_BROWSABLE_GROUPS
//// 	- the parent groups of the file
////
//// ... computes the sharing file permission hint flags set for this peer, that is a combination of
//// 		RS_FILE_HINTS_NETWORK_WIDE and RS_FILE_HINTS_BROWSABLE.
////
//virtual FileSearchFlags computePeerPermissionFlags(const RsPeerId& peer_id,FileStorageFlags file_sharing_flags,const std::list<std::string>& file_parent_groups) = 0;

///* Service permission flags */

//virtual ServicePermissionFlags servicePermissionFlags(const RsPgpId& gpg_id) = 0;
//virtual ServicePermissionFlags servicePermissionFlags(const RsPeerId& ssl_id) = 0;
//virtual void setServicePermissionFlags(const RsPgpId& gpg_id,const ServicePermissionFlags& flags) = 0;
