#include <QString>

#include "LuaCode.h"

LuaCode::LuaCode() :
    _code(""),
    _name("")
{
}

LuaCode::LuaCode(QString name) :
    _code(""),
    _name(name)
{
}

LuaCode::LuaCode(QString name, QString code) :
    _code(code),
    _name(name)
{
}

// getter/setter
QString LuaCode::code() const
{
    return _code;
}

void LuaCode::setCode(const QString &code)
{
    _code = code;
}

QString LuaCode::name() const
{
    return _name;
}

void LuaCode::setName(const QString &name)
{
    _name = name;
}

