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
        std::string pgpID = LuaCore::getInstance()->peers()->getOwnId();
        lua_pushstring(L, pgpID.c_str());
        return 1;
    }

    // virtual bool	getOnlineList(std::list<std::string> &ssl_ids)
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

    // virtual bool	getFriendList(std::list<std::string> &ssl_ids)
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

    // virtual bool getPeerCount (unsigned int *pnFriendCount, unsigned int *pnnOnlineCount, bool ssl)
    int peers_getPeerCount(lua_State* L)
    {
        unsigned int online, all;
        LuaCore::getInstance()->peers()->getPeerCount(&all, &online, false);
        lua_pushinteger(L, all);
        lua_pushinteger(L, online);
        return 2;
    }

    // virtual bool isOnline(const std::string &ssl_id)
    int peers_isOnline(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

        const std::string sslid = lua_tostring(L, 1);
        const bool online = LuaCore::getInstance()->peers()->isOnline(sslid);
        lua_pushboolean(L, online);
        return 1;
    }

    // virtual bool isFriend(const std::string &ssl_id)
    int peers_isFriend(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

        const std::string sslid = lua_tostring(L, 1);
        const bool online = LuaCore::getInstance()->peers()->isFriend(sslid);
        lua_pushboolean(L, online);
        return 1;
    }
    // virtual bool isGPGAccepted(const std::string &gpg_id_is_friend)

    int peers_isGPGAccepted(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

        const std::string gpgid = lua_tostring(L, 1);
        const bool online = LuaCore::getInstance()->peers()->isGPGAccepted(gpgid);
        lua_pushboolean(L, online);
        return 1;
    }

    // virtual std::string getPeerName(const std::string &ssl_or_gpg_id)
    int peers_getPeerName(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

        const std::string id = lua_tostring(L, 1);
        const std::string name = LuaCore::getInstance()->peers()->getPeerName(id);
        lua_pushstring(L, name.c_str());
        return 1;
    }

    // virtual bool	getPeerDetails(const std::string &ssl_or_gpg_id, RsPeerDetails &d) = 0; //get Peer detail accept SSL and PGP certs
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
}

/// TODO
/*
virtual bool FriendsChanged() 					= 0;
virtual bool OthersChanged() 					= 0;

virtual bool   haveSecretKey(const std::string& gpg_id) = 0 ;

//virtual bool	getOthersList(std::list<std::string> &ssl_ids)	= 0;


virtual std::string getGPGName(const std::string &gpg_id)	= 0;

virtual std::string getGPGOwnId()				= 0;
virtual std::string getGPGId(const std::string &sslid_or_gpgid)	= 0; //return the gpg id of the given gpg or ssl id
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


virtual bool    addGroup(RsGroupInfo &groupInfo) = 0;
virtual bool    editGroup(const std::string &groupId, RsGroupInfo &groupInfo) = 0;
virtual bool    removeGroup(const std::string &groupId) = 0;
virtual bool    getGroupInfo(const std::string &groupId, RsGroupInfo &groupInfo) = 0;
virtual bool    getGroupInfoList(std::list<RsGroupInfo> &groupInfoList) = 0;
// groupId == "" && assign == false -> remove from all groups
virtual bool    assignPeerToGroup(const std::string &groupId, const std::string &peerId, bool assign) = 0;
virtual bool    assignPeersToGroup(const std::string &groupId, const std::list<std::string> &peerIds, bool assign) = 0;



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
