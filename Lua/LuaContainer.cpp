#include "LuaContainer.h"

LuaContainer::LuaContainer() :
    _luaCode(new LuaCode())
{
}

LuaContainer::LuaContainer(LuaCode& luacode)
{
    setLuaCode(luacode);
}


LuaContainer::LuaContainer(LuaCode& luacode, LuaConfig& luaconfig)
{
    setLuaCode(luacode);
    setLuaConfig(luaconfig);
}


LuaContainer::~LuaContainer()
{
    delete _luaCode;
}


// getter / setter
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
