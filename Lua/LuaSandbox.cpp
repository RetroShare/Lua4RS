#include "LuaSandbox.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "LuaExecutionGuard.h"

namespace {

// Registry keys (addresses are unique).
char kProto;      // table: safe globals / library copies
char kNamespaces; // table: name -> pristine copy of a binding namespace
char kEnvs;       // table: script name -> env

// ---------------------------------------------------------------------------
// allocator / panic

void* limitedAlloc(void* ud, void* ptr, size_t osize, size_t nsize)
{
    auto mem = static_cast<LuaSandbox::MemoryLimiter*>(ud);
    if(!ptr) osize = 0; // osize encodes the object type when ptr is NULL
    if(nsize == 0)
    {
        std::free(ptr);
        mem->used -= std::min(mem->used, osize);
        return nullptr;
    }
    // Only growth may fail; Lua requires shrinking to succeed.
    if(mem->limit && nsize > osize && mem->used - osize + nsize > mem->limit)
        return nullptr;
    void* block = std::realloc(ptr, nsize);
    if(block) mem->used = mem->used - osize + nsize;
    return block;
}

int panic(lua_State* L)
{
    const char* msg = lua_tostring(L, -1);
    std::fprintf(stderr, "[Lua] PANIC: unprotected error: %s\n", msg ? msg : "(non-string error)");
    return 0; // Lua aborts afterwards
}

// ---------------------------------------------------------------------------
// wrappers for sandboxed scripts

// pcall/xpcall/coroutine.resume/coroutine.close catch errors, including the
// one raised by the execution guard. Re-raise it so a script cannot outlive
// its time budget by catching the abort. Yieldable via continuation.
int afterCatch(lua_State* L, int /*status*/, lua_KContext /*ctx*/)
{
    if(LuaExecutionGuard::expired())
        return luaL_error(L, "%s", LuaExecutionGuard::expiredMessage());
    return lua_gettop(L);
}

int rethrowIfExpired(lua_State* L)
{
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_insert(L, 1);
    lua_callk(L, lua_gettop(L) - 1, LUA_MULTRET, 0, afterCatch);
    return afterCatch(L, LUA_OK, 0);
}

// Finalizers run whenever the GC decides, possibly while a trusted script
// runs or outside any guard (no time limit, wrong trust level): forbid them.
int safeSetmetatable(lua_State* L)
{
    if(lua_istable(L, 2))
    {
        lua_getfield(L, 2, "__gc");
        const bool hasGc = !lua_isnil(L, -1);
        lua_pop(L, 1);
        if(hasGc) return luaL_error(L, "__gc metamethods are not allowed in the sandbox");
    }
    // Call directly (not via lua_call) so argument errors name the function,
    // e.g. "bad argument #1 to 'print'". Bindings must not use upvalues.
    const lua_CFunction f = lua_tocfunction(L, lua_upvalueindex(1));
    return f(L);
}

int guardedBinding(lua_State* L)
{
    const auto access = static_cast<LuaAccess>(lua_tointeger(L, lua_upvalueindex(2)));
    const LuaTrust trust = LuaExecutionGuard::currentTrust();
    if(!luaTrustAllows(trust, access))
        return luaL_error(L, "%s: %s access denied (script runs as %s; needs sandbox-write or trusted)",
                          lua_tostring(L, lua_upvalueindex(3)), luaAccessName(access), luaTrustName(trust));
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_insert(L, 1);
    lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
    return lua_gettop(L);
}

// ---------------------------------------------------------------------------
// helpers

// Pushes a shallow copy of the table at index idx.
void pushShallowCopy(lua_State* L, int idx)
{
    idx = lua_absindex(L, idx);
    lua_newtable(L);
    lua_pushnil(L);
    while(lua_next(L, idx))
    {
        lua_pushvalue(L, -2);  // key
        lua_insert(L, -2);     // key key value
        lua_rawset(L, -4);
    }
}

// Pushes registry[key], creating an empty table if missing.
void pushRegistryTable(lua_State* L, const void* key)
{
    if(lua_rawgetp(L, LUA_REGISTRYINDEX, key) != LUA_TTABLE)
    {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_rawsetp(L, LUA_REGISTRYINDEX, key);
    }
}

// proto[lib] = copy of global lib; names: include list (nullptr = all) / exclude list.
void copyLibrary(lua_State* L, int proto, const char* lib,
                 const char* const* include, const char* const* exclude)
{
    if(lua_getglobal(L, lib) != LUA_TTABLE) { lua_pop(L, 1); return; }
    const int src = lua_gettop(L);
    lua_newtable(L);
    const int dst = lua_gettop(L);
    if(include)
    {
        for(auto name = include; *name; ++name)
        {
            lua_getfield(L, src, *name);
            lua_setfield(L, dst, *name);
        }
    }
    else
    {
        lua_pushnil(L);
        while(lua_next(L, src))
        {
            bool skip = false;
            if(exclude && lua_type(L, -2) == LUA_TSTRING)
                for(auto name = exclude; *name; ++name)
                    if(std::string(lua_tostring(L, -2)) == *name) { skip = true; break; }
            if(skip) { lua_pop(L, 1); continue; }
            lua_pushvalue(L, -2);
            lua_insert(L, -2);
            lua_rawset(L, dst);
        }
    }
    lua_setfield(L, proto, lib);
    lua_pop(L, 1); // src
}

// proto[name] = closure(wrapper, original global)
void wrapGlobal(lua_State* L, int proto, const char* name, lua_CFunction wrapper)
{
    lua_getglobal(L, name);
    if(lua_isnil(L, -1)) { lua_pop(L, 1); return; }
    lua_pushcclosure(L, wrapper, 1);
    lua_setfield(L, proto, name);
}

// proto[lib][name] = closure(wrapper, proto[lib][name])
void wrapField(lua_State* L, int proto, const char* lib, const char* name, lua_CFunction wrapper)
{
    if(lua_getfield(L, proto, lib) != LUA_TTABLE) { lua_pop(L, 1); return; }
    lua_getfield(L, -1, name);
    if(lua_isnil(L, -1)) { lua_pop(L, 2); return; }
    lua_pushcclosure(L, wrapper, 1);
    lua_setfield(L, -2, name);
    lua_pop(L, 1);
}

} // namespace

namespace LuaSandbox {

MemoryScope::MemoryScope(MemoryLimiter& mem, bool active, size_t perRun, size_t stateCap)
    : _mem(mem), _previous(mem.limit)
{
    if(!active) return;
    // Never below current usage: then any growth fails, nothing else breaks.
    const size_t cap = std::max(stateCap, mem.used);
    _mem.limit = std::min(mem.used + perRun, cap);
}

MemoryScope::~MemoryScope() { _mem.limit = _previous; }

lua_State* newState(MemoryLimiter* mem)
{
#if LUA_VERSION_NUM >= 505
    lua_State* L = lua_newstate(limitedAlloc, mem, luaL_makeseed(nullptr));
#else
    lua_State* L = lua_newstate(limitedAlloc, mem);
#endif
    if(L) lua_atpanic(L, panic);
    return L;
}

void snapshot(lua_State* L)
{
    lua_newtable(L);
    const int proto = lua_gettop(L);

    static const char* const base[] = {
        "assert", "error", "ipairs", "next", "pairs", "select", "tonumber", "tostring",
        "type", "rawequal", "rawlen", "rawget", "rawset", "getmetatable", "print",
        "_VERSION", nullptr };
    for(auto name = base; *name; ++name)
    {
        lua_getglobal(L, *name);
        lua_setfield(L, proto, *name);
    }
    wrapGlobal(L, proto, "pcall", rethrowIfExpired);
    wrapGlobal(L, proto, "xpcall", rethrowIfExpired);
    wrapGlobal(L, proto, "setmetatable", safeSetmetatable);

    // Not included: load, loadstring, dofile, loadfile, require, package, io,
    // debug, collectgarbage, string.dump and everything in os except below.
    static const char* const stringExclude[] = { "dump", nullptr };
    copyLibrary(L, proto, "string", nullptr, stringExclude);
    copyLibrary(L, proto, "table", nullptr, nullptr);
    copyLibrary(L, proto, "math", nullptr, nullptr);
    copyLibrary(L, proto, "utf8", nullptr, nullptr);
    copyLibrary(L, proto, "coroutine", nullptr, nullptr);
    static const char* const osInclude[] = { "time", "date", "clock", "difftime", nullptr };
    copyLibrary(L, proto, "os", osInclude, nullptr);
    // JSON helpers only; call/configure/subscribe stay unavailable for now.
    static const char* const jsonInclude[] = { "encode", "decode", "array", "null", nullptr };
    copyLibrary(L, proto, "jsonapi", jsonInclude, nullptr);

    wrapField(L, proto, "coroutine", "resume", rethrowIfExpired);
    wrapField(L, proto, "coroutine", "close", rethrowIfExpired);

    lua_rawsetp(L, LUA_REGISTRYINDEX, &kProto);

    // ("x"):method() goes through the real string table via the string
    // metatable; hide that metatable so sandboxed code cannot patch it.
    lua_pushliteral(L, "");
    if(lua_getmetatable(L, -1))
    {
        lua_pushboolean(L, 0);
        lua_setfield(L, -2, "__metatable");
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    pushRegistryTable(L, &kNamespaces); lua_pop(L, 1);
    pushRegistryTable(L, &kEnvs);       lua_pop(L, 1);
}

void registerNamespace(lua_State* L, const char* name)
{
    const int table = lua_gettop(L);
    pushRegistryTable(L, &kNamespaces);
    pushShallowCopy(L, table);
    lua_setfield(L, -2, name);
    lua_pop(L, 1);
}

void pushGuardedFunction(lua_State* L, lua_CFunction f, LuaAccess access, const char* fullName)
{
    lua_pushcfunction(L, f);
    lua_pushinteger(L, static_cast<lua_Integer>(access));
    lua_pushstring(L, fullName);
    lua_pushcclosure(L, guardedBinding, 3);
}

void pushEnv(lua_State* L, const std::string& scriptName)
{
    pushRegistryTable(L, &kEnvs);
    if(lua_getfield(L, -1, scriptName.c_str()) == LUA_TTABLE)
    {
        lua_remove(L, -2);
        return;
    }
    lua_pop(L, 1);

    lua_newtable(L);
    const int env = lua_gettop(L);

    // Library tables are copied per env, so one sandboxed script cannot
    // change what another one (or the next version of itself) sees.
    for(const void* key : { static_cast<const void*>(&kProto), static_cast<const void*>(&kNamespaces) })
    {
        pushRegistryTable(L, key);
        const int src = lua_gettop(L);
        lua_pushnil(L);
        while(lua_next(L, src))
        {
            if(lua_istable(L, -1))
            {
                pushShallowCopy(L, -1);
                lua_remove(L, -2);
            }
            lua_pushvalue(L, -2);
            lua_insert(L, -2);
            lua_rawset(L, env);
        }
        lua_pop(L, 1);
    }
    lua_pushvalue(L, env);
    lua_setfield(L, env, "_G");

    lua_pushvalue(L, env);
    lua_setfield(L, env - 1, scriptName.c_str()); // envs[name] = env
    lua_remove(L, env - 1);                        // drop envs, env stays
}

void dropEnv(lua_State* L, const std::string& scriptName)
{
    pushRegistryTable(L, &kEnvs);
    lua_pushnil(L);
    lua_setfield(L, -2, scriptName.c_str());
    lua_pop(L, 1);
}

int load(lua_State* L, const char* code, size_t size, const std::string& scriptName, LuaTrust trust)
{
    const std::string chunkName = "=" + (scriptName.empty() ? std::string("script") : scriptName);
    const int result = luaL_loadbufferx(L, code, size, chunkName.c_str(), "t");
    if(result != LUA_OK || trust == LuaTrust::Trusted) return result;
    pushEnv(L, scriptName);
    if(!lua_setupvalue(L, -2, 1)) // first upvalue of a main chunk is _ENV
    {
        lua_pop(L, 2);
        lua_pushliteral(L, "internal error: chunk has no _ENV upvalue");
        return LUA_ERRRUN;
    }
    return LUA_OK;
}

} // namespace LuaSandbox
