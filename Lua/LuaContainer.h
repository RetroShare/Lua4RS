#ifndef LUACONTAINER_H
#define LUACONTAINER_H

#include <QHash>
#include "LuaCode.h"
#include "LuaConfig.h"

//typedef  QHash<QString,QString> LuaConfig;

class LuaContainer
{
public:
    LuaContainer();
    LuaContainer(LuaCode luacode);
    LuaContainer(LuaCode luacode, LuaConfig luaconfig);
    ~LuaContainer();

    LuaCode getLuaCode();
    LuaConfig getLuaConfig();
    void setLuaCode(LuaCode luacode);
    void setLuaConfig(LuaConfig luaconfig);

private:
    LuaCode _luacode;
    LuaConfig _luaconfig ;
};

#endif // LUACONTAINER_H

