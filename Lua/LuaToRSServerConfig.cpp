#include <retroshare/rsconfig.h>

#include "LuaCore.h"
#include "LuaToRS.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}


extern "C" {
    //	/* Operating Mode */
    //virtual uint32_t getOperatingMode() = 0;
    int config_getOperatingMode(lua_State* L)
    {
        const uint32_t opMode = rsConfig->getOperatingMode();
        lua_pushinteger(L, opMode);

        std::string opModeS = "";
        switch (opMode) {
        case RS_OPMODE_FULL:
            opModeS = "full";
            break;
        case RS_OPMODE_NOTURTLE:
            opModeS = "noturtle";
            break;
        case RS_OPMODE_MINIMAL:
            opModeS = "minimal";
            break;
        case RS_OPMODE_GAMING:
            opModeS = "gamig";
            break;
        default:
            // return without the string
            return 1;
            break;
        }
        lua_pushstring(L, opModeS.c_str());
        return 2;
    }

    //virtual bool     setOperatingMode(uint32_t opMode) = 0;
    int config_setOperatingMode(lua_State* L)
    {
        if( getArgCount(L) != 1)
            return 0;

        uint32_t opMode = 0;
        // check for int first! a int can easyly be converted to a string (lua_isstring() returned true even for numbers)
        if(lua_isnumber(L, 1))
        {
            const int opModeI = luaL_checkinteger(L, 1);
            //std::cout << "[Lua] config_setOperatingMode got int: " << opModeI << std::endl;
            switch (opModeI) {
            case 1:
                opMode = RS_OPMODE_FULL;
                break;
            case 2:
                opMode = RS_OPMODE_NOTURTLE;
                break;
            case 3:
                opMode = RS_OPMODE_GAMING;
                break;
            case 4:
                opMode = RS_OPMODE_MINIMAL;
                break;
            default:
                // opMode = 0
                break;
            }
        } else {
            const std::string opModeS = luaL_checkstring(L, 1);
            //std::cout << "[Lua] config_setOperatingMode got string: " << opModeS << std::endl;

            if(opModeS == "full")
                opMode = RS_OPMODE_FULL;
            else if (opModeS == "noturtle")
                opMode = RS_OPMODE_NOTURTLE;
            else if (opModeS == "minimal")
                opMode = RS_OPMODE_MINIMAL;
            else if (opModeS == "gaming")
                opMode = RS_OPMODE_GAMING;
            // else opMode = 0
        }

        //std::cout << "[Lua] config_setOperatingMode opMode: " << opMode << std::endl;
        if(opMode == 0)
            // somthing went wrong
            return 0;

        rsConfig->setOperatingMode(opMode);
        return 0;
    }
}


//virtual int 	getConfigNetStatus(RsConfigNetStatus &status) = 0;

//virtual int 	getTotalBandwidthRates(RsConfigDataRates &rates) = 0;
//virtual int 	getAllBandwidthRates(std::map<std::string, RsConfigDataRates> &ratemap) = 0;

//virtual uint32_t getUserLevel() = 0;

//virtual uint32_t getNetState() = 0;
//virtual uint32_t getNetworkMode() = 0;
//virtual uint32_t getNatTypeMode() = 0;
//virtual uint32_t getNatHoleMode() = 0;
//virtual uint32_t getConnectModes() = 0;

//virtual bool getConfigurationOption(uint32_t key, std::string &opt) = 0;
//virtual bool setConfigurationOption(uint32_t key, const std::string &opt) = 0;



//	/* Data Rate Control - to be moved here */
//virtual int SetMaxDataRates( int downKb, int upKb ) = 0;
//virtual int GetMaxDataRates( int &inKb, int &outKb ) = 0;
//virtual int GetCurrentDataRates( float &inKb, float &outKb ) = 0;
