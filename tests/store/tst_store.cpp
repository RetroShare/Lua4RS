// Tests for the per-script store (store.get/set/keys), no RetroShare needed.
// Build: qmake store.pro && make && ./tst_store
#include <cstdio>
#include <cstring>

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QTemporaryDir>

#include <lua.hpp>

#include "LuaStore.h"

namespace {
int failures = 0;
int checks = 0;

void check(bool condition, const char* what, int line)
{
    ++checks;
    if(!condition) { ++failures; std::printf("FAIL  line %d: %s\n", line, what); }
}
#define CHECK(cond) check((cond), #cond, __LINE__)

lua_State* newState(LuaStore& store)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    LuaStore::install(L, &store);
    lua_newtable(L);
    lua_pushcfunction(L, LuaStore::luaGet);  lua_setfield(L, -2, "get");
    lua_pushcfunction(L, LuaStore::luaSet);  lua_setfield(L, -2, "set");
    lua_pushcfunction(L, LuaStore::luaKeys); lua_setfield(L, -2, "keys");
    lua_setglobal(L, "store");
    return L;
}

// Runs code as one script run; returns "" on success, else the error text.
// The code's return value (if any) is stored in `result`.
QString run(lua_State* L, LuaStore& store, const char* script, LuaStore::Mode mode,
            const char* code, QString* result = nullptr)
{
    store.begin(script, mode);
    QString error;
    if(luaL_loadstring(L, code) != LUA_OK || lua_pcall(L, 0, 1, 0) != LUA_OK)
        error = QString::fromUtf8(lua_tostring(L, -1));
    else if(result) *result = QString::fromUtf8(luaL_tolstring(L, -1, nullptr)), lua_pop(L, 1);
    lua_pop(L, 1);
    store.end();
    return error;
}
} // namespace

int main()
{
    QTemporaryDir dir;
    LuaStore store;
    store.setDirectory(dir.filePath(".store"));
    lua_State* L = newState(store);
    QString out;
    const auto N = LuaStore::Mode::Normal;

    // values survive runs; tables, numbers, booleans; whole numbers stay integers
    CHECK(run(L, store, "a.lua", N, "store.set('n', 5) store.set('t', {x = 1, list = {1, 2}}) store.set('b', true)").isEmpty());
    CHECK(QFile::exists(store.path("a.lua")));
    CHECK(run(L, store, "a.lua", N, "return math.type(store.get('n')) .. ' ' .. store.get('t').x .. ' ' .. #store.get('t').list .. ' ' .. tostring(store.get('b'))", &out).isEmpty());
    CHECK(out == "integer 1 2 true");
    CHECK(run(L, store, "a.lua", N, "return 'Ticket ' .. store.get('n')", &out).isEmpty() && out == "Ticket 5");
    CHECK(run(L, store, "a.lua", N, "return table.concat(store.keys(), ',')", &out).isEmpty() && out == "b,n,t");
    // per script
    CHECK(run(L, store, "b.lua", N, "return tostring(store.get('n'))", &out).isEmpty() && out == "nil");
    // nil deletes
    CHECK(run(L, store, "a.lua", N, "store.set('b', nil) return #store.keys()", &out).isEmpty() && out == "2");

    // a failing run still keeps what it stored before the error
    CHECK(!run(L, store, "a.lua", N, "store.set('n', 6) error('boom')").isEmpty());
    CHECK(run(L, store, "a.lua", N, "return store.get('n')", &out).isEmpty() && out == "6");

    // test runs see real data, changes are dropped
    CHECK(run(L, store, "a.lua", LuaStore::Mode::DiscardWrites, "store.set('n', 99) return store.get('n')", &out).isEmpty() && out == "99");
    CHECK(run(L, store, "a.lua", N, "return store.get('n')", &out).isEmpty() && out == "6");
    // ad-hoc code: empty, never written
    CHECK(run(L, store, "a.lua", LuaStore::Mode::Ephemeral, "local before = store.get('n') store.set('n', 1) return tostring(before)", &out).isEmpty() && out == "nil");
    CHECK(run(L, store, "a.lua", N, "return store.get('n')", &out).isEmpty() && out == "6");
    CHECK(!QFile::exists(store.path("(agent)")));

    // limits and bad values
    CHECK(run(L, store, "a.lua", N, "store.set('', 1)").contains("1 to 256"));
    CHECK(run(L, store, "a.lua", N, "store.set(string.rep('k', 257), 1)").contains("1 to 256"));
    CHECK(run(L, store, "a.lua", N, "store.set('f', function() end)").contains("cannot be stored"));
    CHECK(run(L, store, "a.lua", N, "local t = {} t.self = t store.set('c', t)").contains("cannot be stored"));
    CHECK(run(L, store, "a.lua", N, "store.set('big', string.rep('x', 300 * 1024))").contains("store is full"));
    CHECK(run(L, store, "a.lua", N, "return tostring(store.get('big'))", &out).isEmpty() && out == "nil");

    // outside a run
    CHECK(luaL_dostring(L, "store.get('n')") != LUA_OK);
    CHECK(QString(lua_tostring(L, -1)).contains("only available while a script runs"));
    lua_pop(L, 1);

    // read (read_store), rename, remove
    QJsonObject data;
    QString error;
    CHECK(store.read("a.lua", data, error) && data.value("n").toInt() == 6);
    CHECK(store.read("missing.lua", data, error) && data.isEmpty());
    CHECK(store.rename("a.lua", "c.lua"));
    CHECK(!QFile::exists(store.path("a.lua")) && store.read("c.lua", data, error) && data.value("n").toInt() == 6);
    CHECK(store.rename("nothing.lua", "x.lua")); // no store: nothing to do
    store.remove("c.lua");
    CHECK(!QFile::exists(store.path("c.lua")));

    // a damaged file is reported, not silently replaced
    {
        QDir().mkpath(dir.filePath(".store"));
        QFile f(store.path("d.lua"));
        CHECK(f.open(QIODevice::WriteOnly));
        f.write("{ not json");
    }
    CHECK(run(L, store, "d.lua", N, "store.set('x', 1)").contains("damaged"));
    {
        QFile f(store.path("d.lua"));
        CHECK(f.open(QIODevice::ReadOnly) && f.readAll() == "{ not json");
    }
    CHECK(!store.read("d.lua", data, error) && error.contains("damaged"));

    lua_close(L);
    std::printf("%d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}
