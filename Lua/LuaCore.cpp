#include <assert.h>
#include <iostream>

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <rsserver/rsaccounts.h>

#include "LuaCore.h"
#include "LuaList.h"
#include "gui/Lua4RSWidget.h"
#include "Lua4RSNotify.h"

#include "LuaToRS.cpp"
#include "LuaToRSChat.cpp"
#include "LuaToRSDiscovery.cpp"
#include "LuaToRSPeers.cpp"
#include "LuaToRSServerConfig.cpp"

#include "helper.h"

LuaCore::LuaCore() :
    _folderName ("Lua4RS"),
    _mutex      ("Lua4RS"),
    _luaList    (new LuaList()),
    _notify     (new Lua4RSNotify()),
    _processingEvent(false),
    _shutDownImminent (false)
{
    /*
     * Notes:
     *  - RS functions get registered to Lua when GUI is initialized
     */
    L = luaL_newstate();
    luaL_openlibs(L);

    _path = rsAccounts->PathAccountDirectory() + "/" + _folderName + "/";

    // load codes
    _luaList->setFilePath(_path);
    if(_luaList->loadAll())
        std::cout << "[Lua] loaded " << _luaList->size() << " Lua script(s)" << std::endl;
    else
        std::cout << "[Lua] didn't load any Lua scripts" << std::endl;
}

LuaCore::~LuaCore()
{
}

void LuaCore::shutDown()
{
    // disbale output - gui might be gone by now!
    _ui->disableOutput();
    _shutDownImminent = true;

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
    QTreeWidgetItem* rs = new QTreeWidgetItem(tw);
    rs->setText(0, QString::fromStdString(namespc));
    rs->setText(1, QString::fromStdString(namespc));
    lua_newtable(L);
    top = lua_gettop(L);

    addFunctionToLuaAndTw(top, namespc, rs, rs_clear,       "clear()",      QObject::tr("clears the output"));
    addFunctionToLuaAndTw(top, namespc, rs, rs_print,       "print()",      QObject::tr("prints to output"));
    addFunctionToLuaAndTw(top, namespc, rs, rs_luaFolder,   "luaFolder()",  QObject::tr("path to Lua4RS folder"));

    lua_setglobal(L, "rs");

    // peers namespace
    namespc = "peers.";
    QTreeWidgetItem* peers = new QTreeWidgetItem(tw);
    peers->setText(0, QString::fromStdString(namespc));
    peers->setText(1, QString::fromStdString(namespc));
    lua_newtable(L);
    top = lua_gettop(L);

    addFunctionToLuaAndTw(top, namespc, peers, peers_getOwnId,          "getOwnId()",       QObject::tr("returns own SSL id (peer id)"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getOnlineList,     "getOnlineList()",  QObject::tr("returns list of online friends (SSL id)"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getFriendList,     "getFriendList()",  QObject::tr("returns list of all friends (SSL id)"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getPeerCount,      "getPeerCount()",   QObject::tr("returns number of all friends and online friends"));

    addFunctionToLuaAndTw(top, namespc, peers, peers_isFriend,          "isFriend()",       QObject::tr("returns if a peer is a friend"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_isGPGAccepted,     "isGPGAccepted()",  QObject::tr("returns is a PGP key is accepted"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_isOnline,          "isOnline()",       QObject::tr("returns if a peer is online"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getGPGName,        "getGPGName()",     QObject::tr("returns the PGP name for a given PGP id"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getPeerName,       "getPeerName()",    QObject::tr("returns the name for a given SSL/PGP id"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getPeerDetails,    "getPeerDetails()", QObject::tr("returns peer details as a table for a given SSL id"));

    addFunctionToLuaAndTw(top, namespc, peers, peers_getGPGOwnId,       "getGPGOwnId()",    QObject::tr("returns own PGP id"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getGPGId,          "getGPGId()",       QObject::tr("returns the PGP id for a given SSL/PGP id"));

    //groups
    addFunctionToLuaAndTw(top, namespc, peers, peers_addGroup,          "addGroup()",           QObject::tr("creates a new group"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_editGroup,         "editGroup()",          QObject::tr("edits an existing group"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_removeGroup,       "removeGroup()",        QObject::tr("removes the group with the given groupd id"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getGroupInfo,      "getGroupInfo()",       QObject::tr("returns group info for a given group id"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getGroupInfoList,  "getGroupInfoList()",   QObject::tr("returns an array with all groups and their group infos"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_assignPeerToGroup, "assignPeerToGroup()",  QObject::tr("returns the PGP id for a given SSL/PGP id"));

    lua_setglobal(L, "peers");

    // server config
    namespc = "config.";
    QTreeWidgetItem* config = new QTreeWidgetItem(tw);
    config->setText(0, QString::fromStdString(namespc));
    config->setText(1, QString::fromStdString(namespc));
    lua_newtable(L);
    top = lua_gettop(L);

    addFunctionToLuaAndTw(top, namespc, config, config_getOperatingMode,    "getOperatingMode()",   QObject::tr("returns the current operation mode as int and string"));
    addFunctionToLuaAndTw(top, namespc, config, config_setOperatingMode,    "setOperatingMode()",   QObject::tr("sets the openration mode (takes int or string)"));

    addFunctionToLuaAndTw(top, namespc, config, config_setMaxDataRates,     "setMaxDataRates()",    QObject::tr("sets max down-/upload bandwidth in kB"));
    addFunctionToLuaAndTw(top, namespc, config, config_getMaxDataRates,     "getMaxDataRates()",    QObject::tr("gets max down-/upload bandwidth in kB"));
    addFunctionToLuaAndTw(top, namespc, config, config_getCurrentDataRates, "getCurrentDataRates()",QObject::tr("gets current down-/upload bandwidth in kB"));

    lua_setglobal(L, "config");

    // discovery
    namespc = "disc.";
    QTreeWidgetItem* discovery = new QTreeWidgetItem(tw);
    discovery->setText(0, QString::fromStdString(namespc));
    discovery->setText(1, QString::fromStdString(namespc));
    lua_newtable(L);
    top = lua_gettop(L);

    addFunctionToLuaAndTw(top, namespc, discovery, disc_getDiscFriends,     "getDiscFriends()",     QObject::tr("gets discovery infos for a SSLID"));
    addFunctionToLuaAndTw(top, namespc, discovery, disc_getDiscPgpFriends,  "getDiscPgpFriends()",  QObject::tr("gets discovery infos for a PGPID"));
    addFunctionToLuaAndTw(top, namespc, discovery, disc_getPeerVersion,     "getPeerVersion()",     QObject::tr("gets RS version from a given peer"));
    addFunctionToLuaAndTw(top, namespc, discovery, disc_getWaitingDiscCount,"getWaitingDiscCount()",QObject::tr("gets current pending discovery packets (down und up)"));

    lua_setglobal(L, "disc");

    // chat
    namespc = "chat.";
    QTreeWidgetItem* chat = new QTreeWidgetItem(tw);
    chat->setText(0, QString::fromStdString(namespc));
    chat->setText(1, QString::fromStdString(namespc));
    lua_newtable(L);
    top = lua_gettop(L);

    addFunctionToLuaAndTw(top, namespc, chat, chat_sendChat, "sendChat()", QObject::tr("send a chat message (ChatId, msg)"));

    lua_setglobal(L, "chat");

    // files
    namespc = "files.";
    QTreeWidgetItem* files = new QTreeWidgetItem(tw);
    files->setText(0, QString::fromStdString(namespc));
    files->setText(1, QString::fromStdString(namespc));
    lua_newtable(L);
    top = lua_gettop(L);

    addFunctionToLuaAndTw(top, namespc, files, file_fileRequest, "fileRequest()", QObject::tr("request a download (params: *name*, *hash*, *size*"));

    lua_setglobal(L, "files");
}

void LuaCore::addFunctionToLuaAndTw(int tableTop, const std::string& namespc, QTreeWidgetItem* item, int (*f)(lua_State*), const std::string& name, const QString& hint)
{
    QTreeWidgetItem *i = new QTreeWidgetItem(item);
    i->setText(0, QString::fromStdString(name));
    i->setText(1, QString::fromStdString(namespc + name));
    i->setToolTip(0, hint);

    // name can be like foo(bar) but function name is just foo
    std::string luaFuncName;
    size_t pos;
    if((pos = name.find_first_of('(')) != std::string::npos)
        luaFuncName = name.substr(0, pos);
    else
        luaFuncName = name;

    pushTable(L, tableTop, luaFuncName, f);
}

bool LuaCore::processEvent(const LuaEvent& e)
{
    // to catch to early events
    if(_ui == NULL)
    {
        std::cerr << "[Lua] LuaCore not ready - event " << e.eventId  << std::endl;
        return false;
    }

    // block everythign except onShutdown when RS is exiting
    if(_shutDownImminent && e.eventId != L4R_SHUTDOWN)
        return false;

    // exit when we are already executing code for a previous event
    if(_processingEvent)
        return false;
    _processingEvent = true;
    // do some magic here
    // std::cout << "[Lua] processing event : " << e.eventId  << std::endl;
    for(LuaContainerList::const_iterator it = _luaList->begin(); it != _luaList->end(); ++it)
    {
        if((*it)->isTriggered(e))
            runLuaByEvent((*it), e);
    }
    _processingEvent = false;
    return true;
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
    // get code
    LuaContainer* lc = NULL;
    if(!_luaList->itemByName(name, lc))
    {
        std::cerr << "[Lua] can't find script " << name.toStdString() << std::endl;
        return;
    }

    runLuaByString(lc->getCode());
}

void LuaCore::runLuaByEvent(LuaContainer* container, const LuaEvent& event)
{
    {
        RsStackMutex mtx(_mutex);   /******* LOCKED MUTEX *****/

        // clear old parameter
        luaL_dostring(L, "args = nil");

        // set parameter
        lua_newtable(L);
        int top = lua_gettop(L);

        QStringList keys = event.dataParm->allKeys();
        for(QStringList::ConstIterator it = keys.begin(); it != keys.end(); it++)
        {
            QString key = *it;
            QString type = key.mid(0, 3);
            //std::cout << "[Lua] runByEvent adding type " << type.toStdString() << " key is " << key.toStdString() << std::endl;
            if(!(type == "str" || type == "int"  || type == "u32" || type != "u64"))
                continue;

            QString name = key.mid(3);
            QVariant value = event.dataParm->value(key);

            //std::cout << "[Lua] runByEvent adding " << name.toStdString() << " with " << value.toString().toStdString() << std::endl;

            lua_pushfstring(L, name.toStdString().c_str());
            if(type == "str") {
                // PANIC: unprotected error in call to Lua API (invalid option '%1' to 'lua_pushfstring')
                ///TODO proper fix
                std::string s = value.toString().toStdString();
                replaceAll(s, "%", "");
                lua_pushfstring(L, s.c_str());
            }
            else if(type == "int")
                lua_pushinteger(L, value.toInt());
            else if(type == "u32")
                lua_pushinteger(L, value.toUInt());
            else if(type == "u64")
                lua_pushinteger(L, value.toULongLong());

            lua_settable(L, top);
        }

        lua_setglobal(L, "args");

    }
    emit appendLog(QObject::tr("running (by event): ") + container->getName());
    runLuaByString(container->getCode());
}

void LuaCore::reportLuaErrors(lua_State *L, int status)
{
    std::string s;
    if ( status!=0 ) {
        s = lua_tostring(L, -1);
        std::cerr << "-- " << s << std::endl;

        s = "Lua error: " + s;
        emit appendLog(QString::fromStdString(s));

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

std::string LuaCore::getPath() const
{
    return _path;
}

void LuaCore::setUi(Lua4RSWidget *ui)
{
    _ui = ui;

    QObject::connect(this, SIGNAL(appendLog(QString)), _ui, SLOT(appendLog(QString)));
    QObject::connect(this, SIGNAL(appendOutput(QString)), _ui, SLOT(appendOutput(QString)));
    QObject::connect(this, SIGNAL(clearOutput()), _ui, SLOT(clearOutput()));
}

Lua4RSWidget* LuaCore::getUI()
{
    ///TODO better fix
    assert(_ui);

    return _ui;
}

// emit
void LuaCore::emitAppendOutput(const QString &s)    { emit appendOutput(s); }
void LuaCore::emitClearOutput()                     { emit clearOutput(); }
