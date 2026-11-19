#include <retroshare/rsidentity.h>

#include "LuaCore.h"
#include "LuaToRS.h"
#include "LuaToRSIds.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

extern "C" {

    // identity.getOwnSignedIds() -> array of own GXS ids signed with your PGP key
    int identity_getOwnSignedIds(lua_State* L)
    {
        std::vector<RsGxsId> ids;
        if(!rsIdentity->getOwnSignedIds(ids)) return 0;
        luaPushIdList(L, ids);
        return 1;
    }

    // identity.getOwnPseudonymousIds() -> array of own anonymous GXS ids
    // (RetroShare spells it getOwnPseudonimousIds)
    int identity_getOwnPseudonymousIds(lua_State* L)
    {
        std::vector<RsGxsId> ids;
        if(!rsIdentity->getOwnPseudonimousIds(ids)) return 0;
        luaPushIdList(L, ids);
        return 1;
    }

    // identity.isOwnId("GXS_ID") -> boolean
    int identity_isOwnId(lua_State* L)
    {
        RsGxsId id;
        if(!luaIdArg(L, 1, id)) return luaL_argerror(L, 1, "invalid identity id");
        lua_pushboolean(L, rsIdentity->isOwnId(id));
        return 1;
    }

    // identity.getIdDetails("GXS_ID") -> table or nil (details may not be loaded yet)
    int identity_getIdDetails(lua_State* L)
    {
        RsGxsId id;
        if(!luaIdArg(L, 1, id)) return luaL_argerror(L, 1, "invalid identity id");
        RsIdentityDetails d;
        if(!rsIdentity->getIdDetails(id, d)) return 0;
        lua_newtable(L);
        const int top = lua_gettop(L);
        pushTable(L, top, "id", d.mId.toStdString());
        pushTable(L, top, "nickname", d.mNickname);
        pushTable(L, top, "pgpId", d.mPgpId.toStdString());
        lua_pushboolean(L, d.mFlags & RS_IDENTITY_FLAGS_PGP_LINKED); lua_setfield(L, top, "pgpLinked");
        lua_pushboolean(L, d.mFlags & RS_IDENTITY_FLAGS_PGP_KNOWN);  lua_setfield(L, top, "pgpKnown");
        lua_pushboolean(L, d.mFlags & RS_IDENTITY_FLAGS_IS_OWN_ID);  lua_setfield(L, top, "own");
        lua_pushboolean(L, d.mFlags & RS_IDENTITY_FLAGS_IS_A_CONTACT); lua_setfield(L, top, "contact");
        lua_pushinteger(L, lua_Integer(d.mPublishTS));   lua_setfield(L, top, "published");
        lua_pushinteger(L, lua_Integer(d.mLastUsageTS)); lua_setfield(L, top, "lastUsed");
        return 1;
    }
}
