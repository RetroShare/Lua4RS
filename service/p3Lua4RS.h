#ifndef P3LUA4RS_H
#define P3LUA4RS_H

#include <plugins/rspqiservice.h>

#include "interface/L4RInterface.h"

class p3Lua4RS : public L4RInterface, public RsTickingThread, public p3Config
{
public:
    p3Lua4RS();

    // p3Config interface
protected:
    bool saveList(bool& cleanup, std::list<RsItem*>& lst);
    bool loadList(std::list<RsItem*>& load);
    RsSerialiser *setupSerialiser();

    // RsTickingThread interface
public:
    void data_tick();

    // Lua4RSInterface interface
public:
    LuaCore *getCore();
    uint getTickIntervalInSeconds() const;
    void setTickIntervalInSeconds(const uint &value);
    uint getSecondsToStarUpEvent() const;
    void setSecondsToStarUpEvent(const uint &value);

private:
    // tick thread
    uint        _secondsToStarUpEvent;
    uint        _tickIntervalInSeconds;
    time_t      _lastRun;
    time_t      _initTime;
    bool        _startUpEventTriggered;
    LuaCore*    _luaCore;
};

#endif // P3LUA4RS_H
