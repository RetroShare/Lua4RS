#include <assert.h>

#include "LuaCore.h"
#include "../Lua4RSMainWidget.h"

#include "LuaToRS.cpp"
#include "LuaToRSPeers.cpp"

LuaCore* LuaCore::_instance;

LuaCore::LuaCore()
{
    L = luaL_newstate();

    /*
    luaopen_base(L);
    luaopen_io(L);
    luaopen_table(L);
    luaopen_string(L);
    luaopen_math(L);
    luaopen_os(L);
    */
    luaL_openlibs(L);

    lua_register(L, "rs_print", rs_print);
    lua_register(L, "rs_clear", rs_clear);

    // peers
    lua_register(L, "getOwnId", rs_peers_getOwnId);
    lua_register(L, "getOnlineList", rs_peers_getOnlineList);
    lua_register(L, "getFriendList", rs_peers_getFriendList);
    lua_register(L, "getPeerCount", rs_peers_getPeerCount);
    lua_register(L, "getPeerName", rs_peers_getPeerName);
    lua_register(L, "getPeerDetails", rs_peers_getPeerDetails);
}

LuaCore::~LuaCore()
{
    lua_close(L);
}

LuaCore* LuaCore::getInstance()
{
    if (_instance == NULL)
        _instance = new LuaCore;

    return _instance;
}

void LuaCore::setUi(Lua4RSMainWidget *ui)
{
    _ui = ui;
}

Lua4RSMainWidget* LuaCore::getUI()
{
    ///TODO better fix
    assert(_ui);

    return _ui;
}

void LuaCore::runLua(const std::string& code)
{
    ///TODO better fix
    assert(_ui);

    std::cout << "[LUA] executing lua ...";
    int ret = luaL_dostring(L, code.c_str());
    std::cout << "done" << std::endl;
    reportLuaErrors(L, ret);
}

void LuaCore::reportLuaErrors(lua_State *L, int status)
{
    std::string s;
    if ( status!=0 ) {
        s = lua_tostring(L, -1);
        std::cerr << "-- " << s << std::endl;

        s = "Lua error: " + s;
        _ui->appendOutput(s);

        lua_pop(L, 1); // remove error message
    }
}

RsPeers *LuaCore::peers() const
{
    return _peers;
}

void LuaCore::setPeers(RsPeers *peers)
{
    _peers = peers;
}

