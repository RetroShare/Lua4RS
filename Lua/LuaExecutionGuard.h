#ifndef LUA_EXECUTION_GUARD_H
#define LUA_EXECUTION_GUARD_H

#include <atomic>
#include <chrono>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include "LuaTrust.h"

// Cooperative limits for Lua bytecode; native calls must enforce their own timeout.
// Also carries the trust level of the running script, so bindings can check it.
class LuaExecutionGuard
{
public:
    LuaExecutionGuard(lua_State* state, const std::atomic<bool>& cancel,
                      std::chrono::milliseconds timeout = std::chrono::seconds(5),
                      LuaTrust trust = LuaTrust::Trusted)
        : _state(state), _cancel(cancel), _deadline(std::chrono::steady_clock::now() + timeout),
          _trust(trust)
    {
        current() = this;
        lua_sethook(state, check, LUA_MASKCOUNT, 10000);
    }
    ~LuaExecutionGuard() { lua_sethook(_state, nullptr, 0, 0); current() = nullptr; }
    static const char* errorText(lua_State* state)
    {
        const char* text = lua_tostring(state, -1);
        return text ? text : "Script raised a non-string error";
    }
    static const std::atomic<bool>* cancellationFlag()
    { return current() ? &current()->_cancel : nullptr; }
    static int remainingMilliseconds(int fallback)
    {
        if(!current()) return fallback;
        const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
            current()->_deadline - std::chrono::steady_clock::now()).count();
        return remaining < 1 ? 1 : (remaining < fallback ? int(remaining) : fallback);
    }
    // True once the running script must stop (cancelled or out of time).
    static bool expired()
    {
        return current() && (current()->_cancel.load() ||
                             std::chrono::steady_clock::now() >= current()->_deadline);
    }
    // Trust of the running script. Without a guard nothing may write.
    static LuaTrust currentTrust()
    { return current() ? current()->_trust : LuaTrust::SandboxReadOnly; }
    static const char* expiredMessage()
    { return "Script cancelled or exceeded its execution time limit"; }
private:
    static void check(lua_State* state, lua_Debug*)
    {
        if(expired()) luaL_error(state, "%s", expiredMessage());
    }
    static LuaExecutionGuard*& current()
    {
        static thread_local LuaExecutionGuard* guard = nullptr;
        return guard;
    }
    lua_State* _state;
    const std::atomic<bool>& _cancel;
    std::chrono::steady_clock::time_point _deadline;
    LuaTrust _trust;
};
#endif
