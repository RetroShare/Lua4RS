#include "p3Lua4RS.h"

#include <iostream>

const u_int16_t RS_SERVICE_TYPE_PLUGIN_LUA4RS = 0x754c;
const u_int32_t CONFIG_TYPE_LUA4RS = 0x754c3461;

p3Lua4RS::p3Lua4RS(RsPluginHandler *pgHandler) :
    RsPQIService(RS_SERVICE_TYPE_PLUGIN_LUA4RS, CONFIG_TYPE_LUA4RS, 1, pgHandler)
{
}

int p3Lua4RS::tick()
{
    std::cout << "tick" << std::endl;
    return 0;
}
