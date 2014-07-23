#include <retroshare/rspeers.h>

#include "LuaCore.h"
#include "LuaToRS.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

extern "C" {

    // virtual std::string getOwnId()
    int peers_getOwnId(lua_State* L)
    {
        std::string pgpID = rsPeers->getOwnId();
        lua_pushstring(L, pgpID.c_str());
        return 1;
    }

    // virtual bool	getOnlineList(std::list<std::string> &ssl_ids)
    int peers_getOnlineList(lua_State* L)
    {
        std::list<std::string> ids;
        if(!rsPeers->getOnlineList(ids))
            return 0;

        lua_newtable(L);
        int top = lua_gettop(L);

        std::list<std::string>::iterator it = ids.begin();
        for (size_t i = 1; it != ids.end(); i++, ++it)
            pushArray(L, top, i, it->c_str());
        return 1;
    }

    // virtual bool	getFriendList(std::list<std::string> &ssl_ids)
    int peers_getFriendList(lua_State* L)
    {
        std::list<std::string> ids;
        if(!rsPeers->getFriendList(ids))
            return 0;

        lua_newtable(L);
        int top = lua_gettop(L);

        std::list<std::string>::iterator it = ids.begin();
        for (size_t i = 1; it != ids.end(); i++, ++it)
            pushArray(L, top, i, it->c_str());

        return 1;
    }

    // virtual bool getPeerCount (unsigned int *pnFriendCount, unsigned int *pnnOnlineCount, bool ssl)
    int peers_getPeerCount(lua_State* L)
    {
        unsigned int online, all;
        if(!rsPeers->getPeerCount(&all, &online, false))
            return 0;
        lua_pushinteger(L, all);
        lua_pushinteger(L, online);
        return 2;
    }

    // virtual bool isOnline(const std::string &ssl_id)
    int peers_isOnline(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

        luaL_checktype(L, 1, LUA_TSTRING);

        const std::string sslid = luaL_checkstring(L, 1);
        const bool online = rsPeers->isOnline(sslid);
        lua_pushboolean(L, online);
        return 1;
    }

    // virtual bool isFriend(const std::string &ssl_id)
    int peers_isFriend(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

        luaL_checktype(L, 1, LUA_TSTRING);

        const std::string sslid = luaL_checkstring(L, 1);
        const bool online = rsPeers->isFriend(sslid);
        lua_pushboolean(L, online);
        return 1;
    }

    // virtual bool isGPGAccepted(const std::string &gpg_id_is_friend)
    int peers_isGPGAccepted(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

        luaL_checktype(L, 1, LUA_TSTRING);

        const std::string gpgid = luaL_checkstring(L, 1);
        const bool online = rsPeers->isGPGAccepted(gpgid);
        lua_pushboolean(L, online);
        return 1;
    }

    // virtual std::string getGPGName(const std::string &gpg_id)
    int peers_getGPGName(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

        luaL_checktype(L, 1, LUA_TSTRING);

        const std::string gpgId = luaL_checkstring(L, 1);
        const std::string name = rsPeers->getGPGName(gpgId);
        lua_pushstring(L, name.c_str());
        return 1;
    }

    // virtual std::string getPeerName(const std::string &ssl_or_gpg_id)
    int peers_getPeerName(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

        luaL_checktype(L, 1, LUA_TSTRING);

        const std::string id = luaL_checkstring(L, 1);
        const std::string name = rsPeers->getPeerName(id);
        lua_pushstring(L, name.c_str());
        return 1;
    }

    // virtual bool	getPeerDetails(const std::string &ssl_or_gpg_id, RsPeerDetails &d) = 0; //get Peer detail accept SSL and PGP certs
    int peers_getPeerDetails(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

        luaL_checktype(L, 1, LUA_TSTRING);

        const std::string id = luaL_checkstring(L, 1);
        RsPeerDetails details;
        if(!rsPeers->getPeerDetails(id, details))
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

    // virtual std::string getGPGOwnId()
    int peers_getGPGOwnId(lua_State* L)
    {
        const std::string gpgID = rsPeers->getGPGOwnId();
        lua_pushstring(L, gpgID.c_str());
        return 1;
    }

    // virtual std::string getGPGId(const std::string &sslid_or_gpgid) //return the gpg id of the given gpg or ssl id
    int peers_getGPGId(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

        luaL_checktype(L, 1, LUA_TSTRING);

        const std::string id = luaL_checkstring(L, 1);
        const std::string gpgId = rsPeers->getGPGId(id);
        lua_pushstring(L, gpgId.c_str());
        return 1;
    }


    // groups
    // virtual bool    addGroup(RsGroupInfo &groupInfo) = 0;
    int peers_addGroup(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

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

    // virtual bool    editGroup(const std::string &groupId, RsGroupInfo &groupInfo) = 0;
    int peers_editGroup(lua_State* L)
    {
        if( getArgCount(L) != 2)
            return 0;

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

    // virtual bool    removeGroup(const std::string &groupId) = 0;
    int peers_removeGroup(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

        luaL_checktype(L, 1, LUA_TSTRING);

        const std::string grpId = luaL_checkstring(L, 1);
        rsPeers->removeGroup(grpId);
        return 0;
    }

    // virtual bool    getGroupInfo(const std::string &groupId, RsGroupInfo &groupInfo) = 0;
    int peers_getGroupInfo(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

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
            foreach (std::string peerId, grpInfo.peerIds) {
                pushArray(L, t2, i, peerId);
                ++i;
            }
        }
        lua_settable(L, t1);
        return 1;
    }

    // virtual bool    getGroupInfoList(std::list<RsGroupInfo> &groupInfoList) = 0;
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
                    foreach (std::string peerId, grpInfo.peerIds) {
                        pushArray(L, t3, j, peerId);
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
    // virtual bool    assignPeerToGroup(const std::string &groupId, const std::string &peerId, bool assign) = 0;
    int peers_assignPeerToGroup(lua_State* L)
    {
        if( getArgCount(L) != 3)
            return 0;

        luaL_checktype(L, 1, LUA_TSTRING);
        luaL_checktype(L, 2, LUA_TSTRING);
        luaL_checktype(L, 3, LUA_TBOOLEAN);

        const std::string grpId = luaL_checkstring(L, 1);
        const std::string peerId = luaL_checkstring(L, 2);
        const bool assign = lua_toboolean(L, 3);
        rsPeers->assignPeerToGroup(grpId, peerId, assign);
        return 0;
    }
    // virtual bool    assignPeersToGroup(const std::string &groupId, const std::list<std::string> &peerIds, bool assign) = 0;

}

/// TODO
/*
virtual bool FriendsChanged() 					= 0;
virtual bool OthersChanged() 					= 0;

virtual bool   haveSecretKey(const std::string& gpg_id) = 0 ;

//virtual bool	getOthersList(std::list<std::string> &ssl_ids)	= 0;

virtual bool    isKeySupported(const std::string& gpg_ids)   = 0;
virtual bool    getGPGAcceptedList(std::list<std::string> &gpg_ids)   = 0;
virtual bool    getGPGSignedList(std::list<std::string> &gpg_ids)   = 0;//friends that we accpet to connect with but we don't want to sign their gpg key
virtual bool    getGPGValidList(std::list<std::string> &gpg_ids)   = 0;
virtual bool    getGPGAllList(std::list<std::string> &gpg_ids) 	= 0;
virtual bool	getGPGDetails(const std::string &gpg_id, RsPeerDetails &d) = 0;
virtual bool    getAssociatedSSLIds(const std::string &gpg_id, std::list<std::string> &ids) = 0;
virtual bool    gpgSignData(const void *data, const uint32_t len, unsigned char *sign, unsigned int *signlen) = 0;


virtual	bool addFriend(const std::string &ssl_id, const std::string &gpg_id,ServicePermissionFlags flags = RS_SERVICE_PERM_ALL)    = 0;
virtual	bool removeFriend(const std::string &ssl_or_gpg_id)  			= 0;
virtual bool removeFriendLocation(const std::string &sslId) 			= 0;


virtual bool removeKeysFromPGPKeyring(const std::list<std::string>& pgp_ids,std::string& backup_file,uint32_t& error_code)=0 ;


virtual	bool connectAttempt(const std::string &ssl_id)			= 0;
virtual bool setLocation(const std::string &ssl_id, const std::string &location) = 0;//location is shown in the gui to differentiate ssl certs
virtual	bool setLocalAddress(const std::string &ssl_id, const std::string &addr, uint16_t port) = 0;
virtual	bool setExtAddress(  const std::string &ssl_id, const std::string &addr, uint16_t port) = 0;
virtual	bool setDynDNS(const std::string &id, const std::string &addr) = 0;
virtual	bool setNetworkMode(const std::string &ssl_id, uint32_t netMode) 	= 0;
virtual bool setVisState(const std::string &ssl_id, uint32_t vis)		= 0;

virtual void getIPServersList(std::list<std::string>& ip_servers) = 0;
virtual void allowServerIPDetermination(bool) = 0;
virtual void allowTunnelConnection(bool) = 0;
virtual bool getAllowServerIPDetermination() = 0 ;
virtual bool getAllowTunnelConnection() = 0 ;


virtual	std::string GetRetroshareInvite(const std::string& ssl_id,bool include_signatures,bool old_format = false) 			= 0;
virtual bool GetPGPBase64StringAndCheckSum(const std::string& gpg_id,std::string& gpg_base64_string,std::string& gpg_base64_checksum) = 0 ;
virtual	std::string GetRetroshareInvite(bool include_signatures,bool old_format = false) 			= 0;
virtual  bool hasExportMinimal() = 0 ;


virtual	bool loadCertificateFromString(const std::string& cert, std::string& ssl_id,std::string& pgp_id, std::string& error_string)  = 0;


virtual	bool loadDetailsFromStringCert(const std::string& certGPG, RsPeerDetails &pd,uint32_t& error_code) = 0;


virtual	bool cleanCertificate(const std::string &certstr, std::string &cleanCert,int& error_code) = 0;
virtual	bool saveCertificateToFile(const std::string& id, const std::string &fname)  = 0;
virtual	std::string saveCertificateToString(const std::string &id)  	= 0;

virtual	bool signGPGCertificate(const std::string &gpg_id)                   	= 0;
virtual	bool trustGPGCertificate(const std::string &gpg_id, uint32_t trustlvl) 	= 0;


// Given
// 	- the peer id
// 	- the permission flags of a given hash, e.g. a combination of
// 			RS_DIR_FLAGS_NETWORK_WIDE_OTHERS, RS_DIR_FLAGS_NETWORK_WIDE_GROUPS, RS_DIR_FLAGS_BROWSABLE_OTHERS and RS_DIR_FLAGS_BROWSABLE_GROUPS
// 	- the parent groups of the file
//
// ... computes the sharing file permission hint flags set for this peer, that is a combination of
// 		RS_FILE_HINTS_NETWORK_WIDE and RS_FILE_HINTS_BROWSABLE.
//
virtual FileSearchFlags computePeerPermissionFlags(const std::string& peer_id,FileStorageFlags file_sharing_flags,const std::list<std::string>& file_parent_groups) = 0;


virtual ServicePermissionFlags servicePermissionFlags(const std::string& gpg_id) = 0;
virtual ServicePermissionFlags servicePermissionFlags_sslid(const std::string& ssl_id) = 0;
virtual void setServicePermissionFlags(const std::string& gpg_id,const ServicePermissionFlags& flags) = 0;
*/
