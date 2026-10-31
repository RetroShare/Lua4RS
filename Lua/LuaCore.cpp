#include <assert.h>
#include <iostream>

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <retroshare/rsinit.h>

#include "LuaCore.h"
#include "LuaList.h"
#include "LuaJsonApi.h"
#include "LuaExecutionGuard.h"
#include <memory>
#include <stdexcept>
#include <vector>
#include "BundledScripts.h"
#include "gui/Lua4RSWidget.h"
#include "gui/FunctionBrowser.h"
#include "Lua4RSNotify.h"

#include "LuaToRS.cpp"
#include "LuaToRSFile.cpp"
#include "LuaToRSChat.cpp"
#include "LuaToRSDiscovery.cpp"
#include "LuaToRSPeers.cpp"
#include "LuaToRSServerConfig.cpp"

#include "helper.h"

namespace {
int printToOutput(lua_State* state)
{
    auto core = static_cast<LuaCore*>(lua_touserdata(state, lua_upvalueindex(1)));
    QStringList parts;
    const int count = lua_gettop(state);
    for(int i = 1; i <= count; ++i)
    {
        size_t length = 0;
        const char* text = luaL_tolstring(state, i, &length);
        parts.append(QString::fromUtf8(text, static_cast<int>(length)));
        lua_pop(state, 1);
    }
    core->emitAppendOutput(parts.join('\t'));
    return 0;
}
}

LuaCore::LuaCore() :
    _folderName ("Lua4RS"),
    _mutex      ("Lua4RS"),
    _mutexRunLua("Lua4RS Lua"),
    _luaList    (new LuaList()),
    _notify     (new Lua4RSNotify()),
    _ui         (nullptr)
{
    /*
     * Notes:
     *  - RS functions get registered to Lua when GUI is initialized
     */
    L = luaL_newstate();
    if(!L) throw std::runtime_error("Cannot allocate Lua state");
    luaL_openlibs(L);
    registerLuaJsonApi(L);
    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, printToOutput, 1);
    lua_setglobal(L, "print");

    _path = RsAccounts::AccountDirectory() + "/" + _folderName + "/";

    // load codes
    _luaList->setFilePath(_path);
    QString exampleError;
    if(!BundledScripts::install(QString::fromStdString(_path), exampleError))
        std::cerr << "[Lua] " << exampleError.toStdString() << std::endl;
    if(_luaList->loadAll())
        std::cout << "[Lua] loaded " << _luaList->size() << " Lua script(s)" << std::endl;
    else
        std::cout << "[Lua] didn't load any Lua scripts" << std::endl;
}

LuaCore::~LuaCore()
{
    shutDown();
    delete _notify;
    delete _luaList;
}

void LuaCore::requestStop()
{
    _shutDownImminent = true;
    _cancel = true;
    _notify->stop();
}

// Caller must stop/join the service thread before entering this function.
void LuaCore::shutDown()
{
    if(!L) return;
    requestStop();
    {
        RsStackMutex lock(_mutex);
        _events.clear();
        _manualCode.clear();
        _manualPending = false;
    }
    _cancel = false; // Give shutdown handlers the same bounded execution budget.
    LuaEvent event;
    event.eventId = L4R_SHUTDOWN;
    event.timeStamp = QDateTime::currentDateTime();
    if(_ready) dispatchEvent(event);
    _cancel = true;
    {
        std::lock_guard<std::recursive_mutex> lock(_scriptsMutex);
        if(!_luaList->saveAll())
            std::cerr << "[Lua] Could not save all scripts during shutdown" << std::endl;
    }
    RsStackMutex lock(_mutexRunLua);
    lua_close(L);
    L = nullptr;
    // Keep containers alive until destruction: the GUI may still refer to them.
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
    RsStackMutex mtx(_mutexRunLua);   /******* LOCKED MUTEX *****/

    FunctionBrowser::addJsonEndpoints(tw);
    QTreeWidgetItem* json = new QTreeWidgetItem(tw);
    json->setText(0, "JSON API client helpers");
    json->setToolTip(0, QObject::tr("Connection and JSON utilities. subscribe is standalone-only; use Automation triggers in the GUI."));
    const QStringList jsonFunctions = {
        "configure({url = \"http://127.0.0.1:9092\", username = \"YOUR_USER\", password = \"YOUR_PASSWORD\"})", "call(\"/rsPeers/getFriendList\", {})",
        "encode({})", "decode(\"{}\")", "array()", "null"
    };
    for(const QString& function : jsonFunctions)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(json);
        item->setText(0, function);
        item->setText(1, "jsonapi." + function);
        item->setToolTip(0, QObject::tr("RetroShare JSON API client. See JSON_API.md for connection settings and examples."));
    }

    auto native = new QTreeWidgetItem(tw, {QObject::tr("Native Lua helpers (GUI only)")});
    native->setToolTip(0, QObject::tr("Direct plugin bindings; no JSON API connection required. Replace placeholder arguments before running."));

    // rs namespace
    namespc = "rs.";
    QTreeWidgetItem* rs = new QTreeWidgetItem(native);
    rs->setText(0, QString::fromStdString(namespc));
    rs->setText(1, QString::fromStdString(namespc));
    lua_newtable(L);
    top = lua_gettop(L);

    addFunctionToLuaAndTw(top, namespc, rs, rs_clear,       "clear()",      QObject::tr("clears the output"));
    addFunctionToLuaAndTw(top, namespc, rs, rs_print,       "print(\"message\")",      QObject::tr("prints to output"));
    addFunctionToLuaAndTw(top, namespc, rs, rs_luaFolder,   "luaFolder()",  QObject::tr("path to Lua4RS folder"));

    lua_setglobal(L, "rs");

    // peers namespace
    namespc = "peers.";
    QTreeWidgetItem* peers = new QTreeWidgetItem(native);
    peers->setText(0, QString::fromStdString(namespc));
    peers->setText(1, QString::fromStdString(namespc));
    lua_newtable(L);
    top = lua_gettop(L);

    addFunctionToLuaAndTw(top, namespc, peers, peers_getOwnId,          "getOwnId()",       QObject::tr("returns own SSL id (peer id)"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getOnlineList,     "getOnlineList()",  QObject::tr("returns list of online friends (SSL id)"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getFriendList,     "getFriendList()",  QObject::tr("returns list of all friends (SSL id)"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getPeerCount,      "getPeerCount()",   QObject::tr("returns number of all friends and online friends"));

    addFunctionToLuaAndTw(top, namespc, peers, peers_isFriend,          "isFriend(\"SSL_ID\")",       QObject::tr("returns if a peer is a friend"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_isGPGAccepted,     "isGPGAccepted(\"PGP_ID\")",  QObject::tr("returns is a PGP key is accepted"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_isOnline,          "isOnline(\"SSL_ID\")",       QObject::tr("returns if a peer is online"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getGPGName,        "getGPGName(\"PGP_ID\")",     QObject::tr("returns the PGP name for a given PGP id"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getPeerName,       "getPeerName(\"SSL_ID\")",    QObject::tr("returns the name for a given SSL/PGP id"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getPeerDetails,    "getPeerDetails(\"SSL_ID\")", QObject::tr("returns peer details as a table for a given SSL id"));

    addFunctionToLuaAndTw(top, namespc, peers, peers_getGPGOwnId,       "getGPGOwnId()",    QObject::tr("returns own PGP id"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getGPGId,          "getGPGId(\"SSL_ID\")",       QObject::tr("returns the PGP id for a given SSL/PGP id"));

    //groups
    addFunctionToLuaAndTw(top, namespc, peers, peers_addGroup,          "addGroup({flag = 0, id = \"\", name = \"Group name\"})",           QObject::tr("creates a new group"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_editGroup,         "editGroup(\"GROUP_ID\", {flag = 0, id = \"GROUP_ID\", name = \"Group name\"})",          QObject::tr("edits an existing group"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_removeGroup,       "removeGroup(\"GROUP_ID\")",        QObject::tr("removes the group with the given groupd id"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getGroupInfo,      "getGroupInfo(\"GROUP_ID\")",       QObject::tr("returns group info for a given group id"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_getGroupInfoList,  "getGroupInfoList()",   QObject::tr("returns an array with all groups and their group infos"));
    addFunctionToLuaAndTw(top, namespc, peers, peers_assignPeerToGroup, "assignPeerToGroup(\"GROUP_ID\", \"PGP_ID\", true)",  QObject::tr("Assign or remove a PGP identity from a group (group ID, PGP ID, assign boolean); returns nothing"));

    lua_setglobal(L, "peers");

    // server config
    namespc = "config.";
    QTreeWidgetItem* config = new QTreeWidgetItem(native);
    config->setText(0, QString::fromStdString(namespc));
    config->setText(1, QString::fromStdString(namespc));
    lua_newtable(L);
    top = lua_gettop(L);

    addFunctionToLuaAndTw(top, namespc, config, config_getOperatingMode,    "getOperatingMode()",   QObject::tr("returns the current operation mode as int and string"));
    addFunctionToLuaAndTw(top, namespc, config, config_setOperatingMode,    "setOperatingMode(\"full\")",   QObject::tr("sets the openration mode (takes int or string)"));

    addFunctionToLuaAndTw(top, namespc, config, config_setMaxDataRates,     "setMaxDataRates(1000, 100)",    QObject::tr("sets max down-/upload bandwidth in kB"));
    addFunctionToLuaAndTw(top, namespc, config, config_getMaxDataRates,     "getMaxDataRates()",    QObject::tr("gets max down-/upload bandwidth in kB"));
    addFunctionToLuaAndTw(top, namespc, config, config_getCurrentDataRates, "getCurrentDataRates()",QObject::tr("gets current down-/upload bandwidth in kB"));

    lua_setglobal(L, "config");

    // discovery
    namespc = "disc.";
    QTreeWidgetItem* discovery = new QTreeWidgetItem(native);
    discovery->setText(0, QString::fromStdString(namespc));
    discovery->setText(1, QString::fromStdString(namespc));
    lua_newtable(L);
    top = lua_gettop(L);

    addFunctionToLuaAndTw(top, namespc, discovery, disc_getDiscFriends,     "getDiscFriends(\"SSL_ID\")",     QObject::tr("gets discovery infos for a SSLID"));
    addFunctionToLuaAndTw(top, namespc, discovery, disc_getDiscPgpFriends,  "getDiscPgpFriends(\"PGP_ID\")",  QObject::tr("gets discovery infos for a PGPID"));
    addFunctionToLuaAndTw(top, namespc, discovery, disc_getPeerVersion,     "getPeerVersion(\"SSL_ID\")",     QObject::tr("gets RS version from a given peer"));
    addFunctionToLuaAndTw(top, namespc, discovery, disc_getWaitingDiscCount,"getWaitingDiscCount()",QObject::tr("gets current pending discovery packets (down und up)"));

    lua_setglobal(L, "disc");

    // chat
    namespc = "chat.";
    QTreeWidgetItem* chat = new QTreeWidgetItem(native);
    chat->setText(0, QString::fromStdString(namespc));
    chat->setText(1, QString::fromStdString(namespc));
    lua_newtable(L);
    top = lua_gettop(L);

    addFunctionToLuaAndTw(top, namespc, chat, chat_sendChat, "sendChat(\"CHAT_ID\", \"message\")", QObject::tr("send a chat message (ChatId, msg)"));

    lua_setglobal(L, "chat");

    // files
    namespc = "files.";
    QTreeWidgetItem* files = new QTreeWidgetItem(native);
    files->setText(0, QString::fromStdString(namespc));
    files->setText(1, QString::fromStdString(namespc));
    lua_newtable(L);
    top = lua_gettop(L);

    addFunctionToLuaAndTw(top, namespc, files, file_fileRequest,  "fileRequest(\"filename\", \"FILE_HASH\", 0)",					QObject::tr("Request a download (name, hash, size in bytes); replace the example size"));
    addFunctionToLuaAndTw(top, namespc, files, file_getDownloadDirectory, "getDownloadDirectory()", QObject::tr("get download folder path"));
    addFunctionToLuaAndTw(top, namespc, files, file_getPartialsDirectory, "getPartialsDirectory()", QObject::tr("get partial folder path"));
    addFunctionToLuaAndTw(top, namespc, files, file_turtleSearch, "turtleSearch(\"search words\")",					QObject::tr("starts a search for given keywords"));
    addFunctionToLuaAndTw(top, namespc, files, file_setDownloadDirectory, "setDownloadDirectory(\"PATH\")", QObject::tr("set download folder path (params: path)"));
    addFunctionToLuaAndTw(top, namespc, files, file_setPartialsDirectory, "setPartialsDirectory(\"PATH\")", QObject::tr("set partial folder path (params: path)"));

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

void LuaCore::processEvent(LuaEvent *e)
{
    std::unique_ptr<LuaEvent> event(e);
    if(!event) return;
    RsStackMutex lock(_mutex);
    // Drop new arrivals when overloaded. Ownership is released on every path.
    if(!_shutDownImminent) _events.push(std::move(event));
}

void LuaCore::processEvent(LuaEventList &events)
{
    while(!events.empty())
    {
        auto event = events.front();
        events.pop_front();
        processEvent(event);
    }
}

bool LuaCore::queueManualRun(const QString& code)
{
    RsStackMutex lock(_mutex);
    if(_shutDownImminent || _manualPending || !_ready) return false;
    _manualCode = code;
    _manualPending = true;
    return true;
}

bool LuaCore::processEventQueue()
{
    std::unique_ptr<LuaEvent> event;
    QString manualCode;
    bool manual = false;
    {
        RsStackMutex lock(_mutex);
        if(_shutDownImminent || !_ready) return false;
        if(_manualPending)
        {
            manual = true;
            manualCode.swap(_manualCode);
            _manualPending = false;
        }
        else if(_events.size())
        {
            event = _events.pop();
        }
        else return false;
    }
    if(manual) emit manualRunFinished(runLuaByString(manualCode));
    else dispatchEvent(*event);
    return true;
}

void LuaCore::dispatchEvent(const LuaEvent& event)
{
    // Snapshot under the same lock used by the editor; never retain raw script
    // pointers while Lua runs or while the user can save/delete a script.
    std::vector<std::pair<QString, QString>> scripts;
    {
        std::lock_guard<std::recursive_mutex> lock(_scriptsMutex);
        for(auto container : *_luaList)
            if(container->isTriggered(event))
                scripts.emplace_back(container->getName(), container->getCode());
    }
    for(const auto& script : scripts)
    {
        if(_cancel) break;
        emit appendLog(QObject::tr("running (by event): ") + script.first);
        runLuaByEvent(script.second, event);
    }
}

bool LuaCore::executeLocked(const QString& code)
{
    if(!L || !_ready || _cancel) return false;
    const QByteArray bytes = code.toUtf8();
    LuaExecutionGuard guard(L, _cancel);
    int result = luaL_loadbuffer(L, bytes.constData(), bytes.size(), "script");
    if(result == LUA_OK) result = lua_pcall(L, 0, 0, 0);
    reportLuaErrors(L, result);
    return result == LUA_OK;
}

bool LuaCore::runLuaByString(const QString& code)
{
    RsStackMutex lock(_mutexRunLua);
    return executeLocked(code);
}

bool LuaCore::configureJsonApi(const QString& url, const QString& username,
                             const QString& password, QString& error)
{
    if(!_mutexRunLua.trylock()) { error = "A script is running. Try again when it finishes."; return false; }
    std::lock_guard<RsMutex> lock(_mutexRunLua, std::adopt_lock);
    if(!L || _shutDownImminent) { error = "Lua is shutting down"; return false; }
    LuaExecutionGuard guard(L, _cancel);
    const int top = lua_gettop(L);
    lua_getglobal(L, "jsonapi");
    if(!lua_istable(L, -1))
    { lua_settop(L, top); error = QObject::tr("The jsonapi table was replaced by a script. Restart the plugin to restore it."); return false; }
    lua_getfield(L, -1, "configure");
    lua_newtable(L);
    const auto field = [this](const char* key, const QString& value) {
        const QByteArray bytes = value.toUtf8();
        lua_pushlstring(L, bytes.constData(), bytes.size()); lua_setfield(L, -2, key);
    };
    field("url", url); field("username", username); field("password", password);
    lua_pushinteger(L, 10000); lua_setfield(L, -2, "timeout_ms");
    const int result = lua_pcall(L, 1, 2, 0);
    const bool success = result == LUA_OK && lua_toboolean(L, -2);
    if(!success) error = QObject::tr("Could not apply JSON API settings. Restore jsonapi.configure if your script changed it.");
    lua_settop(L, top);
    return success;
}

void LuaCore::runLuaByName(const QString& name)
{
    QString code;
    {
        std::lock_guard<std::recursive_mutex> lock(_scriptsMutex);
        LuaContainer* container = nullptr;
        if(!_luaList->itemByName(name, container)) return;
        code = container->getCode();
    }
    runLuaByString(code);
}

void addSettingsEntryToLua(lua_State *L, int top, const QString &type, const QVariant& value, const QString &name)
{
//    std::cout << "[Lua] runByEvent/addSettingsEntry adding " << name.toStdString() << " with " << value.toString().toStdString() << std::endl;

    const QByteArray key = name.toUtf8();
    lua_pushlstring(L, key.constData(), key.size());
    if(type == "str") {
        const QByteArray text = value.toString().toUtf8();
        lua_pushlstring(L, text.constData(), text.size());
    }
    else if(type == "int")
        lua_pushinteger(L, value.toInt());
    else if(type == "u32")
        lua_pushinteger(L, value.toUInt());
    else if(type == "u64")
        lua_pushinteger(L, value.toULongLong());

    lua_settable(L, top);
}

void addSettingsToLua(lua_State  *L, int top, LuaEventParameters *settings, int8_t skipLvl)
{
    QStringList keys = settings->allKeys();
    for(QStringList::ConstIterator it = keys.begin(); it != keys.end(); it++)
    {
        QString key = *it;

        /*
        while (skipLvl-- && key.contains('/'))
            key = key.mid(key.indexOf('/') + 1);
        */
	(void) skipLvl;

        QString type = key.mid(0, 3);
//        std::cout << "[Lua] runByEvent/addSettingsToLua adding type " << type.toStdString() << " key is " << key.toStdString() << " (was " << it->toStdString() << ")" << std::endl;
        if(!(type == "str" || type == "int"  || type == "u32" || type == "u64" /*|| type == "arr" */))
            continue;

        QString name = key.mid(3);
        QVariant value = settings->value(key);

        /*
        if (type == "arr") {
            lua_pushfstring(L, name.toStdString().c_str());

            // add array table
            lua_newtable(L);
            int top2 = lua_gettop(L);
            int index, num = settings->beginReadArray(key);

            for (index = 0; index < num; ++index) {
                // add array index
                lua_pushinteger(L, index);
                settings->setArrayIndex(index);

                // add value table
                lua_newtable(L);
                int top3 = lua_gettop(L);

                // values
                addSettingsToLua(L, top3, settings, 2);

                // end array table
                lua_settable(L, top2);
            }

            // end args table
            lua_settable(L, top);
            settings->endArray();
        }
        else
        */
            addSettingsEntryToLua(L, top, type, value, name);
    }
}

void LuaCore::runLuaByEvent(const QString& code, const LuaEvent& event)
{
    RsStackMutex lock(_mutexRunLua);
    if(!L || _cancel) return;
    lua_newtable(L);
    addSettingsToLua(L, lua_gettop(L), event.dataParm, 0);
    lua_setglobal(L, "args");
    executeLocked(code);
}

void LuaCore::reportLuaErrors(lua_State *L, int status)
{
    std::string s;
    if ( status!=0 ) {
        s = LuaExecutionGuard::errorText(L);
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
    if(_ui) QObject::disconnect(this, nullptr, _ui, nullptr);
    _ui = ui;
    if(ui) _ready = true;
    if(!ui) return;

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
void LuaCore::emitAppendOutput(const QString &s)
{
    // Rate-limit signals across runs, not just document size after delivery.
    const auto now = std::chrono::steady_clock::now();
    if(now - _outputWindow >= std::chrono::seconds(1))
    {
        _outputWindow = now;
        _outputRemaining = 64 * 1024;
        _outputMessages = 200;
    }
    if(!_outputRemaining || !_outputMessages) return;
    --_outputMessages;
    const int length = qMin<int>(s.size(), static_cast<int>(_outputRemaining));
    _outputRemaining -= qMin<size_t>(_outputRemaining, size_t(length) + 1);
    emit appendOutput(s.left(length));
}
void LuaCore::emitClearOutput()                     { emit clearOutput(); }
