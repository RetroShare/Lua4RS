#include <assert.h>
#include <iostream>

#include <retroshare/rsinit.h>

#include "LuaCore.h"
#include "../Lua4RSWidget.h"

#include "LuaToRS.cpp"
#include "LuaToRSPeers.cpp"

LuaCore* LuaCore::_instance;

LuaCore::LuaCore() :
    _folderName("Lua4RS")
{
    _luaList = new LuaList();
    _notify = new Lua4RSNotify();
    _peers = NULL; // gets set later
    _thread = new Lua4RSTickThread();

    // lua files get loaded when _peer is set

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
    lua_register(L, "rsInit", rs_initRsNamespace);

    // peers
    lua_register(L, "getOwnId", peers_getOwnId);
    lua_register(L, "getOnlineList", peers_getOnlineList);
    lua_register(L, "getFriendList", peers_getFriendList);
    lua_register(L, "getPeerCount", peers_getPeerCount);
    lua_register(L, "getPeerName", peers_getPeerName);
    lua_register(L, "getPeerDetails", peers_getPeerDetails);

    // start tick thread (after everything else is setup)
    _thread->start();
}

LuaCore::~LuaCore()
{
    // stop thread and wait for shutdown
    _thread->join();

    // save lua scripts
    if(_luaList->saveAll())
        std::cout << "[Lua] saved " << _luaList->size() << " Lua script(s)" << std::endl;
    else
        std::cout << "[Lua] error occured while saving Lua scripts" << std::endl;

    // close lua (after threads is stopped)
    lua_close(L);

    delete _notify;
    delete _luaList;
}

void LuaCore::setPeers(RsPeers* peers)
{
    _peers = peers;
    _path = RsInit::RsConfigDirectory() + "/" + _peers->getOwnId() + "/" + _folderName + "/";

    // load codes
    _luaList->setFilePath(_path);
    if(_luaList->loadAll())
        std::cout << "[Lua] loaded " << _luaList->size() << " Lua script(s)" << std::endl;
    else
        std::cout << "[Lua] didn't load any Lua scripts" << std::endl;
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
    LuaContainer* lc = NULL;
    if(!_luaList->itemByName(name, lc))
    {
        std::cerr << "[Lua] can't find script " << name << std::endl;
        return;
    }
    code += lc->getCode().toStdString();

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

// getter & setter
Lua4RSNotify* LuaCore::notify() const
{
    return _notify;
}

LuaList* LuaCore::codeList() const
{
    return _luaList;
}

RsPeers* LuaCore::peers() const
{
    return _peers;
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
