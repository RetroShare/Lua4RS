#include "LuaScriptContainer.h"

// ctor
LuaScriptContainer::LuaScriptContainer()
{
}

// ctor
LuaScriptContainer::LuaScriptContainer(LuaCode luacode)
{
    setLuaCode(luacode);
}

// ctor
LuaScriptContainer::LuaScriptContainer(LuaCode luacode, LuaConfig luaconfig)
{
    setLuaCode(luacode);
    setLuaConfig(luaconfig);
}

// dtor
LuaScriptContainer::~LuaScriptContainer()
{
}


LuaCode LuaScriptContainer::getLuaCode()
{
    return _luacode;
}

LuaConfig LuaScriptContainer::getLuaConfig()
{
    return _luaconfig;
}

void LuaScriptContainer::setLuaCode(LuaCode luacode)
{
    _luacode = luacode;
}

void LuaScriptContainer::setLuaConfig(LuaConfig luaconfig)
{
    _luaconfig = luaconfig;
}




