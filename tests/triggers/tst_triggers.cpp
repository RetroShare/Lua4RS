// Tests for script triggers, no RetroShare needed.
// Build: qmake triggers.pro && make && ./tst_triggers
#include <cstdio>

#include <QDir>
#include <QFile>
#include <QTemporaryDir>

#include "LuaContainer.h"
#include "LuaList.h"
#include "LuaEvent.h"

namespace {
int failures = 0;
int checks = 0;

void check(bool condition, const char* what, int line)
{
    ++checks;
    if(!condition) { ++failures; std::printf("FAIL  line %d: %s\n", line, what); }
}
#define CHECK(cond) check((cond), #cond, __LINE__)

bool fire(LuaContainer& c, uint eventId, const QDateTime& at)
{
    LuaEvent event;
    event.eventId = eventId;
    event.timeStamp = at;
    return c.isTriggered(event);
}
bool tick(LuaContainer& c, const QDateTime& at) { return fire(c, L4R_TIMERTICK, at); }

LuaTriggers every(uint amount, uint unit)
{
    LuaTriggers t;
    t.every = true;
    t.everyAmount = amount;
    t.everyUnit = unit;
    return t;
}

const uint kHours = 2; // unit index: sec, min, hour, day, week
} // namespace

int main()
{
    const QDateTime now = QDateTime::currentDateTime();
    {
        LuaContainer c;
        c.setEnabled(true);

        // A new timer runs on the next tick, then waits for its interval.
        c.setTriggers(every(1, kHours));
        CHECK(!c.getTimerLastRun().isValid());
        CHECK(tick(c, now));
        CHECK(c.getTimerLastRun().isValid());
        CHECK(c.getLastRun().isValid());
        CHECK(!tick(c, now.addSecs(60)));

        // Saving replaces the triggers: the timer continues its schedule
        // instead of running again right away.
        c.setTriggers(every(1, kHours));
        CHECK(!tick(c, now.addSecs(120)));
        CHECK(tick(c, now.addSecs(3600 + 60)));

        // A changed interval still counts from the last run.
        c.setTriggers(every(2, kHours));
        CHECK(!tick(c, now.addSecs(3600 + 120)));

        // Without a timer there is no timer run to show.
        LuaTriggers none;
        c.setTriggers(none);
        CHECK(!c.getTimerLastRun().isValid());
        CHECK(!tick(c, now.addSecs(4 * 3600)));

        // A timer switched on again starts anew: it runs on the next tick,
        // although its old schedule (last run at +1h) is not due yet.
        LuaTriggers both = every(1, kHours);
        both.eventIds << 10010;
        c.setTriggers(both);
        CHECK(tick(c, now.addSecs(3600 + 240)));

        // Other triggers do not move the timer's schedule.
        const QDateTime timerRun = c.getTimerLastRun();
        CHECK(fire(c, 10010, now.addSecs(3600 + 600)));
        CHECK(!fire(c, 10011, now.addSecs(3600 + 600)));
        CHECK(c.getTimerLastRun() == timerRun);

        // Disabled scripts never run.
        c.setEnabled(false);
        CHECK(!fire(c, 10010, now));
    }

    // Startup, shutdown, once and the time window.
    {
        LuaContainer c;
        c.setEnabled(true);
        LuaTriggers t;
        t.startup = true;
        t.once = true;
        t.onceAt = now.addSecs(600);
        c.setTriggers(t);
        CHECK(fire(c, L4R_STARTUP, now));
        CHECK(!fire(c, L4R_SHUTDOWN, now));
        CHECK(!tick(c, now.addSecs(599)));
        CHECK(tick(c, now.addSecs(601)));  // a little late is fine
        CHECK(!tick(c, now.addSecs(602))); // but only once
        CHECK(!tick(c, now.addSecs(700)));

        // An event run just after the due time does not swallow the once run.
        LuaTriggers e;
        e.eventIds << 10010;
        e.once = true;
        e.onceAt = now.addSecs(1200);
        c.setTriggers(e);
        CHECK(fire(c, 10010, now.addSecs(1200)));
        CHECK(tick(c, now.addSecs(1201)));
        CHECK(!tick(c, now.addSecs(1202)));

        LuaTriggers w;
        w.startup = true;
        w.window = true;
        w.windowFrom = QTime(8, 0);
        w.windowTo = QTime(18, 0);
        c.setTriggers(w);
        CHECK(fire(c, L4R_STARTUP, QDateTime(now.date(), QTime(12, 0))));
        CHECK(!fire(c, L4R_STARTUP, QDateTime(now.date(), QTime(19, 0))));
    }

    // .ini files of earlier versions load and are written back the same way.
    {
        QTemporaryDir dir;
        QSettings old(SRCDIR "/old-format.ini", QSettings::IniFormat);
        LuaContainer c;
        c.fromSettings(old);
        const LuaTriggers& t = c.triggers();
        CHECK(c.getEnabled());
        CHECK(t.every && t.everyAmount == 1 && t.everyUnit == kHours);
        CHECK(t.startup && !t.shutdown && !t.once && !t.window);
        CHECK(t.eventIds == QList<uint>{ 10010 });
        CHECK(c.getLastRun().isValid());
        CHECK(c.getTimerLastRun().isValid());
        CHECK(c.getTimerLastRun() < c.getLastRun());

        QSettings written(dir.filePath("written.ini"), QSettings::IniFormat);
        c.toSettings(written);
        written.sync();
        // Every key of the old file is written with the same value, except the
        // LastRun of startup, shutdown and event triggers: they now hold the
        // script's last run (only timer and once keep their own).
        for(const QString& key : old.allKeys())
        {
            const QString className = old.value(key.section('/', 0, 0) + "/Classname").toString();
            if(key.endsWith("/LastRun") && className != "LuaTriggerTimerInterval" && className != "LuaTriggerTimerOnce")
                continue;
            if(written.value(key) != old.value(key))
            {
                ++failures;
                std::printf("FAIL  key %s differs\n", qPrintable(key));
            }
            ++checks;
        }
        CHECK(written.allKeys().size() == old.allKeys().size());
    }

    // The last run survives a restart without a clean shutdown: the settings
    // are saved right after the run (LuaList::saveSettings), not only by saveAll.
    {
        QTemporaryDir dir;
        LuaList before;
        before.setFilePath(dir.path().toStdString() + "/");
        LuaContainer* t = before.createItem();
        t->setName(QString("timer.lua"));
        t->setCode(QString("print(1)"));
        t->setEnabled(true);
        t->setTriggers(every(1, kHours));
        CHECK(before.addItemAndSave(t));
        CHECK(tick(*t, now));
        CHECK(before.saveSettings(t));

        LuaList after; // what a restart loads
        after.setFilePath(dir.path().toStdString() + "/");
        CHECK(after.loadAll());
        LuaContainer* loaded = nullptr;
        CHECK(after.itemByName(QString("timer.lua"), loaded));
        if(loaded)
        {
            CHECK(!tick(*loaded, now.addSecs(60)));
            CHECK(tick(*loaded, now.addSecs(3600 + 60)));
            CHECK(loaded->getCode() == "print(1)");
        }
    }

    // Script files: UTF-8 on every platform, written back unchanged; deleting
    // a script also deletes its settings.
    {
        QTemporaryDir dir;
        const std::string path = dir.path().toStdString() + "/";
        auto writeRaw = [&](const char* name, const QByteArray& data) {
            QFile f(dir.filePath(name));
            return f.open(QIODevice::WriteOnly) && f.write(data) == data.size();
        };
        const QString umlauts = QString::fromUtf8("print(\"Grüße, Ärger 🙂\")\n");
        CHECK(writeRaw("utf8.lua", umlauts.toUtf8()));
        CHECK(writeRaw("bom.lua", QByteArray("\xEF\xBB\xBF") + "print(1)"));
        CHECK(writeRaw("crlf.lua", "a = 1\r\nb = 2\r\n"));
        CHECK(writeRaw("noeol.lua", "return 1"));

        LuaList list;
        list.setFilePath(path);
        CHECK(list.loadAll()); // no .ini files: loads anyway
        LuaContainer* c = nullptr;
        CHECK(list.itemByName(QString("utf8.lua"), c) && c->getCode() == umlauts);
        CHECK(list.itemByName(QString("bom.lua"), c) && c->getCode() == "print(1)");
#ifdef _WIN32
        CHECK(list.itemByName(QString("crlf.lua"), c) && c->getCode() == "a = 1\nb = 2\n");
#else
        // Byte for byte: the approval hash covers the code in memory.
        CHECK(list.itemByName(QString("crlf.lua"), c) && c->getCode() == "a = 1\r\nb = 2\r\n");
#endif
        CHECK(list.itemByName(QString("noeol.lua"), c) && c->getCode() == "return 1");

        CHECK(list.saveAll());
        QFile saved(dir.filePath("utf8.lua"));
        CHECK(saved.open(QIODevice::ReadOnly) && saved.readAll() == LuaList::encodeScript(umlauts));
        QFile crlf(dir.filePath("crlf.lua"));
        // Windows: written back as it was read (LF in memory, CRLF on disk).
        CHECK(crlf.open(QIODevice::ReadOnly) && crlf.readAll() == "a = 1\r\nb = 2\r\n");
        CHECK(QFile::exists(dir.filePath("utf8.lua.ini")));

        CHECK(list.itemByName(QString("utf8.lua"), c) && list.removeItemAndDelete(c));
        CHECK(!QFile::exists(dir.filePath("utf8.lua")));
        CHECK(!QFile::exists(dir.filePath("utf8.lua.ini")));
    }

    // Script files <-> code, for both line-ending modes (crlf = Windows).
    {
        const QString tricky = QString::fromUtf8("a\rb\r\nc\r\r\nd = [[x\ry]]\n\u00e4\n");
        for(bool crlf : { false, true })
        {
            // Any code comes back unchanged: the approval hash covers it.
            CHECK(LuaList::decodeScript(LuaList::encodeScript(tricky, crlf), crlf) == tricky);
            CHECK(LuaList::decodeScript(LuaList::encodeScript("x\n", crlf), crlf) == "x\n");
        }
        CHECK(LuaList::encodeScript("a\nb", true) == "a\r\nb");
        CHECK(LuaList::encodeScript("a\r\nb", true) == "a\r\r\nb");
        CHECK(LuaList::decodeScript("a\r\nb\rc", true) == "a\nb\rc"); // a lone CR stays
        CHECK(LuaList::decodeScript("a\r\nb", false) == "a\r\nb");
        CHECK(LuaList::decodeScript("\xEF\xBB\xBFx", false) == "x");
        // Qt 5 stopped at the first NUL in some conversions.
        CHECK(LuaList::decodeScript(QByteArray("a\0b", 3), false).size() == 3);
        CHECK(LuaList::decodeScript(QByteArray("\xff\0b", 3), false).size() == 3);
        // Not UTF-8: Latin-1 here (every byte kept), the ANSI code page on
        // Windows. A file that ends inside a UTF-8 sequence is not UTF-8 either.
#ifndef _WIN32
        CHECK(LuaList::decodeScript("-- \xE4", false) == QString::fromUtf8("-- \u00e4"));
        CHECK(LuaList::decodeScript("x = \"\xE4\"\n", false) == QString::fromUtf8("x = \"\u00e4\"\n"));
#else
        CHECK(LuaList::decodeScript("-- \xE4", false).size() == 4);
#endif
    }

    // Names: valid on every platform, compared ignoring case.
    {
        for(const char* ok : { "a.lua", "my-script.lua", "Gr\xC3\xBC\xC3\x9F" "e.lua", "con2.lua", "console.lua", "nul_x.lua" })
            CHECK(LuaList::isValidName(QString::fromUtf8(ok)));
        for(const char* bad : { ".lua", "a.txt", "a.LUA", "a/b.lua", "a\\b.lua", "c:a.lua", "a..lua", "a?.lua", "a*.lua",
                                "a\".lua", "a<.lua", "a|.lua", " a.lua", "a.lua ", "SYNCFR~1.lua", "con.lua", "NUL.lua",
                                "Com1.lua", "lpt9.lua", "aux.backup.lua", "prn .lua", "a\x01" "b.lua", "a\x7f.lua" })
            CHECK(!LuaList::isValidName(QString::fromUtf8(bad)));
        CHECK(!LuaList::isValidName(QString::fromUtf8("com\xC2\xB9.lua")));
        CHECK(!LuaList::isValidName(QString(1, QChar(0)) + ".lua"));
        CHECK(LuaList::isValidName(QString(LuaList::kMaxNameLength - 4, 'a') + ".lua"));
        CHECK(!LuaList::isValidName(QString(LuaList::kMaxNameLength - 3, 'a') + ".lua"));

        QTemporaryDir dir;
        LuaList list;
        list.setFilePath(dir.path().toStdString() + "/");
        auto add = [&](const char* name) {
            auto c = new LuaContainer();
            c->setName(name); c->setCode("return 1\n");
            list.addItem(c);
            CHECK(list.save(c));
            return c;
        };
        LuaContainer* foo = add("foo.lua");
        CHECK(list.nameConflict("bar.lua").isEmpty());
        CHECK(list.nameConflict("foo.lua") == "foo.lua");
        CHECK(list.nameConflict("FOO.lua") == "foo.lua");
        CHECK(list.nameConflict("Foo.lua", foo).isEmpty()); // its own name, other case
        CHECK(list.nameConflict("foo.lua", foo).isEmpty());
        LuaContainer* bar = add("bar.lua");
        CHECK(list.nameConflict("Foo.lua", bar) == "foo.lua");
        // A file that is not in the list (not loaded, or another spelling).
        { QFile f(dir.filePath("stray.lua")); CHECK(f.open(QIODevice::WriteOnly)); }
        CHECK(list.nameConflict("stray.lua") == "stray.lua");

        // Renaming only the case keeps the file and its settings.
        foo->setName("Foo.lua");
        CHECK(list.rename("foo.lua", "Foo.lua"));
        CHECK(list.save(foo));
        const QStringList files = QDir(dir.path()).entryList({ "*.lua", "*.ini" }, QDir::Files);
        CHECK(files.contains("Foo.lua") && files.contains("Foo.lua.ini"));
        CHECK(!files.contains("foo.lua") && !files.contains("foo.lua.ini"));
        LuaList reloaded;
        reloaded.setFilePath(dir.path().toStdString() + "/");
        CHECK(reloaded.loadAll());
        LuaContainer* c = nullptr;
        CHECK(reloaded.itemByName(QString("Foo.lua"), c) && c->getCode() == "return 1\n");

        // discard takes back a new script with its files.
        LuaContainer* gone = add("gone.lua");
        list.discard(gone);
        CHECK(!list.itemByName(QString("gone.lua"), c));
        CHECK(!QFile::exists(dir.filePath("gone.lua")) && !QFile::exists(dir.filePath("gone.lua.ini")));

        // A file that is already gone does not block deleting the script.
        LuaContainer* missing = add("missing.lua");
        CHECK(QFile::remove(dir.filePath("missing.lua")));
        CHECK(list.removeItemAndDelete(missing));
        CHECK(!list.itemByName(QString("missing.lua"), c));
        CHECK(!QFile::exists(dir.filePath("missing.lua.ini")));

        // A file that cannot be deleted keeps the script in the list.
        LuaContainer* locked = add("locked.lua");
        const QFile::Permissions permissions = QFile::permissions(dir.path());
        QFile::setPermissions(dir.path(), QFile::ReadOwner | QFile::ExeOwner);
        if(!QFile::remove(dir.filePath("locked.lua"))) // not as root
        {
            CHECK(!list.removeItemAndDelete(locked));
            CHECK(list.itemByName(QString("locked.lua"), c) && c == locked);
        }
        QFile::setPermissions(dir.path(), permissions);
    }

    // .ini text written by Qt 6 (UTF-8) and by Qt 5 (escapes) reads the same
    // with either Qt version.
    {
        QTemporaryDir dir;
        const QString desc = QString::fromUtf8("Gr\u00fc\u00dfe \u20ac \u65e5 \U0001F600");
        auto writeRaw = [&](const QString& name, const QByteArray& data) {
            QFile f(dir.filePath(name));
            return f.open(QIODevice::WriteOnly) && f.write(data) == data.size();
        };
        CHECK(writeRaw("qt6.lua", "return 1\n"));
        CHECK(writeRaw("qt6.lua.ini", "[General]\nDescription=" + desc.toUtf8() + "\n"));
        CHECK(writeRaw("qt5.lua", "return 1\n"));
        CHECK(writeRaw("qt5.lua.ini", "[General]\nDescription=Gr\\xfc\\xdf\\x65 \\x20ac \\x65e5 \\xd83d\\xde00\n"));
        LuaList list;
        list.setFilePath(dir.path().toStdString() + "/");
        CHECK(list.loadAll());
        LuaContainer* c = nullptr;
        CHECK(list.itemByName(QString("qt6.lua"), c) && c->getDesc() == desc);
        CHECK(list.itemByName(QString("qt5.lua"), c) && c->getDesc() == desc);
        // And what this build writes reads back the same.
        CHECK(list.saveAll());
        LuaList again;
        again.setFilePath(dir.path().toStdString() + "/");
        CHECK(again.loadAll());
        CHECK(again.itemByName(QString("qt6.lua"), c) && c->getDesc() == desc);
    }

    std::printf("%d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}
