#include <assert.h>
#include <iostream>

#include <QTreeWidget>
#include <QTreeWidgetItem>

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
    luaL_openlibs(L);

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

void LuaCore::setupRsFunctionsAndTw(QTreeWidget* tw)
{
    int top;

    // two namespaces
    tw->setColumnCount(2);

    // rs namespace
    QTreeWidgetItem *rs = new QTreeWidgetItem(tw);
    rs->setText(0, "rs.");
    lua_newtable(L);
    top = lua_gettop(L);

    addFunctionToLuaAndTw(top, rs_clear, "clear()", "clears the output", rs);
    addFunctionToLuaAndTw(top, rs_print, "print()", "prints to output", rs);

    lua_setglobal(L, "rs");

    // peers namespace
    QTreeWidgetItem *peers = new QTreeWidgetItem(tw);
    peers->setText(0, "peers.");
    lua_newtable(L);
    top = lua_gettop(L);

    addFunctionToLuaAndTw(top, peers_getOwnId, "getOwnId()", "returns own SSL id", peers);
    addFunctionToLuaAndTw(top, peers_getOnlineList, "getOnlineList()", "returns list of online friends (SSL id)", peers);
    addFunctionToLuaAndTw(top, peers_getFriendList, "getFriendList()", "returns list of all friends (SSL id)", peers);
    addFunctionToLuaAndTw(top, peers_getPeerCount, "getPeerCount()", "returns number of all friends and online friends", peers);
    addFunctionToLuaAndTw(top, peers_getPeerName, "getPeerName()", "returns the name for a given SSL/PGP id", peers);
    addFunctionToLuaAndTw(top, peers_getPeerDetails, "getPeerDetails()", "returns peer details as a table for a given SSL id", peers);

    lua_setglobal(L, "peers");
}

void LuaCore::addFunctionToLuaAndTw(int tableTop, int (*f)(lua_State*), const std::string& name, const std::string& hint, QTreeWidgetItem* item)
{
    QTreeWidgetItem *i = new QTreeWidgetItem(item);
    i->setText(0, QString::fromStdString(name));
    i->setToolTip(0, QString::fromStdString(hint));

    // name can be like foo(bar) but function name is just foo
    std::string luaFuncName;
    size_t pos;
    if((pos = name.find_first_of('(')) != std::string::npos)
        luaFuncName = name.substr(0, pos);
    else
        luaFuncName = name;

    pushTable(L, tableTop, luaFuncName, f);
}

// invoke lua
void LuaCore::runLuaByString(const std::string& code)
{
    if(_ui == NULL)
    {
        std::cerr << "[Lua] runLuaByString: ERROR: _ui is NULL -> aborting Lua execution" << std::endl;
        return;
    }

    int ret = luaL_dostring(L, code.c_str());
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
        code += it->first + " = " + it->second + "\n";

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
