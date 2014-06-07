#ifndef LUACODE_H
#define LUACODE_H

#include <string>
#include <map>

///TODO better fix of this include mess ..
class LuaCode;
typedef std::map<std::string, LuaCode>      codeMap;

#include "LuaCore.h"

class LuaCode
{
public:
    LuaCode();
    LuaCode(std::string name, std::string desc);
    LuaCode(std::string name, std::string desc, std::string code);

    static bool load(const std::string name, LuaCode& code);
    static bool loadAll(codeMap& map);
    static bool save(LuaCode& code);
    static bool saveAll(codeMap& map);
    bool save();

    static void setOwnID(const std::string& id);

    // getter/setter
    std::string code() const;
    void setCode(const std::string &code);

    std::string desc() const;
    void setDesc(const std::string &desc);

    std::string name() const;
    void setName(const std::string &name);

private:
    static const std::string _folderName;
    static const std::string _extension;
    static std::string _path;
    std::string _code;
    std::string _desc;
    std::string _name;
};

#endif // LUACODE_H
