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

// getter / setter
LuaContainer::~LuaContainer()
{
    delete _luaCode;
    delete _luaConfig;
}

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

/*
LuaCode* LuaContainer::getLuaCode()
{
    return _luaCode;
}

void LuaContainer::setLuaCode(LuaCode& luacode)
{
    _luaCode = &luacode;
}

void LuaContainer::setLuaCode(LuaCode* luacode)
{
    _luaCode = luacode;
}

LuaConfig *LuaContainer::getLuaConfig()
{
    return &_luaConfig;
}

void LuaContainer::setLuaConfig(LuaConfig& luaconfig)
{
    _luaConfig = luaconfig;
}

void LuaContainer::setLuaConfig(LuaConfig *luaconfig)
{
    _luaConfig = *luaconfig;
}
*/
