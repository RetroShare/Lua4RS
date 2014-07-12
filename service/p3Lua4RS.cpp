#include <iostream>
#include <sstream>

#include <serialiser/rsconfigitems.h>

#include "p3Lua4RS.h"

#define L4RCONFIG_TIMER_STARTUP "L4RCONFIG_TIMER_STARTUP"
#define L4RCONFIG_TIMER_SLEEP   "L4RCONFIG_TIMER_SLEEP"
#define L4RCONFIG_TIMER_TICK    "L4RCONFIG_TIMER_TICK"

/* DEFINE INTERFACE POINTER! */
Lua4RSInterface* L4R::L4RConfig = NULL;

p3Lua4RS::p3Lua4RS(RsPluginHandler* rph) :
    RsPQIService(RS_SERVICE_TYPE_L4R_PLUGIN, CONFIG_TYPE_L4R_PLUGIN, 0, rph)
{
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

    vitem->tlvkvs.pairs.push_back(push_uint_value(L4RCONFIG_TIMER_STARTUP, _secondsToStarUpEvent));
    vitem->tlvkvs.pairs.push_back(push_uint_value(L4RCONFIG_TIMER_SLEEP,   _sleepPeriodInMilliseconds));
    vitem->tlvkvs.pairs.push_back(push_uint_value(L4RCONFIG_TIMER_TICK,    _tickIntervalInSeconds));

    lst.push_back(vitem);

    return true;
}

bool p3Lua4RS::loadList(std::list<RsItem*>& load)
{
    std::cout << "[Lua] p3Lua4RS::loadList" << std::endl;
    for(std::list<RsItem*>::const_iterator it = load.begin(); it!=load.end(); ++it)
    {
        RsConfigKeyValueSet *vitem = dynamic_cast<RsConfigKeyValueSet*>(*it);
        if(vitem != NULL)
            for(std::list<RsTlvKeyValue>::const_iterator kit = vitem->tlvkvs.pairs.begin(); kit != vitem->tlvkvs.pairs.end(); ++kit)
                if(kit->key ==      L4RCONFIG_TIMER_STARTUP)
                    _secondsToStarUpEvent       = pop_uint_value(kit->value);
                else if(kit->key == L4RCONFIG_TIMER_SLEEP)
                    _sleepPeriodInMilliseconds  = pop_uint_value(kit->value);
                else if(kit->key == L4RCONFIG_TIMER_TICK)
                    _tickIntervalInSeconds      = pop_uint_value(kit->value);
                else
                    std::cerr << "[Lua] L4RConfig::loadList : unknown key: " << kit->key << std::endl;

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
    //std::cout << "[Lua] p3Lua4RS::tick" << std::endl;
    return 0;
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

uint p3Lua4RS::getSleepPeriodInMilliseconds() const
{
    return _sleepPeriodInMilliseconds;
}

void p3Lua4RS::setSleepPeriodInMilliseconds(const uint& value)
{
    _sleepPeriodInMilliseconds = value;
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
