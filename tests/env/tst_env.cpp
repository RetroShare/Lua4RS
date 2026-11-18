// Per-script sandbox environments: globals persist between runs of the same
// script, are separate between scripts, and are gone after dropEnv.
// Build: qmake env.pro && make && ./tst_env
#include <cstdio>
#include <cstring>
#include <string>

#include <lua.hpp>

#include "LuaSandbox.h"

namespace {
int failures = 0;
int checks = 0;

void check(bool condition, const char* what, int line)
{
    ++checks;
    if(!condition) { ++failures; std::printf("FAIL  line %d: %s\n", line, what); }
}
#define CHECK(cond) check((cond), #cond, __LINE__)

// Runs `counter = (counter or 0) + 1; return counter` as `name`, returns the result.
lua_Integer bump(lua_State* L, const char* name, LuaTrust trust = LuaTrust::SandboxReadOnly)
{
    const char* code = "counter = (counter or 0) + 1 return counter";
    if(LuaSandbox::load(L, code, std::strlen(code), name, trust) != LUA_OK ||
       lua_pcall(L, 0, 1, 0) != LUA_OK)
    {
        std::printf("Lua error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        return -1;
    }
    const lua_Integer value = lua_tointeger(L, -1);
    lua_pop(L, 1);
    return value;
}
} // namespace

int main()
{
    LuaSandbox::MemoryLimiter memory;
    lua_State* L = LuaSandbox::newState(&memory);
    luaL_openlibs(L);
    LuaSandbox::snapshot(L);

    CHECK(bump(L, "a.lua") == 1);
    CHECK(bump(L, "a.lua") == 2);          // globals persist for the same script
    CHECK(bump(L, "b.lua") == 1);          // ... but not across scripts
    LuaSandbox::dropEnv(L, "a.lua");
    CHECK(bump(L, "a.lua") == 1);          // fresh after dropEnv
    CHECK(bump(L, "b.lua") == 2);          // others untouched
    LuaSandbox::dropEnv(L, "missing.lua"); // harmless

    // trusted scripts use the real globals, not an env
    CHECK(bump(L, "t.lua", LuaTrust::Trusted) == 1);
    CHECK(bump(L, "t.lua", LuaTrust::Trusted) == 2);
    CHECK(bump(L, "b.lua") == 3);
    lua_getglobal(L, "counter");
    CHECK(lua_tointeger(L, -1) == 2);
    lua_pop(L, 1);

    CHECK(lua_gettop(L) == 0);             // nothing left on the stack
    lua_close(L);
    std::printf("%d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}
