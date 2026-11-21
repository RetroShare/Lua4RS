#include <retroshare/rsservicecontrol.h>

#include "LuaCore.h"
#include "LuaToRS.h"
#include "LuaToRSIds.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

// Service permissions decide which friends may use a RetroShare service
// (chat, discovery, file transfer, ...). Each service has a default and one
// exception list: with defaultAllowed only "denied" counts, otherwise only
// "allowed".

namespace {

// Service argument: its name as in services.list() (e.g. "chat") or its id.
// Returns false if there is no such service; the caller raises the error with
// no C++ objects alive.
bool luaServiceArg(lua_State* L, int n, uint32_t& serviceId)
{
    RsPeerServiceInfo own;
    if(!rsServiceControl->getOwnServices(own)) return false;
    if(lua_type(L, n) == LUA_TNUMBER)
    {
        serviceId = uint32_t(lua_tointeger(L, n));
        return own.mServiceList.count(serviceId) > 0;
    }
    size_t length = 0;
    const char* text = lua_tolstring(L, n, &length);
    if(!text) return false;
    const std::string name(text, length);
    for(const auto& entry : own.mServiceList)
        if(entry.second.mServiceName == name) { serviceId = entry.first; return true; }
    return false;
}

void pushPermissions(lua_State* L, uint32_t serviceId, const RsServicePermissions& p)
{
    lua_newtable(L);
    const int top = lua_gettop(L);
    lua_pushinteger(L, lua_Integer(serviceId)); lua_setfield(L, top, "id");
    pushTable(L, top, "name", p.mServiceName);
    lua_pushboolean(L, p.mDefaultAllowed);      lua_setfield(L, top, "defaultAllowed");
    luaPushIdList(L, p.mPeersAllowed);          lua_setfield(L, top, "allowed");
    luaPushIdList(L, p.mPeersDenied);           lua_setfield(L, top, "denied");
}

} // namespace

extern "C" {

    // services.list() -> array of { id = number, name = "chat" }, sorted by name
    int services_list(lua_State* L)
    {
        std::vector<std::pair<std::string, uint32_t>> services;
        {
            RsPeerServiceInfo own;
            if(!rsServiceControl->getOwnServices(own)) return 0;
            for(const auto& entry : own.mServiceList)
                services.emplace_back(entry.second.mServiceName, entry.first);
        }
        std::sort(services.begin(), services.end());
        lua_createtable(L, int(services.size()), 0);
        int i = 1;
        for(const auto& service : services)
        {
            lua_newtable(L);
            const int top = lua_gettop(L);
            lua_pushinteger(L, lua_Integer(service.second)); lua_setfield(L, top, "id");
            pushTable(L, top, "name", service.first);
            lua_rawseti(L, -2, i++);
        }
        return 1;
    }

    // services.getPermissions("chat") -> { id, name, defaultAllowed, allowed = {SSL ids}, denied = {SSL ids} }
    int services_getPermissions(lua_State* L)
    {
        uint32_t serviceId = 0;
        if(!luaServiceArg(L, 1, serviceId)) return luaL_argerror(L, 1, "unknown service (see services.list())");
        bool ok;
        {
            RsServicePermissions p;
            ok = rsServiceControl->getServicePermissions(serviceId, p);
            if(ok) pushPermissions(L, serviceId, p);
        }
        return ok ? 1 : 0;
    }

    // services.isAllowed("chat", "SSL_ID") -> boolean
    int services_isAllowed(lua_State* L)
    {
        uint32_t serviceId = 0;
        if(!luaServiceArg(L, 1, serviceId)) return luaL_argerror(L, 1, "unknown service (see services.list())");
        RsPeerId peer;
        if(!luaIdArg(L, 2, peer)) return luaL_argerror(L, 2, "invalid SSL id");
        bool allowed = false;
        {
            RsServicePermissions p;
            if(!rsServiceControl->getServicePermissions(serviceId, p)) return 0;
            allowed = p.peerHasPermission(peer);
        }
        lua_pushboolean(L, allowed);
        return 1;
    }

    // services.setAllowed("chat", "SSL_ID", false) -> true on success
    int services_setAllowed(lua_State* L)
    {
        uint32_t serviceId = 0;
        if(!luaServiceArg(L, 1, serviceId)) return luaL_argerror(L, 1, "unknown service (see services.list())");
        RsPeerId peer;
        if(!luaIdArg(L, 2, peer)) return luaL_argerror(L, 2, "invalid SSL id");
        luaL_checktype(L, 3, LUA_TBOOLEAN);
        const bool allowed = lua_toboolean(L, 3);
        bool ok = false;
        {
            RsServicePermissions p;
            if(rsServiceControl->getServicePermissions(serviceId, p))
            {
                if(p.peerHasPermission(peer) == allowed) ok = true; // nothing to change
                else
                {
                    // Not RsServicePermissions::setPermission/resetPermission:
                    // they erase a possibly missing element.
                    if(p.mDefaultAllowed)
                    {
                        if(allowed) p.mPeersDenied.erase(peer); else p.mPeersDenied.insert(peer);
                    }
                    else
                    {
                        if(allowed) p.mPeersAllowed.insert(peer); else p.mPeersAllowed.erase(peer);
                    }
                    ok = rsServiceControl->updateServicePermissions(serviceId, p);
                }
            }
        }
        lua_pushboolean(L, ok);
        return 1;
    }

} // extern "C"
