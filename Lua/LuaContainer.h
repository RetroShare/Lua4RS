#ifndef LUACONTAINER_H
#define LUACONTAINER_H

#include <QSettings>
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

    QString getCode();
    QString getDesc();
    QString getName();

    void setCode(const QString& code);
    void setCode(const std::string& code);
    void setDesc(const QString& desc);
    void setDesc(const std::string& desc);
    void setName(const QString& name);
    void setName(const std::string& name);

    void getSettings(QSettings& settings);
    void loadSettings(QSettings& settings);

    bool getConstraintsEnable();
    void setConstraintsEnable(const bool enable);

private:
    LuaCode* _luaCode;
    LuaConfig* _luaConfig ;
};

#endif // LUACONTAINER_H
