#include "LuaContainer.h"

LuaContainer::LuaContainer() :
    _luaCode(new LuaCode()),
    _luaConfig(new LuaConfig())
{
}

LuaContainer::LuaContainer(LuaCode* luacode) :
    _luaConfig(new LuaConfig())
{
    _luaCode = luacode;
}


LuaContainer::LuaContainer(LuaCode* luacode, LuaConfig* luaconfig)
{
    _luaCode = luacode;
    _luaConfig = luaconfig;
}

LuaContainer::~LuaContainer()
{
    delete _luaCode;
    delete _luaConfig;
}

// getter / setter
QString LuaContainer::getCode()
{
    return _luaCode->code();
}

QString LuaContainer::getDesc()
{
    return _luaCode->desc();
}

QString LuaContainer::getName()
{
    return _luaCode->name();
}

void LuaContainer::setCode(const QString& code)
{
    _luaCode->setCode(code);
}

void LuaContainer::setCode(const std::string& code)
{
    setCode(QString::fromStdString(code));
}

void LuaContainer::setDesc(const QString& desc)
{
    _luaCode->setDesc(desc);
}

void LuaContainer::setDesc(const std::string& desc)
{
    setDesc(QString::fromStdString(desc));
}

void LuaContainer::setName(const QString& name)
{
    _luaCode->setName(name);
}

void LuaContainer::setName(const std::string& name)
{
    setName(QString::fromStdString(name));
}

void LuaContainer::getSettings(QSettings& settings)
{
    _luaConfig->toSettings(settings);
}

void LuaContainer::loadSettings(QSettings& settings)
{
    _luaConfig->fromSettings(settings);
}
