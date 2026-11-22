// Tests for script triggers, no RetroShare needed.
// Build: qmake triggers.pro && make && ./tst_triggers
#include <cstdio>

#include "LuaContainer.h"
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

bool tick(LuaContainer& c, const QDateTime& at)
{
    LuaEvent event;
    event.eventId = L4R_TIMERTICK;
    event.timeStamp = at;
    return c.isTriggered(event);
}

const uint kHours = 2; // unit index: sec, min, hour, day, week
} // namespace

int main()
{
    LuaContainer c;
    c.setEnabled(true);
    const QDateTime now = QDateTime::currentDateTime();

    // A new timer runs on the next tick, then waits for its interval.
    c.addRunEveryTrigger(1, kHours);
    CHECK(tick(c, now));
    CHECK(!tick(c, now.addSecs(60)));

    // Saving rebuilds all triggers: the timer continues its schedule instead
    // of running again right away.
    c.removeAllTrigger();
    c.addRunEveryTrigger(1, kHours);
    CHECK(!tick(c, now.addSecs(120)));
    CHECK(tick(c, now.addSecs(3600 + 60)));

    // A changed interval still counts from the last run.
    c.removeAllTrigger();
    c.addRunEveryTrigger(2, kHours);
    CHECK(!tick(c, now.addSecs(3600 + 120)));

    // A timer that did not exist before the save runs right away.
    c.removeAllTrigger();
    c.addRunStratupTrigger();
    c.removeAllTrigger();
    c.addRunEveryTrigger(1, kHours);
    CHECK(tick(c, now.addSecs(3600 + 180)));

    std::printf("%d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}
