#ifndef LUA4RSINTERFACE_H
#define LUA4RSINTERFACE_H

#ifdef _WIN32
    typedef unsigned int    uint;
#else
    #include <stdlib.h>
#endif

class Lua4RSInterface;

namespace L4R {
    extern Lua4RSInterface* L4RConfig;
}

class Lua4RSInterface
{
public:
    virtual ~Lua4RSInterface() {}

    // config
    virtual uint getTickIntervalInSeconds() const = 0;
    virtual void setTickIntervalInSeconds(const uint &value) = 0;

    virtual uint getSecondsToStarUpEvent() const = 0;
    virtual void setSecondsToStarUpEvent(const uint &value) = 0;
};

#endif // LUA4RSINTERFACE_H
