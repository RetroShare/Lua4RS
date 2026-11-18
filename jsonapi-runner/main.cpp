#include <QCoreApplication>
#include <QFile>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "../Lua/LuaJsonApi.h"
#include "../Lua/LuaSandbox.h"
#include "../Lua/LuaExecutionGuard.h"
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace {
// Stand-ins for native bindings, only registered with --sandbox, so tests can
// check the read/write gate without RetroShare.
int selftestOk(lua_State* L) { lua_pushboolean(L, 1); return 1; }

void usage()
{
    std::fprintf(stderr, "Usage: lua4rs-jsonapi [--sandbox=readonly|write] script.lua [arguments...]\n"
        "Connection: RETROSHARE_API_URL, RETROSHARE_API_USER, RETROSHARE_API_PASSWORD\n"
        "--sandbox runs the script like an untrusted Lua4RS script (time limit:\n"
        "LUA4RS_TIMEOUT_MS, default 5000) and adds selftest.read()/selftest.write().\n");
}
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    int first = 1;
    bool sandbox = false;
    LuaTrust trust = LuaTrust::Trusted;
    if(argc > 1 && std::strncmp(argv[1], "--sandbox", 9) == 0)
    {
        sandbox = true;
        const char* mode = argv[1] + 9;
        if(!std::strcmp(mode, "") || !std::strcmp(mode, "=readonly")) trust = LuaTrust::SandboxReadOnly;
        else if(!std::strcmp(mode, "=write")) trust = LuaTrust::SandboxWrite;
        else { usage(); return 2; }
        first = 2;
    }
    if(argc <= first) { usage(); return 2; }

    QFile file(QString::fromLocal8Bit(argv[first]));
    if(!file.open(QIODevice::ReadOnly))
    {
        std::fprintf(stderr, "cannot open %s\n", argv[first]);
        return 1;
    }
    const QByteArray code = file.readAll();

    LuaSandbox::MemoryLimiter memory;
    lua_State* L = LuaSandbox::newState(&memory);
    if(!L) return 1;
    luaL_openlibs(L);
    registerLuaJsonApi(L, true);
    LuaSandbox::snapshot(L);

    if(sandbox)
    {
        lua_newtable(L);
        lua_pushstring(L, "read");
        LuaSandbox::pushGuardedFunction(L, selftestOk, LuaAccess::Read, "selftest.read");
        lua_settable(L, -3);
        lua_pushstring(L, "write");
        LuaSandbox::pushGuardedFunction(L, selftestOk, LuaAccess::Write, "selftest.write");
        lua_settable(L, -3);
        LuaSandbox::registerNamespace(L, "selftest");
        lua_pop(L, 1);
    }

    const std::string name = argv[first];
    int status = LuaSandbox::load(L, code.constData(), code.size(), name, trust);
    if(status == LUA_OK)
    {
        lua_newtable(L);
        for(int i = first; i < argc; ++i) { lua_pushstring(L, argv[i]); lua_rawseti(L, -2, i - first); }
        if(sandbox)
        {
            LuaSandbox::pushEnv(L, name);
            lua_insert(L, -2);
            lua_setfield(L, -2, "arg");
            lua_pop(L, 1);

            const char* timeoutEnv = std::getenv("LUA4RS_TIMEOUT_MS");
            const int timeoutMs = timeoutEnv ? std::atoi(timeoutEnv) : 5000;
            std::atomic<bool> cancel{false};
            LuaExecutionGuard guard(L, cancel, std::chrono::milliseconds(timeoutMs > 0 ? timeoutMs : 5000), trust);
            LuaSandbox::MemoryScope limit(memory, true);
            status = lua_pcall(L, 0, LUA_MULTRET, 0);
        }
        else
        {
            lua_setglobal(L, "arg");
            status = lua_pcall(L, 0, LUA_MULTRET, 0);
        }
    }
    if(status != LUA_OK) std::fprintf(stderr, "%s\n", lua_tostring(L, -1) ? lua_tostring(L, -1) : "Lua script failed");
    lua_close(L);
    return status == LUA_OK ? 0 : 1;
}
