#include <retroshare/rsconfig.h>

#include "LuaCore.h"
#include "LuaToRS.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

extern "C" {
    //	/* Operating Mode */
    int config_getOperatingMode(lua_State* L)
    {
        const RsOpMode opMode = rsConfig->getOperatingMode();
        lua_pushinteger(L, static_cast<lua_Integer>(opMode));

        std::string opModeS = "";
        switch (opMode) {
        case RsOpMode::FULL:
            opModeS = "full";
            break;
        case RsOpMode::NOTURTLE:
            opModeS = "noturtle";
            break;
        case RsOpMode::MINIMAL:
            opModeS = "minimal";
            break;
        case RsOpMode::GAMING:
            opModeS = "gaming";
            break;
        default:
            // return without the string
            return 1;
            break;
        }
        lua_pushstring(L, opModeS.c_str());
        return 2;
    }

    int config_setOperatingMode(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

        RsOpMode opMode = static_cast<RsOpMode>(0);
        // check for int first! a int can easyly be converted to a string (lua_isstring() returned true even for numbers)
        if(lua_isnumber(L, 1))
        {
            const int opModeI = luaL_checkinteger(L, 1);
            switch (opModeI) {
            case 1:
                opMode = RsOpMode::FULL;
                break;
            case 2:
                opMode = RsOpMode::NOTURTLE;
                break;
            case 3:
                opMode = RsOpMode::GAMING;
                break;
            case 4:
                opMode = RsOpMode::MINIMAL;
                break;
            default:
                // opMode = 0
                break;
            }
        } else {
            const std::string opModeS = luaL_checkstring(L, 1);

            if(opModeS == "full")
                opMode = RsOpMode::FULL;
            else if (opModeS == "noturtle")
                opMode = RsOpMode::NOTURTLE;
            else if (opModeS == "minimal")
                opMode = RsOpMode::MINIMAL;
            else if (opModeS == "gaming")
                opMode = RsOpMode::GAMING;
            // else opMode = 0
        }

        if(opMode == static_cast<RsOpMode>(0))
            // somthing went wrong
            return 0;

        rsConfig->setOperatingMode(opMode);
        return 0;
    }

    // download / upload speed
    int config_setMaxDataRates(lua_State* L)
    {
        if( getArgCount(L) != 2)
            return 0;

        luaL_checktype(L, 1, LUA_TNUMBER);
        luaL_checktype(L, 2, LUA_TNUMBER);

        int kbDown = luaL_checkinteger(L, 1);
        int kbUp = luaL_checkinteger(L, 2);

        rsConfig->SetMaxDataRates(kbDown, kbUp);
        return 0;
    }

    int config_getMaxDataRates(lua_State* L)
    {
        int kbDown, kbUp;
        rsConfig->GetMaxDataRates(kbDown, kbUp);
        lua_pushinteger(L, kbDown);
        lua_pushinteger(L, kbUp);
        return 2;
    }

    int config_getCurrentDataRates(lua_State* L)
    {
        float kbDown, kbUp;
        rsConfig->GetCurrentDataRates(kbDown, kbUp);
        lua_pushnumber(L, kbDown);
        lua_pushnumber(L, kbUp);
        return 2;
    }
}


// TODO: not bound yet, possibly useful (see retroshare/rsconfig.h):
//   getConfigNetStatus, getTotalBandwidthRates, getAllBandwidthRates, getUserLevel,
//   getNetState, getNetworkMode, getNatTypeMode, getNatHoleMode, getConnectModes
