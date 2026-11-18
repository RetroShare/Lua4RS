#ifndef LUA_TRUST_H
#define LUA_TRUST_H

// Trust level a script runs with. Ordered: higher value = more permissions.
enum class LuaTrust : int
{
    SandboxReadOnly = 0, // sandboxed env, only bindings marked Read
    SandboxWrite    = 1, // sandboxed env, Read + Write bindings
    Trusted         = 2  // global env, all standard libraries (legacy behaviour)
};

// Classification of every native binding, checked on each call.
enum class LuaAccess : int
{
    Read  = 0, // no side effects on RetroShare or the network
    Write = 1  // changes state, sends something, or starts network activity
};

inline const char* luaTrustName(LuaTrust trust)
{
    switch(trust)
    {
    case LuaTrust::SandboxReadOnly: return "sandbox-readonly";
    case LuaTrust::SandboxWrite:    return "sandbox-write";
    case LuaTrust::Trusted:         return "trusted";
    }
    return "unknown";
}

inline bool luaTrustAllows(LuaTrust trust, LuaAccess access)
{
    return access == LuaAccess::Read || trust >= LuaTrust::SandboxWrite;
}

#endif // LUA_TRUST_H
