#include <assert.h>
#include <iostream>

#include "LuaCore.h"
#include "../Lua4RSWidget.h"

#include "LuaToRS.cpp"
#include "LuaToRSPeers.cpp"

LuaCore* LuaCore::_instance;

LuaCore::LuaCore()
{
    _codeMap.clear();
    _notify = new Lua4RSNotify();
    _peers = NULL; // gets set later
    _thread = new Lua4RSTickThread();

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
/*
    lua_register(L, "getOwnId", rs_peers_getOwnId);
    lua_register(L, "getOnlineList", rs_peers_getOnlineList);
    lua_register(L, "getFriendList", rs_peers_getFriendList);
    lua_register(L, "getPeerCount", rs_peers_getPeerCount);
    lua_register(L, "getPeerName", rs_peers_getPeerName);
    lua_register(L, "getPeerDetails", rs_peers_getPeerDetails);
*/

    // start tick thread (after everything els is setup)
    _thread->start();
}

LuaCore::~LuaCore()
{
    // stop thread and wait for shutdown
    _thread->join();
    _thread->stop();

    // close lua (after threads is stopped)
    lua_close(L);
}

LuaCore* LuaCore::getInstance()
{
    if (_instance == NULL)
        _instance = new LuaCore();

    return _instance;
}

void LuaCore::shutDown()
{
    delete _instance;
    _instance = NULL;
}

void LuaCore::setUi(Lua4RSWidget *ui)
{
    _ui = ui;
}

Lua4RSWidget* LuaCore::getUI()
{
    ///TODO better fix
    assert(_ui);

    return _ui;
}

// invoke lua
void LuaCore::runLuaByString(const std::string& code)
{
    if(_ui == NULL)
    {
        std::cerr << "[Lua] runLuaByString: ERROR: _ui is NULL -> aborting Lua execution" << std::endl;
        return;
    }

    // std::cout << "[LUA] executing lua ... ";
    int ret = luaL_dostring(L, code.c_str());
    // std::cout << "done" << std::endl;
    reportLuaErrors(L, ret);
}

void LuaCore::runLuaByName(const std::string& name)
{
    parameterMap m;
    runLuaByNameWithParams(name, m);
}

void LuaCore::runLuaByNameWithParams(const std::string& name, parameterMap paramMap)
{
    std::string code = "";

    // set parameters
    for(std::map<std::string, std::string>::iterator it = paramMap.begin(); it != paramMap.end(); ++it)
        code += it->first + " = " + it->second + '\n';

    // get code
    codeMap::iterator it = _codeMap.find(name);
    if(it == _codeMap.end())
        return;
    code += it->second.code();

    runLuaByString(code);
}

void LuaCore::reportLuaErrors(lua_State *L, int status)
{
    std::string s;
    if ( status!=0 ) {
        s = lua_tostring(L, -1);
        std::cerr << "-- " << s << std::endl;

        s = "Lua error: " + s;
        _ui->appendLog(s);

        lua_pop(L, 1); // remove error message
    }
}

Lua4RSNotify *LuaCore::notify() const
{
    return _notify;
}

RsPeers *LuaCore::peers() const
{
    return _peers;
}

void LuaCore::setPeers(RsPeers *peers)
{
    _peers = peers;
}

