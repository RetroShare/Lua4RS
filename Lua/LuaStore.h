#ifndef LUA_STORE_H
#define LUA_STORE_H

#include <mutex>
#include <string>

#include <QJsonObject>
#include <QString>

struct lua_State;

// Small persistent key-value store per script (Lua: store.get/set/keys).
// Values are anything JSON can hold. One JSON file per script in
// <scripts>/.store/<name>.json, written once after a run if it changed.
// NOT encrypted: anyone with access to the folder can read it.
//
// Allowed at every access level: it only holds the script's own data and
// changes nothing in RetroShare. Approvals do not depend on it.
class LuaStore
{
public:
    enum class Mode
    {
        Normal,        // real store, written after the run
        DiscardWrites, // test runs with simulated args: real data, changes dropped
        Ephemeral      // ad-hoc agent code: starts empty, never written
    };

    static const int kMaxBytes = 256 * 1024; // serialized size per script
    static const int kMaxKeyLength = 256;

    void setDirectory(const QString& dir);

    // Lua worker, around one run.
    void begin(const QString& script, Mode mode);
    void end();

    // Lua: store.get(key), store.set(key, value|nil), store.keys()
    static void install(lua_State* L, LuaStore* store);
    static int luaGet(lua_State* L);
    static int luaSet(lua_State* L);
    static int luaKeys(lua_State* L);

    // Any thread (GUI, MCP): follow renames and deletes, read for read_store.
    bool rename(const QString& oldName, const QString& newName);
    void remove(const QString& name);
    bool read(const QString& name, QJsonObject& data, QString& error) const;

    QString path(const QString& name) const;

private:
    static LuaStore* from(lua_State* L);
    bool loadLocked(std::string& error);  // current script, lazily
    static bool readFile(const QString& path, QJsonObject& data, QString& error);

    mutable std::mutex _mutex;
    QString _dir;
    QString _script;
    Mode _mode = Mode::Normal;
    bool _active = false;
    bool _loaded = false;
    bool _dirty = false;
    QJsonObject _data;
};

#endif // LUA_STORE_H
