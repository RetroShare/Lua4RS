#include <assert.h>
#include <iostream>

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <retroshare/rsinit.h>  // needed for config dir

#include "LuaCore.h"
#include "LuaList.h"
#include "../Lua4RSWidget.h"
#include "../Lua4RSNotify.h"
#include "../Lua4RSTickThread.h"


#include "LuaToRS.cpp"
#include "LuaToRSPeers.cpp"

LuaCore* LuaCore::_instance;

LuaCore::LuaCore() :
    _folderName ("Lua4RS"),
    _mutex      ("Lua4RS"),
    _peers      (NULL),
    _luaList    (new LuaList()),
    _notify     (new Lua4RSNotify()),
    _thread     (new Lua4RSTickThread()),
    _shutDownImminent (false)
{
    /*
     * Notes:
     *  - Lua files are loaded when _peers is set
     *  - RS functions get registered to Lua when GUI is initialized
     */
    L = luaL_newstate();
    luaL_openlibs(L);

    // start tick thread (after everything else is setup)
    _thread->start();
}

LuaCore::~LuaCore()
{
    // disbale output - gui might be gone by now!
    _ui->disableOutput();
    _shutDownImminent = true;

    // stop thread and wait for shutdown
    _thread->join();

    // send shotdown event
    {
        LuaEvent e;
        e.eventId = L4R_SHUTDOWN;
        e.timeStamp = QDateTime::currentDateTime();

        processEvent(e);
    }

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
    std::string namespc; // namespace (with '.' at the end)

    // two namespaces
    tw->setColumnCount(2);

    // setup tree widget (after ColumnCount was set!)
    // no headers to not to confuse the user
    tw->setHeaderHidden(true);
    // make it big enough to show the full functionnames
    tw->setColumnWidth(0,250);
    // second col is only a "container" for the paste value, therefore it needs no width
    tw->setColumnWidth(1,0);

    // we need to mess around with lua
    RsStackMutex mtx(_mutex);   /******* LOCKED MUTEX *****/

    // rs namespace
    namespc = "rs.";
    QTreeWidgetItem *rs = new QTreeWidgetItem(tw);
    rs->setText(0, QString::fromStdString(namespc));
    rs->setText(1, QString::fromStdString(namespc));
    lua_newtable(L);
    top = lua_gettop(L);

    addFunctionToLuaAndTw(top, rs_clear, "clear()", "clears the output", namespc, rs);
    addFunctionToLuaAndTw(top, rs_print, "print()", "prints to output", namespc, rs);

    lua_setglobal(L, "rs");

    // peers namespace
    namespc = "peers.";
    QTreeWidgetItem *peers = new QTreeWidgetItem(tw);
    peers->setText(0, QString::fromStdString(namespc));
    peers->setText(1, QString::fromStdString(namespc));
    lua_newtable(L);
    top = lua_gettop(L);

    addFunctionToLuaAndTw(top, peers_getOwnId, "getOwnId()", "returns own SSL id", namespc, peers);
    addFunctionToLuaAndTw(top, peers_getOnlineList, "getOnlineList()", "returns list of online friends (SSL id)", namespc, peers);
    addFunctionToLuaAndTw(top, peers_getFriendList, "getFriendList()", "returns list of all friends (SSL id)", namespc, peers);
    addFunctionToLuaAndTw(top, peers_getPeerCount, "getPeerCount()", "returns number of all friends and online friends", namespc, peers);
    addFunctionToLuaAndTw(top, peers_getPeerName, "getPeerName()", "returns the name for a given SSL/PGP id", namespc, peers);
    addFunctionToLuaAndTw(top, peers_getPeerDetails, "getPeerDetails()", "returns peer details as a table for a given SSL id", namespc, peers);

    lua_setglobal(L, "peers");
}

void LuaCore::addFunctionToLuaAndTw(int tableTop, int (*f)(lua_State*), const std::string& name, const std::string& hint, const std::string& namespc, QTreeWidgetItem* item)
{
    QTreeWidgetItem *i = new QTreeWidgetItem(item);
    i->setText(0, QString::fromStdString(name));
    i->setText(1, QString::fromStdString(namespc + name));
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

void LuaCore::processEvent(LuaEvent &e)
{
    // to catch to early events
    if(!sane())
    {
        std::cerr << "[Lua] LuaCore not ready - event " << e.eventId  << std::endl;
        return;
    }

    // block everythign except onShutdown when RS is exiting
    if(_shutDownImminent && e.eventId != L4R_SHUTDOWN)
        return;

    // do some magic here
    std::cout << "[Lua] processing event : " << e.eventId  << std::endl;
    for(LuaContainerList::const_iterator it = _luaList->begin(); it != _luaList->end(); ++it)
    {
        if((*it)->isTriggered(e))
            runLuaByEvent((*it), e);
    }
}

// invoke lua
void LuaCore::runLuaByString(const QString& code)
{
    if(_ui == NULL)
    {
        std::cerr << "[Lua] runLuaByString: ERROR: _ui is NULL -> aborting Lua execution" << std::endl;
        return;
    }

    std::string code2;
#ifdef _WIN32
    code2 = code.toLocal8Bit().constData();
#else
    code2 = code.toUtf8().constData();
#endif

    RsStackMutex mtx(_mutex);   /******* LOCKED MUTEX *****/
    int ret = luaL_dostring(L, code2.c_str());
    reportLuaErrors(L, ret);
}

void LuaCore::runLuaByName(const QString& name)
{
    /*
    parameterMap m;
    runLuaByNameWithParams(name, m);
    */
    // get code
    LuaContainer* lc = NULL;
    if(!_luaList->itemByName(name, lc))
    {
        std::cerr << "[Lua] can't find script " << name.toStdString() << std::endl;
        return;
    }

    runLuaByString(lc->getCode());
}

/*
void LuaCore::runLuaByNameWithParams(const QString& name, parameterMap paramMap)
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
*/

void LuaCore::runLuaByEvent(LuaContainer* container, const LuaEvent& event)
{
    // do some magic with parameters from event

    emit _ui->appendLog("triggered script: " + container->getName());
    runLuaByString(container->getCode());
}

void LuaCore::reportLuaErrors(lua_State *L, int status)
{
    std::string s;
    if ( status!=0 ) {
        s = lua_tostring(L, -1);
        std::cerr << "-- " << s << std::endl;

        s = "Lua error: " + s;
        emit _ui->appendLog(s);

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
