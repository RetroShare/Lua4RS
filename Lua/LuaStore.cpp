#include "LuaStore.h"
#include "LuaJsonApi.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSaveFile>
#include <QStringList>

#include <lua.hpp>

namespace {
char registryKey;

// Lua errors longjmp over C++ frames: build the message in a scope that has
// ended (all destructors run) before raising it.
int raise(lua_State* L) { return lua_error(L); }
}

void LuaStore::setDirectory(const QString& dir)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _dir = dir;
}

QString LuaStore::path(const QString& name) const
{
    return QDir(_dir).filePath(name + ".json");
}

void LuaStore::begin(const QString& script, Mode mode)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _script = script;
    _mode = mode;
    _active = true;
    _loaded = false;
    _dirty = false;
    _data = QJsonObject();
}

void LuaStore::end()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(_active && _dirty && _mode == Mode::Normal && !_dir.isEmpty())
    {
        QDir().mkpath(_dir);
        QSaveFile file(path(_script));
        const QByteArray bytes = QJsonDocument(_data).toJson(QJsonDocument::Indented);
        if(file.open(QIODevice::WriteOnly) && file.write(bytes) == bytes.size()) file.commit();
    }
    _active = false;
    _data = QJsonObject();
}

bool LuaStore::readFile(const QString& path, QJsonObject& data, QString& error)
{
    QFile file(path);
    if(!file.exists()) { data = QJsonObject(); return true; }
    if(!file.open(QIODevice::ReadOnly)) { error = "cannot open " + path; return false; }
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if(parseError.error != QJsonParseError::NoError || !doc.isObject())
    { error = "store file is damaged (" + path + "): " + parseError.errorString(); return false; }
    data = doc.object();
    return true;
}

bool LuaStore::loadLocked(std::string& error)
{
    if(!_active) { error = "store is only available while a script runs"; return false; }
    if(_loaded) return true;
    if(_mode != Mode::Ephemeral && !_dir.isEmpty())
    {
        QString message;
        if(!readFile(path(_script), _data, message)) { error = message.toStdString(); return false; }
    }
    _loaded = true;
    return true;
}

bool LuaStore::rename(const QString& oldName, const QString& newName)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(_dir.isEmpty() || !QFile::exists(path(oldName))) return true;
    QFile::remove(path(newName));
    return QFile::rename(path(oldName), path(newName));
}

void LuaStore::remove(const QString& name)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(!_dir.isEmpty()) QFile::remove(path(name));
}

bool LuaStore::read(const QString& name, QJsonObject& data, QString& error) const
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(_dir.isEmpty()) { data = QJsonObject(); return true; }
    return readFile(path(name), data, error);
}

void LuaStore::install(lua_State* L, LuaStore* store)
{
    lua_pushlightuserdata(L, store);
    lua_rawsetp(L, LUA_REGISTRYINDEX, &registryKey);
}

LuaStore* LuaStore::from(lua_State* L)
{
    lua_rawgetp(L, LUA_REGISTRYINDEX, &registryKey);
    auto store = static_cast<LuaStore*>(lua_touserdata(L, -1));
    lua_pop(L, 1);
    return store;
}

int LuaStore::luaGet(lua_State* L)
{
    LuaStore* store = from(L);
    const char* key = luaL_checkstring(L, 1);
    bool ok = true;
    {
        std::string error;
        QJsonValue value;
        if(!store) error = "store is not available";
        else
        {
            std::lock_guard<std::mutex> lock(store->_mutex);
            if(store->loadLocked(error)) value = store->_data.value(QString::fromUtf8(key));
        }
        if(!error.empty()) { ok = false; lua_pushlstring(L, error.data(), error.size()); }
        else if(value.isUndefined()) lua_pushnil(L);
        else
        {
            QString convertError;
            if(!pushLuaJson(L, value, convertError)) lua_pushnil(L);
        }
    }
    if(!ok) return raise(L);
    return 1;
}

int LuaStore::luaSet(lua_State* L)
{
    LuaStore* store = from(L);
    size_t keyLength = 0;
    const char* key = luaL_checklstring(L, 1, &keyLength);
    lua_settop(L, 2);
    bool ok = true;
    {
        std::string error;
        QJsonValue value;
        QString convertError;
        const bool remove = lua_isnil(L, 2);
        if(!store) error = "store is not available";
        else if(keyLength == 0 || keyLength > size_t(kMaxKeyLength))
            error = "store key must be 1 to 256 bytes";
        else if(!remove && !luaToJson(L, 2, value, convertError))
            error = "store.set: value cannot be stored: " + convertError.toStdString();
        else
        {
            std::lock_guard<std::mutex> lock(store->_mutex);
            if(store->loadLocked(error))
            {
                const QString k = QString::fromUtf8(key, int(keyLength));
                QJsonObject changed = store->_data;
                if(remove) changed.remove(k);
                else changed.insert(k, value);
                const int size = QJsonDocument(changed).toJson(QJsonDocument::Compact).size();
                if(size > kMaxBytes)
                    error = "store is full: " + std::to_string(size) + " bytes, at most " + std::to_string(kMaxBytes);
                else
                {
                    store->_data = changed;
                    store->_dirty = true;
                }
            }
        }
        if(!error.empty()) { ok = false; lua_pushlstring(L, error.data(), error.size()); }
    }
    if(!ok) return raise(L);
    return 0;
}

int LuaStore::luaKeys(lua_State* L)
{
    LuaStore* store = from(L);
    bool ok = true;
    {
        std::string error;
        QStringList keys;
        if(!store) error = "store is not available";
        else
        {
            std::lock_guard<std::mutex> lock(store->_mutex);
            if(store->loadLocked(error)) keys = store->_data.keys(); // sorted
        }
        if(!error.empty()) { ok = false; lua_pushlstring(L, error.data(), error.size()); }
        else
        {
            lua_createtable(L, keys.size(), 0);
            int i = 1;
            for(const QString& k : keys)
            {
                const QByteArray utf8 = k.toUtf8();
                lua_pushlstring(L, utf8.constData(), utf8.size());
                lua_rawseti(L, -2, i++);
            }
        }
    }
    if(!ok) return raise(L);
    return 1;
}
