#ifndef LUA4RSINTERFACE_H
#define LUA4RSINTERFACE_H

#ifdef _WIN32
    typedef unsigned int    uint;
#else
    #include <stdlib.h>
#endif

class L4RInterface;
class LuaCore;

namespace L4R {
    extern L4RInterface* L4RConfig;
}

class L4RInterface
{
public:
    virtual ~L4RInterface() {}

    virtual LuaCore* getCore() = 0;

    // config
    virtual uint getTickIntervalInSeconds() const = 0;
    virtual void setTickIntervalInSeconds(const uint &value) = 0;

    virtual uint getSecondsToStarUpEvent() const = 0;
    virtual void setSecondsToStarUpEvent(const uint &value) = 0;
};

#endif // LUA4RSINTERFACE_H
