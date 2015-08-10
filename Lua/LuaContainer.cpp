#include "LuaContainer.h"
#include "LuaCode.h"
#include "LuaConfig.h"

LuaContainer::LuaContainer() :
    _code(new LuaCode()),
    _config(new LuaConfig())
{
}

LuaContainer::LuaContainer(LuaCode* luacode) :
    _config(new LuaConfig())
{
    _code = luacode;
}


LuaContainer::LuaContainer(LuaCode* luacode, LuaConfig* luaconfig)
{
    _code = luacode;
    _config = luaconfig;
}

LuaContainer::~LuaContainer()
{
    delete _code;
    delete _config;
}

bool LuaContainer::isTriggered(const LuaEvent& event)
{
    return _config->isTriggered(event);
}

// getter / setter

QString LuaContainer::getCode()                         { return _code->code(); }
void    LuaContainer::setCode(const std::string& code)  { setCode(QString::fromUtf8(code.c_str())); }
void    LuaContainer::setCode(const QString& code)      { _code->setCode(code); }

QString LuaContainer::getDesc()                         { return _config->getDescription(); }
void    LuaContainer::setDesc(const std::string& desc)  { setDesc(QString::fromStdString(desc)); }
void    LuaContainer::setDesc(const QString& desc)      { _config->setDescription(desc); }

QString LuaContainer::getName()                         { return _code->name(); }
void    LuaContainer::setName(const std::string& name)  { setName(QString::fromStdString(name)); }
void    LuaContainer::setName(const QString& name)
{
    if(name.endsWith(".lua"))
        _code->setName(name);
    else
        _code->setName(name + ".lua");
}

void    LuaContainer::getSettings(QSettings& settings)  { _config->toSettings(settings); }
void    LuaContainer::loadSettings(QSettings& settings) { _config->fromSettings(settings); }

bool    LuaContainer::getEnabled()                  { return _config->isScriptEnabled(); }
void    LuaContainer::setEnabled(const bool enable) { _config->enableScript(enable); }

QDateTime   LuaContainer::getLastTriggered()                    { return _config->getLastTriggered(); }
void        LuaContainer::setLastTriggered(const QDateTime& dt) { _config->setLastTriggered(dt); }

// constraint

bool    LuaContainer::getConstraintEnabled()                    { return _config->isConstraintEnabled(); }
void    LuaContainer::setConstraintEnabled(const bool enable)   { _config->enableConstraint(enable); }

void    LuaContainer::getConstraintFromTo(QTime& from, QTime& to)                { from = _config->getConstraintFrom(); to = _config->getConstraintTo(); }
void    LuaContainer::setConstraintFromTo(const QTime& from, const QTime& to)    { _config->setConstraintFrom(from); _config->setConstraintTo(to); }

// trigger
void    LuaContainer::removeAllTrigger() { _config->removeAllTrigger(); }

// every
void    LuaContainer::addRunEveryTrigger(uint amout, uint unit)
{
    std::cout << "[Lua] LuaContainer::addRunEveryTrigger() : triggercount=" << _config->getTriggerCount() << std::endl;
    _config->addTrigger(new LuaTriggerTimerInterval(amout, unit));
}

bool    LuaContainer::getRunEveryChecked(uint& amout, uint& unit)
{
    for(QList<LuaTriggerBase*>::const_iterator it = _config->triggersBegin(); it != _config->triggersEnd(); ++it)
    {
        if((*it)->classname() == LUA_TRIGGER_TIMER_INTERVAL)
        {
            LuaTriggerTimerInterval* t = dynamic_cast<LuaTriggerTimerInterval*>(*it);
            if(t == NULL)
            {
                std::cerr << "[Lua] LuaContainer::getRunEveryChecked() - failed cast!" << std::endl;
                continue; // since there should be only one trigger with that name the continue might be useless - keep it for now
            }
            t->getValues(amout, unit);
            return true;
        }
    }
    return false;
}

// once
void    LuaContainer::addRunOnceTrigger(const QDateTime& when)
{
    std::cout << "[Lua] LuaContainer::addRunEveryTrigger() : triggercount=" << _config->getTriggerCount() << std::endl;
    _config->addTrigger(new LuaTriggerOnce(when));
}

bool    LuaContainer::getRunOnceChecked(QDateTime& when)
{
    for(QList<LuaTriggerBase*>::const_iterator it = _config->triggersBegin(); it != _config->triggersEnd(); ++it)
    {
        if((*it)->classname() == LUA_TRIGGER_ONCE)
        {
            LuaTriggerOnce* t = dynamic_cast<LuaTriggerOnce*>(*it);
            if(t == NULL)
            {
                std::cerr << "[Lua] LuaContainer::getRunOnceChecked() - failed cast!" << std::endl;
                continue; // since there should be only one trigger with that name the continue might be useless - keep it for now
            }
            when = t->getValues();
            return true;
        }
    }
    return false;
}

// start up
void    LuaContainer::addRunStratupTrigger()
{
    std::cout << "[Lua] LuaContainer::addRunEveryTrigger() : triggercount=" << _config->getTriggerCount() << std::endl;
    _config->addTrigger(new LuaTriggerStartup());
}

bool    LuaContainer::getRunStartupChecked()
{
    for(QList<LuaTriggerBase*>::const_iterator it = _config->triggersBegin(); it != _config->triggersEnd(); ++it)
    {
        if((*it)->classname() == LUA_TRIGGER_STARTUP)
            return true;
    }
    return false;
}

// shut down
void    LuaContainer::addRunShutdownTrigger()
{
    std::cout << "[Lua] LuaContainer::addRunEveryTrigger() : triggercount=" << _config->getTriggerCount() << std::endl;
    _config->addTrigger(new LuaTriggerShutdown());
}

bool    LuaContainer::getRunShutdownChecked()
{
    for(QList<LuaTriggerBase*>::const_iterator it = _config->triggersBegin(); it != _config->triggersEnd(); ++it)
    {
        if((*it)->classname() == LUA_TRIGGER_SHUTDOWN)
            return true;
    }
    return false;
}

void LuaContainer::addEventTrigger(uint eventId)
{
    std::cout << "[Lua] LuaContainer::addEventTrigger() : triggercount=" << _config->getTriggerCount() << std::endl;
    _config->addTrigger(new LuaTriggerEvent(eventId));
}

bool LuaContainer::getEventTriggerChecked(uint eventId)
{
    for(QList<LuaTriggerBase*>::const_iterator it = _config->triggersBegin(); it != _config->triggersEnd(); ++it)
    {
        if((*it)->classname() == "LuaTriggerEvent")
        {
            LuaTriggerEvent *e = dynamic_cast<LuaTriggerEvent*>(*it);
            if(e != NULL && e->getEventId() == eventId)
                return true;
        }
    }
    return false;
}





























