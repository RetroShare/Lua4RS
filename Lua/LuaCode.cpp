#include <QString>

#include "LuaCode.h"

LuaCode::LuaCode() :
    _code(""),
    _desc(""),
    _name("")
{
}

LuaCode::LuaCode(QString name, QString desc) :
    _code(""),
    _desc(desc),
    _name(name)
{
}

LuaCode::LuaCode(QString name, QString desc, QString code) :
    _code(code),
    _desc(desc),
    _name(name)
{
}

/*
//only one non static
bool inline LuaCode::save()
{
    return save(*this);
}
*/

// getter/setter
QString LuaCode::code() const
{
    return _code;
}

void LuaCode::setCode(const QString &code)
{
    _code = code;
}

QString LuaCode::desc() const
{
    return _desc;
}

void LuaCode::setDesc(const QString &desc)
{
    _desc = desc;
}

QString LuaCode::name() const
{
    return _name;
}

void LuaCode::setName(const QString &name)
{
    _name = name;
}

