#ifndef P3LUA4RS_H
#define P3LUA4RS_H

#include <retroshare/rsplugin.h>
#include <plugins/rspqiservice.h>

/*
 * $ echo 'Lua4RS' | hexdump
 * 0000000 754c 3461 5352 000a
 *
 * --> use 754c as service typ
 * --> use 754c 3461 as config typ
 */
extern const u_int16_t RS_SERVICE_TYPE_PLUGIN_LUA4RS;
extern const u_int32_t CONFIG_TYPE_LUA4RS;


class p3Lua4RS : public RsPQIService
{
public:
    p3Lua4RS(RsPluginHandler *pgHandler);

    /****************** p3Service STUFF ******************/
    virtual int tick();
};

#endif // P3LUA4RS_H
