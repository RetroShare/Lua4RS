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

// getter / setter
QString LuaContainer::getCode()                         { return _code->code(); }
void    LuaContainer::setCode(const QString& code)      { _code->setCode(code); }
void    LuaContainer::setCode(const std::string& code)  { setCode(QString::fromStdString(code)); }

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
