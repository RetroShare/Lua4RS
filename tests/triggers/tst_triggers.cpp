// Tests for script triggers, no RetroShare needed.
// Build: qmake triggers.pro && make && ./tst_triggers
#include <cstdio>

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

int main(int, char** argv)
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

        // Other triggers do not move the timer's schedule.
        LuaTriggers both = every(1, kHours);
        both.eventIds << 10010;
        c.setTriggers(both);
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
        const QString source = QString::fromLocal8Bit(argv[0]).section('/', 0, -2) + "/old-format.ini";
        QString fixture = QFile::exists(source) ? source : QString(SRCDIR "/old-format.ini");
        QSettings old(fixture, QSettings::IniFormat);
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
        // LastRun of non-timer triggers: they now share the script's last run.
        for(const QString& key : old.allKeys())
        {
            if(key.endsWith("/LastRun") && old.value(key.section('/', 0, 0) + "/Classname") != "LuaTriggerTimerInterval")
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

    std::printf("%d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}
