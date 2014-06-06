#include "LuaCode.h"

LuaCode::LuaCode() :
    _code(""),
    _desc(""),
    _name("")
{
}

LuaCode::LuaCode(std::string name, std::string desc) :
    _code(""),
    _desc(desc),
    _name(name)
{
}

LuaCode::LuaCode(std::string name, std::string desc, std::string code) :
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
std::string LuaCode::code() const
{
    return _code;
}

void LuaCode::setCode(const std::string &code)
{
    _code = code;
}

std::string LuaCode::desc() const
{
    return _desc;
}

void LuaCode::setDesc(const std::string &desc)
{
    _desc = desc;
}

std::string LuaCode::name() const
{
    return _name;
}

void LuaCode::setName(const std::string &name)
{
    _name = name;
}

