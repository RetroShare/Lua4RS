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
    LuaContainer(LuaCode* luacode);
    LuaContainer(LuaCode* luacode, LuaConfig* luaconfig);
    ~LuaContainer();

    /*
    LuaCode* getLuaCode();
    void setLuaCode(LuaCode& luacode);
    void setLuaCode(LuaCode* luacode);
    LuaConfig* getLuaConfig();
    void setLuaConfig(LuaConfig& luaconfig);
    void setLuaConfig(LuaConfig* luaconfig);
    */

    QString getCode();
    QString getDesc();
    QString getName();

    void setCode(const QString& code);
    void setCode(const std::string& code);
    void setDesc(const QString& desc);
    void setDesc(const std::string& desc);
    void setName(const QString& name);
    void setName(const std::string& name);

private:
    LuaCode* _luaCode;
    LuaConfig* _luaConfig ;
};

#endif // LUACONTAINER_H
