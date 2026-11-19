#ifndef LUA_TO_RS_IDS_H
#define LUA_TO_RS_IDS_H

#include <string>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

// Reads a RetroShare id (RsPeerId, RsPgpId, RsGxsId, ...) from argument n.
// Returns false for anything that is not a valid id; the caller raises the
// error (luaL_argerror) with no non-trivial C++ objects alive.
template<class Id>
bool luaIdArg(lua_State* L, int n, Id& id)
{
    size_t length = 0;
    const char* text = luaL_checklstring(L, n, &length);
    id = Id(std::string(text, length));
    return !id.isNull();
}

template<class Container>
void luaPushIdList(lua_State* L, const Container& ids)
{
    lua_createtable(L, int(ids.size()), 0);
    int i = 1;
    for(const auto& id : ids)
    {
        const std::string s = id.toStdString();
        lua_pushlstring(L, s.data(), s.size());
        lua_rawseti(L, -2, i++);
    }
}

#endif // LUA_TO_RS_IDS_H
