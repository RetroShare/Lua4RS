#include <iostream>
#include <sstream>

#include <rsitems/rsconfigitems.h>

#include "p3Lua4RS.h"
#include "Lua/LuaCore.h"

#define L4RCONFIG_TIMER_STARTUP "L4RCONFIG_TIMER_STARTUP"
#define L4RCONFIG_TIMER_TICK    "L4RCONFIG_TIMER_TICK"

/* DEFINE INTERFACE POINTER! */
L4RInterface* L4R::L4RConfig = NULL;

p3Lua4RS::p3Lua4RS() :
    RsTickingThread(), p3Config(),
    _lastRun( time(0) ),
    _initTime( time(0) ),
    _startUpEventTriggered( false ),
    _luaCore(new LuaCore())
{
    // setup defaults
    _secondsToStarUpEvent = 5;
    _tickIntervalInSeconds = 1;

    start("Lua4RS");
}

// helper
RsTlvKeyValue push_int_value(const std::string& key, int value)
{
    RsTlvKeyValue kv;
    kv.key = key ;
    rs_sprintf(kv.value, "%d", value);

    return kv ;
}

RsTlvKeyValue push_uint_value(const std::string& key, uint value)
{
    RsTlvKeyValue kv;
    kv.key = key ;
    rs_sprintf(kv.value, "%d", value);

    return kv ;
}

int pop_int_value(const std::string& s)
{
    std::istringstream is(s) ;

    int val ;
    is >> val ;

    return val ;
}

uint pop_uint_value(const std::string& s)
{
    std::istringstream is(s) ;

    uint val ;
    is >> val ;

    return val ;
}

bool p3Lua4RS::saveList(bool& cleanup, std::list<RsItem*>& lst)
{
    std::cout << "[Lua] p3Lua4RS::saveList" << std::endl;

    // VOIP has it set to true -> can't be that wrong ....
    cleanup = true;

    RsConfigKeyValueSet *vitem = new RsConfigKeyValueSet;

    vitem->tlvkvs.pairs.push_back(push_int_value(L4RCONFIG_TIMER_STARTUP, _secondsToStarUpEvent));
    vitem->tlvkvs.pairs.push_back(push_int_value(L4RCONFIG_TIMER_TICK,    _tickIntervalInSeconds));

    lst.push_back(vitem);

    return true;
}

bool p3Lua4RS::loadList(std::list<RsItem*>& load)
{
    std::cout << "[Lua] p3Lua4RS::loadList" << std::endl;

    for(std::list<RsItem*>::const_iterator it = load.begin(); it!=load.end(); ++it) {
        RsConfigKeyValueSet *vitem = dynamic_cast<RsConfigKeyValueSet*>(*it);
        if(vitem != NULL) {
            for(std::list<RsTlvKeyValue>::const_iterator kit = vitem->tlvkvs.pairs.begin(); kit != vitem->tlvkvs.pairs.end(); ++kit) {
                if(kit->key ==      L4RCONFIG_TIMER_STARTUP)
                    _secondsToStarUpEvent       = pop_int_value(kit->value);
                else if(kit->key == L4RCONFIG_TIMER_TICK)
                    _tickIntervalInSeconds      = pop_int_value(kit->value);
                else
                    std::cerr << "[Lua] L4RConfig::loadList : unknown key: " << kit->key << std::endl;
	    }
	}

        delete vitem;
    }

    return true;
}

RsSerialiser *p3Lua4RS::setupSerialiser()
{
    RsSerialiser* rsSerialiser = new RsSerialiser();
    rsSerialiser->addSerialType(new RsGeneralConfigSerialiser());

    return rsSerialiser ;
}

void p3Lua4RS::threadTick()
{
    bool foundWork = false;

    // start up event
    if(!_startUpEventTriggered && (_initTime + _secondsToStarUpEvent) <= (uint)time(0))
    {
        struct LuaEvent *e = new LuaEvent();
        e->eventId = L4R_STARTUP;
        e->timeStamp = QDateTime::currentDateTime();

        _luaCore->processEvent(e);

        _startUpEventTriggered = true;
        foundWork |= true;
    }

    // tick each X second
    if(_lastRun + _tickIntervalInSeconds <= (uint)time(0))
    {
        struct LuaEvent *e = new LuaEvent();
        e->eventId = L4R_TIMERTICK;
        e->timeStamp = QDateTime::currentDateTime();
        // remove ms
        e->timeStamp.setTime(QTime(e->timeStamp.time().hour(), e->timeStamp.time().minute(), e->timeStamp.time().second()));

        _luaCore->processEvent(e);

        _lastRun = time(0);

        foundWork |= true;
    }

    // event queue
    foundWork |= _luaCore->processEventQueue();

    if (!foundWork)
        usleep(250 * 1000);
}

LuaCore *p3Lua4RS::getCore()
{
    return _luaCore;
}

uint p3Lua4RS::getTickIntervalInSeconds() const
{
    return _tickIntervalInSeconds;
}

void p3Lua4RS::setTickIntervalInSeconds(const uint& value)
{
    _tickIntervalInSeconds = value;
    IndicateConfigChanged();
}

uint p3Lua4RS::getSecondsToStarUpEvent() const
{
    return _secondsToStarUpEvent;
}

void p3Lua4RS::setSecondsToStarUpEvent(const uint& value)
{
    _secondsToStarUpEvent = value;
    IndicateConfigChanged();
}
