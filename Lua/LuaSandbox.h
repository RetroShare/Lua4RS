#ifndef LUA_SANDBOX_H
#define LUA_SANDBOX_H

#include <cstddef>
#include <string>

#include "LuaTrust.h"

struct lua_State;
typedef int (*lua_CFunction)(lua_State*);

// Sandbox for untrusted (agent written) scripts.
//
// Independent of Qt and RetroShare so the standalone runner and tests can use it.
//
// Model: one lua_State shared by all scripts (as before). Trusted scripts run
// in the global env. Sandboxed scripts get their own env table per script name,
// built from a snapshot of safe functions taken at startup, so neither trusted
// nor other sandboxed scripts can tamper with what a sandboxed script sees.
namespace LuaSandbox
{
    // Memory accounting for the whole state; limit 0 = unlimited.
    struct MemoryLimiter
    {
        size_t used  = 0;
        size_t limit = 0;
    };

    // Applies a memory limit for the duration of a sandboxed run.
    class MemoryScope
    {
    public:
        // perRun: max growth during this run; stateCap: max total state size.
        MemoryScope(MemoryLimiter& mem, bool active,
                    size_t perRun = 32u << 20, size_t stateCap = 256u << 20);
        ~MemoryScope();
    private:
        MemoryLimiter& _mem;
        size_t _previous;
    };

    // Creates a state with the accounting allocator (handles 5.4/5.5 API).
    lua_State* newState(MemoryLimiter* mem);

    // Call once after the standard libraries, print and jsonapi are set up and
    // before any user code ran. Copies everything sandboxed scripts may use.
    void snapshot(lua_State* L);

    // Registers a binding namespace (table on top of stack, left there) so
    // sandboxed envs get a pristine copy of it.
    void registerNamespace(lua_State* L, const char* name);

    // Pushes a native binding wrapped in the access check.
    // fullName is used in error messages, e.g. "chat.sendChat".
    void pushGuardedFunction(lua_State* L, lua_CFunction f, LuaAccess access, const char* fullName);

    // Pushes the env of the given script (created on first use; globals of a
    // sandboxed script persist across runs, like globals of trusted scripts).
    void pushEnv(lua_State* L, const std::string& scriptName);

    // Forgets the env of a script (renamed / deleted).
    void dropEnv(lua_State* L, const std::string& scriptName);

    // Loads code as text only (never bytecode). For non-trusted levels the
    // chunk's _ENV is replaced by the script's sandbox env. On success the
    // chunk is on top of the stack; on failure the error message is.
    int load(lua_State* L, const char* code, size_t size, const std::string& scriptName,
             LuaTrust trust);
}

#endif // LUA_SANDBOX_H
