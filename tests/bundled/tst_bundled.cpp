// Tests for the scripts compiled into the plugin (scripts/scripts.qrc).
// Build: qmake bundled.pro && make && ./tst_bundled
#include <cstdio>

#include <QDir>
#include <QSet>
#include <QTemporaryDir>

#include <lua.hpp>

#include "BundledScripts.h"

namespace {
int failures = 0;
int checks = 0;

void check(bool condition, const QString& what, int line)
{
    ++checks;
    if(!condition) { ++failures; std::printf("FAIL  line %d: %s\n", line, qPrintable(what)); }
}
#define CHECK(cond) check((cond), #cond, __LINE__)
#define CHECK_FOR(cond, name) check((cond), QString(#cond " (%1)").arg(name), __LINE__)

void allScripts()
{
    QSet<QString> seen;
    for(const BundledScripts::Group& group : BundledScripts::groups())
    {
        const QStringList names = BundledScripts::names(group.folder);
        CHECK_FOR(!names.isEmpty(), group.folder);
        for(const QString& name : names)
        {
            CHECK_FOR(!seen.contains(name), name); // names must be unique across groups
            seen.insert(name);
            QByteArray code;
            CHECK_FOR(BundledScripts::read(name, code), name);
            CHECK_FOR(BundledScripts::resourcePath(name).contains(QLatin1String(group.folder)), name);
            CHECK_FOR(code.size() <= 64 * 1024, name); // same limit as for agent scripts
            CHECK_FOR(!BundledScripts::summary(code).isEmpty(), name); // menu tooltip
            lua_State* L = luaL_newstate();
            const QByteArray chunk = "=" + name.toUtf8();
            const int status = luaL_loadbuffer(L, code.constData(), size_t(code.size()), chunk.constData());
            check(status == LUA_OK, name + ": " + (status == LUA_OK ? "" : lua_tostring(L, -1)), __LINE__);
            lua_close(L);
        }
    }
    { QByteArray none; CHECK(!BundledScripts::read("nonexistent.lua", none)); }
    CHECK(BundledScripts::resourcePath("../examples/chatBot.lua").isEmpty());
}

void summary()
{
    CHECK(BundledScripts::summary("-- A: b\n-- c.\n--\n-- later\n") == "A: b c.");
    CHECK(BundledScripts::summary("local x = 1\n") == "");
}

void installOnlyExamples()
{
    QTemporaryDir dir;
    QString error;
    CHECK(BundledScripts::install(dir.path(), error));
    const QStringList installed = QDir(dir.path()).entryList({"*.lua"}, QDir::Files, QDir::Name);
    CHECK(installed == BundledScripts::names("examples"));
    // Once: a deleted example stays deleted.
    CHECK(QFile::remove(QDir(dir.path()).filePath("chatBot.lua")));
    CHECK(BundledScripts::install(dir.path(), error));
    CHECK(!QFileInfo::exists(QDir(dir.path()).filePath("chatBot.lua")));
}
}

int main()
{
    allScripts();
    summary();
    installOnlyExamples();
    std::printf("%d checks, %d failures (%s)\n", checks, failures, LUA_VERSION);
    return failures ? 1 : 0;
}
