#include <fstream>
#include <stdexcept>
#include <iostream>

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QString>

#include "LuaList.h"

LuaList::LuaList() :
    _filePath("")
{
    _luaList.clear();
}

LuaList::~LuaList()
{
    for(LuaContainerList::iterator it = _luaList.begin(); it != _luaList.end(); ++it)
        delete *it;

    _luaList.clear();
}

void LuaList::setFilePath(const std::string& path)
{
    _filePath = QString::fromStdString(path);

    if(!QDir(_filePath).exists())
        QDir().mkpath(_filePath);
}

bool LuaList::itemByName(const QString &name, LuaContainer*& container)
{
    for(LuaContainerList::iterator it = _luaList.begin(); it != _luaList.end(); ++it)
        if((*it)->getName() == name)
        {
            container = (*it);
            return true;
        }

    return false;

}

bool LuaList::itemByName(const std::string& name, LuaContainer*& container)
{
    return itemByName(QString::fromStdString(name), container);
}

LuaContainerList::const_iterator LuaList::begin()
{
    return _luaList.begin();
}

LuaContainerList::const_iterator LuaList::end()
{
    return _luaList.end();
}

void LuaList::addItem(LuaContainer* container)
{
    _luaList.push_back(container);
}

bool LuaList::addItemAndSave(LuaContainer* container)
{
    addItem(container);
    return save(container);
}

void LuaList::removeItem(LuaContainer* container)
{
    _luaList.remove(container);
}

bool LuaList::removeItemAndDelete(LuaContainer* container)
{
    removeItem(container);
    return remove(container);
}

size_t LuaList::size()
{
    return _luaList.size();
}

bool LuaListCompare(LuaContainer* a, LuaContainer* b)
{
    return a->getName().compare(b->getName(), Qt::CaseInsensitive);
}

void LuaList::sort()
{
    _luaList.sort(LuaListCompare);
}

bool LuaList::load(const std::string& name, LuaContainer* container, bool ignoreNoSettingsFile)
{
    if(_filePath == "")
        return false;

    // set name
    container->setName(name);

    QString luaFileName = _filePath + QString::fromStdString(name);
    QString settingsFileName =  luaFileName + QString(".ini");

    // load code
    {
        // check whether file exists
        if(!QFile::exists(luaFileName))
        {
            std::cerr << "[Lua] can't find Lua file " << luaFileName.toStdString() << std::endl;
            return false;
        }

        std::ifstream file;
        file.open(luaFileName.toLocal8Bit().data(), std::ios::in);
        if(!file.good() || !file.is_open())
        {
            std::cerr << "[Lua] can't open file " << luaFileName.toStdString() << std::endl;
            return false;
        }

        std::string line, c = "";
        while(!file.eof())
        {
            std::getline(file, line);
            c += line;

            // don't add \n at the end of the file
            if(!file.eof())
                c += '\n';
        }
        container->setCode(c);

        file.close();
    }

    // load config
    {
        // check whether file exists
        if(!QFile::exists(settingsFileName))
        {
            if(ignoreNoSettingsFile)
            {
                std::cerr << "[Lua] can't find settings file " << settingsFileName.toStdString() << " -> continuing" << std::endl;
                return true;
            } else
            {
                std::cerr << "[Lua] can't find settings file " << settingsFileName.toStdString() << " -> aborting" << std::endl;
                return false;
            }
        }

        QSettings settings(settingsFileName, QSettings::IniFormat);
        container->loadSettings(settings);
    }
    return true;
}

bool LuaList::loadAll()
{
    std::cout << "[Lua] loading all script files from " << _filePath.toStdString() << std::endl;

    // get everything with ".lua"
    /*
    QStringList nameFilter(QString::fromStdString("*.lua"));
    QDir directory(QString::fromStdString(path));
    QStringList files = directory.entryList(nameFilter);

    // drop folders
    for( QStringList::iterator it = files.begin(); it != files.end(); ++it)
        if( QFileInfo(*it).isDir())
            files.removeOne(*it);
    */


    QStringList files;
    QDirIterator dirIt(_filePath);
    while (dirIt.hasNext()) {
        dirIt.next();
        if (QFileInfo(dirIt.filePath()).isFile())
            if (QFileInfo(dirIt.filePath()).suffix() == "lua")
                files.append(dirIt.fileName());
    }


    // load lua files
    for( QStringList::iterator it = files.begin(); it != files.end(); ++it)
    {
        LuaContainer* c = new LuaContainer();
        std::cout << "[Lua] loading file " << it->toStdString() << " ..." << std::endl;
        if(load(it->toStdString(), c))
            _luaList.push_back(c);
        else
            std::cerr << "[Lua] can't load file " << it->toStdString() << std::endl;
    }
    if(_luaList.empty())
        return false;
    else
        return true;
}

bool LuaList::save(LuaContainer* container)
{
    if(_filePath == "")
        return false;

    QString luaFileName = _filePath + container->getName();
    // check for .lua ending
    {
        QFile f(luaFileName);
        QFileInfo fi(f);
        if(fi.suffix() != "lua")
            luaFileName += ".lua";
    }
    QString settingsFileName =  luaFileName + QString(".ini");

    // safe code
    {
        std::ofstream file;
        file.open(luaFileName.toLocal8Bit().data(), std::ios::trunc);
        if(!file.is_open())
        {
            std::cerr << "[Lua] can't open file " << luaFileName.toStdString() << std::endl;
            return false;
        }

        std::string code;
#ifdef _WIN32
        code = container->getCode().toLocal8Bit().constData();
#else
        code = container->getCode().toUtf8().constData();
#endif
        // rest = code
        file << code;

        file.flush();
        file.close();
    }

    // save config
    {
        QSettings settings(settingsFileName, QSettings::IniFormat);
        if(!settings.isWritable())
        {
            std::cerr << "[Lua] no write access for settings file " << settingsFileName.toStdString() << " -> aborting (code was saved)" << std::endl;
            return false;
        }
        container->getSettings(settings);
    }

    return true;
}

bool LuaList::saveAll()
{
    std::cout << "[Lua] saving all script files" << std::endl;
    bool r = true;
    for(LuaContainerList::iterator it = _luaList.begin(); it != _luaList.end(); ++it)
        if(!save((*it)))
        {
            std::cerr << "[Lua] failed to save lua script " << (*it)->getName().toStdString() << std::endl;
            r = false;
        }

    return r;
}

bool LuaList::remove(LuaContainer *container)
{
    if(_filePath == "")
        return false;

    QString fileName = _filePath + container->getName();

    if( !QFile::exists(fileName))
    {
        std::cerr << "[Lua] can't find file " << fileName.toStdString() << std::endl;
        return false;
    }

    QFile f(fileName);
    return f.remove();
}

void LuaList::dump()
{
    std::cout << "[Lua] dumping LuaList: " << size() << " elements" << std::endl;
    uint counter = 0;
    for( LuaContainerList::iterator it = _luaList.begin(); it != _luaList.end(); ++it, counter++)
    {
        LuaContainer* c = *it;
        std::cout << " "<< counter << ": ";
        if( c == NULL)
            std::cout << "NULL" << std::endl;
        else
        {
            std::cout << "Name: " << c->getName().toStdString() << std::endl;
            std::cout << "Desc: " << c->getDesc().toStdString() << std::endl;
        }
    }
}
