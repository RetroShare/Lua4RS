#include <iostream>
#include <sstream>

#include <rsitems/rsconfigitems.h>

#include "p3Lua4RS.h"
#include "Lua/LuaCore.h"

#define L4RCONFIG_TIMER_STARTUP "L4RCONFIG_TIMER_STARTUP"
#define L4RCONFIG_TIMER_TICK    "L4RCONFIG_TIMER_TICK"

/* DEFINE INTERFACE POINTER! */
L4RInterface* L4R::L4RConfig = NULL;

p3Lua4RS::p3Lua4RS(RsPluginHandler* rph) :
    RsPQIService(RS_SERVICE_TYPE_L4R_PLUGIN, 0, rph),
    _lastRun( time(0) ),
    _initTime( time(0) ),
    _startUpEventTriggered( false ),
    _luaCore(new LuaCore())
{
    // setup defaults
    _secondsToStarUpEvent = 5;
    _tickIntervalInSeconds = 1;
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

int p3Lua4RS::tick()
{
    // start up event
    if(!_startUpEventTriggered && (_initTime + _secondsToStarUpEvent) <= (uint)time(0))
    {
        LuaEvent e;
        e.eventId = L4R_STARTUP;
        e.timeStamp = QDateTime::currentDateTime();

        if(_luaCore->processEvent(e))
            // startup event wasn't blocked by core
            _startUpEventTriggered = true;
    }

    // tick each X second
    if(_lastRun + _tickIntervalInSeconds <= (uint)time(0))
    {
        LuaEvent e;
        e.eventId = L4R_TIMERTICK;
        e.timeStamp = QDateTime::currentDateTime();
        // remove ms
        e.timeStamp.setTime(QTime(e.timeStamp.time().hour(), e.timeStamp.time().minute(), e.timeStamp.time().second()));

        _luaCore->processEvent(e);

        _lastRun = time(0);
    }

    return 0;
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

RsServiceInfo p3Lua4RS::getServiceInfo()
{
    const std::string L4R_APP_NAME = "Lua4RS";
    const uint16_t L4R_APP_MAJOR_VERSION  =       1;
    const uint16_t L4R_APP_MINOR_VERSION  =       0;
    const uint16_t L4R_MIN_MAJOR_VERSION  =       1;
    const uint16_t L4R_MIN_MINOR_VERSION  =       0;

    return RsServiceInfo(RS_SERVICE_TYPE_L4R_PLUGIN,
                         L4R_APP_NAME,
                         L4R_APP_MAJOR_VERSION,
                         L4R_APP_MINOR_VERSION,
                         L4R_MIN_MAJOR_VERSION,
                         L4R_MIN_MINOR_VERSION);
}
