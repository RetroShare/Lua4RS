#ifndef LUASCRIPTCONTAINER_H
#define LUASCRIPTCONTAINER_H

#include <QHash>
#include "LuaCode.h"
#include "LuaConfig.h"

//typedef  QHash<QString,QString> LuaConfig;

class LuaScriptContainer
{
public:
    LuaScriptContainer();
    LuaScriptContainer(LuaCode luacode);
    LuaScriptContainer(LuaCode luacode, LuaConfig luaconfig);
    ~LuaScriptContainer();

    LuaCode getLuaCode();
    LuaConfig getLuaConfig();
    void setLuaCode(LuaCode luacode);
    void setLuaConfig(LuaConfig luaconfig);

private:
    LuaCode _luacode;
    LuaConfig _luaconfig ;
};

#endif // LUASCRIPTCONTAINER_H

