#ifndef LUACONTAINER_H
#define LUACONTAINER_H

#include <QMap>
#include <QString>

#include "LuaCode.h"
#include "LuaConfig.h"

class LuaContainer
{
public:
    LuaContainer();
    LuaContainer(LuaCode &luacode);
    LuaContainer(LuaCode &luacode, LuaConfig &luaconfig);
    ~LuaContainer();

    LuaCode* getLuaCode();
    void setLuaCode(LuaCode& luacode);
    void setLuaCode(LuaCode* luacode);
    LuaConfig* getLuaConfig();
    void setLuaConfig(LuaConfig& luaconfig);
    void setLuaConfig(LuaConfig* luaconfig);

private:
    LuaCode* _luaCode;
    LuaConfig _luaConfig ;
};

#endif // LUACONTAINER_H
