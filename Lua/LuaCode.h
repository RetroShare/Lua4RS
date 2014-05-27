#ifndef LUACODE_H
#define LUACODE_H

#include <string>

class LuaCode
{
public:
    LuaCode();
    LuaCode(std::string name, std::string desc);
    LuaCode(std::string name, std::string desc, std::string code);

    bool load();
    bool save();

    // getter/setter
    std::string code() const;
    void setCode(const std::string &code);

    std::string desc() const;
    void setDesc(const std::string &desc);

    std::string name() const;
    void setName(const std::string &name);

private:
    std::string _code;
    std::string _desc;
    std::string _name;
};

#endif // LUACODE_H
