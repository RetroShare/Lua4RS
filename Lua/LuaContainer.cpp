#include "LuaContainer.h"

#include <QObject>
#include <QStringList>

#include "util/rsdebuglevel0.h"

namespace {
// .ini keys, unchanged since the first Lua4RS versions
const char* kDesc = "Description";
const char* kEnabled = "Enabled";
const char* kLastTriggered = "LastTriggered";
const char* kConstraintEnabled = "ConstraintEnabled";
const char* kConstraintFrom = "ConstraintFrom";
const char* kConstraintTo = "ConstraintTo";
const char* kTriggerGroup = "Trigger_";   // Trigger_0, Trigger_1, ...
const char* kClassname = "Classname";
const char* kLastRun = "LastRun";
const char* kTimerAmount = "TimerAmount";
const char* kTimerUnit = "TimerUnit";
const char* kWhen = "When";
const char* kEventId = "EventId";

const char* kClassTimer = "LuaTriggerTimerInterval";
const char* kClassOnce = "LuaTriggerTimerOnce";
const char* kClassStartup = "LuaTriggerStartup";
const char* kClassShutdown = "LuaTriggerShutdown";
const char* kClassEvent = "LuaTriggerEvent";

// Earlier versions store "never" as the epoch.
QDateTime epoch() { return QDateTime(QDate(1970, 1, 1), QTime(0, 0, 0)); }
QDateTime readTime(const QSettings& settings, const char* key)
{
    const QDateTime dt = settings.value(key).toDateTime();
    return dt.isValid() && dt.toSecsSinceEpoch() > 86400 ? dt : QDateTime();
}
QDateTime orEpoch(const QDateTime& dt) { return dt.isValid() ? dt : epoch(); }
} // namespace

quint64 LuaTriggers::intervalSeconds(uint amount, uint unit)
{
    //                              sec, min, hour,  day,   week
    static const quint64 units[] = { 1,   60,  3600, 86400, 604800 };
    if(unit >= 5) return 0;
    const quint64 seconds = quint64(amount) * units[unit];
    return seconds <= 0xffffffffULL ? seconds : 0;
}

void LuaContainer::setName(const QString& name)
{
    _name = name.endsWith(".lua") ? name : name + ".lua";
}

bool LuaContainer::isTriggered(const LuaEvent& event)
{
    if(!_enabled) return false;
    const QTime time = event.timeStamp.time();
    if(_triggers.window && (time < _triggers.windowFrom || time > _triggers.windowTo)) return false;

    // The time the event happened, not when the queue got to it.
    const QDateTime at = event.timeStamp.isValid() ? event.timeStamp : QDateTime::currentDateTime();
    bool triggered = false;
    switch(event.eventId)
    {
    case L4R_TIMERTICK:
    {
        const quint64 interval = _triggers.every ? LuaTriggers::intervalSeconds(_triggers.everyAmount, _triggers.everyUnit) : 0;
        if(interval && (!_timerLastRun.isValid() || _timerLastRun.addSecs(qint64(interval)) <= at))
        {
            _timerLastRun = at.addMSecs(-at.time().msec());
            triggered = true;
        }
        // Not run since the time it was due; a few seconds late is fine.
        const QDateTime& due = _triggers.onceAt;
        if(_triggers.once && due.isValid() && (!_lastRun.isValid() || _lastRun < due) &&
           at >= due && at < due.addSecs(5))
            triggered = true;
        break;
    }
    case L4R_STARTUP:  triggered = _triggers.startup; break;
    case L4R_SHUTDOWN: triggered = _triggers.shutdown; break;
    default:           triggered = _triggers.eventIds.contains(event.eventId); break;
    }
    if(triggered) _lastRun = at;
    return triggered;
}

QString LuaContainer::getTriggerNames() const
{
    QStringList names;
    if(_triggers.every) names << "TimerInterval";
    if(_triggers.once) names << "TimerOnce";
    if(_triggers.startup) names << "Startup";
    if(_triggers.shutdown) names << "Shutdown";
    for(int i = 0; i < _triggers.eventIds.size(); ++i) names << "Event";
    if(names.isEmpty()) return QObject::tr("none");
    names.sort();
    return names.join("|");
}

void LuaContainer::toSettings(QSettings& settings)
{
    if(_description.isEmpty())
        _description = "It enters a description! It does so whenever it is told to.";
    settings.setValue(kDesc, _description);
    settings.setValue(kEnabled, _enabled);
    settings.setValue(kLastTriggered, orEpoch(_lastRun));
    settings.setValue(kConstraintEnabled, _triggers.window);
    settings.setValue(kConstraintFrom, _triggers.windowFrom);
    settings.setValue(kConstraintTo, _triggers.windowTo);

    int index = 0;
    auto group = [&](const char* className, const QDateTime& lastRun) {
        settings.beginGroup(kTriggerGroup + QString::number(index++));
        settings.setValue(kClassname, className);
        settings.setValue(kLastRun, orEpoch(lastRun));
    };
    if(_triggers.every)
    {
        group(kClassTimer, _timerLastRun);
        settings.setValue(kTimerAmount, _triggers.everyAmount);
        settings.setValue(kTimerUnit, _triggers.everyUnit);
        settings.endGroup();
    }
    if(_triggers.once)
    {
        group(kClassOnce, _lastRun);
        settings.setValue(kWhen, _triggers.onceAt);
        settings.endGroup();
    }
    if(_triggers.startup) { group(kClassStartup, _lastRun); settings.endGroup(); }
    if(_triggers.shutdown) { group(kClassShutdown, _lastRun); settings.endGroup(); }
    for(uint id : _triggers.eventIds)
    {
        group(kClassEvent, _lastRun);
        settings.setValue(kEventId, id);
        settings.endGroup();
    }
}

void LuaContainer::fromSettings(QSettings& settings)
{
    _description = settings.value(kDesc).toString();
    _enabled = settings.value(kEnabled, false).toBool();
    _lastRun = readTime(settings, kLastTriggered);
    _timerLastRun = QDateTime();

    _triggers = LuaTriggers();
    _triggers.window = settings.value(kConstraintEnabled, false).toBool();
    const QTime from = settings.value(kConstraintFrom).toTime(), to = settings.value(kConstraintTo).toTime();
    if(from.isValid()) _triggers.windowFrom = from;
    if(to.isValid()) _triggers.windowTo = to;

    for(const QString& name : settings.childGroups())
    {
        if(!name.startsWith(kTriggerGroup)) continue;
        settings.beginGroup(name);
        const QString className = settings.value(kClassname).toString();
        if(className == kClassTimer)
        {
            _triggers.every = true;
            _triggers.everyAmount = settings.value(kTimerAmount, 0).toUInt();
            _triggers.everyUnit = settings.value(kTimerUnit, 0).toUInt();
            _timerLastRun = readTime(settings, kLastRun);
        }
        else if(className == kClassOnce)
        {
            _triggers.once = true;
            _triggers.onceAt = settings.value(kWhen).toDateTime();
        }
        else if(className == kClassStartup) _triggers.startup = true;
        else if(className == kClassShutdown) _triggers.shutdown = true;
        else if(className == kClassEvent)
        {
            const uint id = settings.value(kEventId).toUInt();
            if(!_triggers.eventIds.contains(id)) _triggers.eventIds << id;
        }
        else
            RS_WARN("unknown trigger class '", className.toStdString(), "' in [", name.toStdString(), "]");
        settings.endGroup();
    }
}
