#include <retroshare/rsfiles.h>
#include <retroshare/rsturtle.h>

#include "LuaCore.h"
#include "LuaToRS.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

extern "C" {

    //virtual TurtleRequestId turtleSearch(const std::string& match_string) = 0 ;
    //virtual TurtleRequestId turtleSearch(const RsRegularExpression::LinearizedExpression& expr) = 0 ;
    // only words for now

    int file_turtleSearch(lua_State *L)
    {
        luaL_checktype(L, 1, LUA_TSTRING);

        const std::string words = luaL_checkstring(L, 1);
        TurtleRequestId id = rsFiles->turtleSearch(words);

        lua_pushnumber(L, id);
        return 1;
    }

    // virtual bool FileRequest(const std::string& fname, const RsFileHash& hash, uint64_t size, const std::string& dest, TransferRequestFlags flags, const std::list<RsPeerId>& srcIds) = 0;
    int file_fileRequest(lua_State *L)
    {
        //int argc = getArgCount(L);
        luaL_checktype(L, 1, LUA_TSTRING);
        luaL_checktype(L, 2, LUA_TSTRING);
        luaL_checktype(L, 3, LUA_TNUMBER);

        const std::string fileName = luaL_checkstring(L, 1);
        const RsFileHash fileHash = RsFileHash(luaL_checkstring(L, 2));
        const uint64_t fileSize = luaL_checknumber(L, 3);

        std::list<RsPeerId> srcIds;
        const bool ok = rsFiles->FileRequest(fileName, fileHash, fileSize, "", RS_FILE_REQ_ANONYMOUS_ROUTING, srcIds);

        lua_pushboolean(L, (int)ok);
        return 1;
    }

    //virtual void    setDownloadDirectory(std::string path) = 0;
    int file_setDownloadDirectory(lua_State *L)
    {
        //int argc = getArgCount(L);
        luaL_checktype(L, 1, LUA_TSTRING);

        const std::string path = luaL_checkstring(L, 1);

        rsFiles->setDownloadDirectory(path);

        return 0;
    }

    //virtual void    setPartialsDirectory(std::string path) = 0;
    int file_setPartialsDirectory(lua_State *L)
    {
        //int argc = getArgCount(L);
        luaL_checktype(L, 1, LUA_TSTRING);

        const std::string path = luaL_checkstring(L, 1);

        rsFiles->setPartialsDirectory(path);

        return 0;
    }

    //virtual std::string getDownloadDirectory() = 0;
    int file_getDownloadDirectory(lua_State *L)
    {
        //int argc = getArgCount(L);

        const std::string path = rsFiles->getDownloadDirectory();

        lua_pushstring(L, path.c_str());
        return 1;
    }

    //virtual std::string getPartialsDirectory() = 0;
    int file_getPartialsDirectory(lua_State *L)
    {
        //int argc = getArgCount(L);

        const std::string path = rsFiles->getPartialsDirectory();

        lua_pushstring(L, path.c_str());
        return 1;
    }
}
