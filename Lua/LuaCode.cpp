#include <assert.h>
#include <fstream>
#include <ios>
#include <stdexcept>

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>

#include <retroshare/rsinit.h>

#include "LuaCode.h"


const std::string LuaCode::_folderName = "Lua4RS";
const std::string LuaCode::_extension = ".lua";
std::string LuaCode::_path = "";

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

// these are all static
void LuaCode::setOwnID(const std::string& id)
{
    _path = RsInit::RsConfigDirectory() + "/" + id + "/" + _folderName + "/";

    QDir p ( QString::fromStdString(_path) );

    if( !p.mkpath( QString::fromStdString( _path ) ) )
        throw  std::runtime_error( std::string( "Error", "Cannot create path at " ) + _path );
}

bool LuaCode::load(const std::string name, LuaCode &code)
{
    ///TODO better fix
    assert(_path != "");

    std::string fileName = _path + name;

    if( !QFile::exists(fileName.c_str()))
    {
        std::cerr << "[Lua] can't find file " << fileName << std::endl;
        return false;
    }

    std::ifstream file;
    file.open(fileName.c_str(), std::ios::in);
    if(!file.good() || !file.is_open())
    {
        std::cerr << "[Lua] can't open file " << fileName << std::endl;
        return false;
    }

    std::string line, c;

    code.setName(name);

    ///TODO description
    code.setDesc("todo");

    // rest = code
    c = "";
    while(!file.eof())
    {
        std::getline(file, line);
        c += line + '\n';
    }
    code.setCode(c);

    file.close();

    return true;
}

bool LuaCode::loadAll(codeMap &map)
{
    std::cout << "[Lua] loading all script files from " << _path << std::endl;

    // get everything with ".lua"
    /*
    QStringList nameFilter(QString::fromStdString("*" + _extension));
    QDir directory(QString::fromStdString(_path));
    QStringList files = directory.entryList(nameFilter);

    // drop folders
    for( QStringList::iterator it = files.begin(); it != files.end(); ++it)
        if( QFileInfo(*it).isDir())
            files.removeOne(*it);
    */

    QStringList files;
    QDirIterator dirIt(QString::fromStdString(_path));
    while (dirIt.hasNext()) {
        dirIt.next();
        if (QFileInfo(dirIt.filePath()).isFile())
            if (QFileInfo(dirIt.filePath()).suffix() == "lua")
                files.append(dirIt.fileName());
    }

    // load lua files
    for( QStringList::iterator it = files.begin(); it != files.end(); ++it)
    {
        LuaCode c;
        std::cout << "[Lua] loading file " << it->toStdString() << " ..." << std::endl;
        if(load(it->toStdString(), c))
            map.insert(std::make_pair(c.name(), c));
        else
            std::cerr << "[Lua] can't load file " << it->toStdString() << std::endl;
    }
    if(map.empty())
        return false;
    else
        return true;
}

bool LuaCode::save(LuaCode &code)
{
    ///TODO better fix
    assert(_path != "");

    std::string fileName = _path + code.name();

    // check for .lua ending
    {
        QFile f(QString::fromStdString(fileName));
        QFileInfo fi(f);
        if(fi.suffix() != "lua")
            fileName += ".lua";
    }

    std::ofstream file;
    file.open(fileName.c_str(), std::ios::trunc);
    if(!file.is_open())
    {
        std::cerr << "[Lua] can't open file " << fileName << std::endl;
        return false;
    }

    ///TODO dstription

    // rest = code
    file << code.code();

    file.flush();
    file.close();

    return true;
}

bool LuaCode::saveAll(codeMap &map)
{
    std::cout << "[Lua] saving all script files" << std::endl;
    bool r = true;
    for(codeMap::iterator it = map.begin(); it != map.end(); ++it)
        if(!save(it->second))
        {
            std::cerr << "[Lua] failed to save lua script " << it->first << std::endl;
            r = false;
        }

    return r;
}

//only one non static
bool inline LuaCode::save()
{
    return save(*this);
}

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

