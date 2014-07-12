#ifndef LUA4RSINTERFACE_H
#define LUA4RSINTERFACE_H

#include <stdlib.h>

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

    virtual uint getSleepPeriodInMilliseconds() const = 0;
    virtual void setSleepPeriodInMilliseconds(const uint &value) = 0;

    virtual uint getSecondsToStarUpEvent() const = 0;
    virtual void setSecondsToStarUpEvent(const uint &value) = 0;
};

#endif // LUA4RSINTERFACE_H
