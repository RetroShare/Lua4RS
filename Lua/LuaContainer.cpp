#include "LuaContainer.h"

// ctor
LuaContainer::LuaContainer()
{
}

// ctor
LuaContainer::LuaContainer(LuaCode luacode)
{
    setLuaCode(luacode);
}

// ctor
LuaContainer::LuaContainer(LuaCode luacode, LuaConfig luaconfig)
{
    setLuaCode(luacode);
    setLuaConfig(luaconfig);
}

// dtor
LuaContainer::~LuaContainer()
{
}


LuaCode LuaContainer::getLuaCode()
{
    return _luacode;
}

LuaConfig LuaContainer::getLuaConfig()
{
    return _luaconfig;
}

void LuaContainer::setLuaCode(LuaCode luacode)
{
    _luacode = luacode;
}

void LuaContainer::setLuaConfig(LuaConfig luaconfig)
{
    _luaconfig = luaconfig;
}




