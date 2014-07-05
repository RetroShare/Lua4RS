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
    clearList();
}

void LuaList::clearList()
{
    for(LuaContainerList::const_iterator it = _luaList.begin(); it != _luaList.end(); ++it)
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
    for(LuaContainerList::const_iterator it = _luaList.begin(); it != _luaList.end(); ++it)
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

LuaContainer* LuaList::createItem()
{
    LuaContainer* container = new LuaContainer();

    /*
     * name the new container newX.lua with x being the lower number that is not already used
     *
     * first iterate over all container and get all numbers that are in use (like new1.lua, new3.lua and new4.lua)
     * second iterate over all numbers in use and finde the smalles not used
     *
     * new42.lua
     * 012345678 <- position
     *
     * 42 is at pos 3 and 2 chars long
     *
     * --> to get the number substring(3, length - 7) will do the trick
     */
    QList<int> usedNr;
    QString name, nr;
    for(LuaContainerList::const_iterator it = _luaList.begin(); it != _luaList.end(); ++it)
    {
        name = (*it)->getName();

        // 'new0.lua' is 8 char. long --> minimum length is 8
        if(name.length() < 8)
            continue;

        if(!(*it)->getName().startsWith("new"))
            continue;

        // try to get the number
        nr = name.mid(3, name.length() - 7);
        int i;
        bool ok;
        i = nr.toInt(&ok);
        if(ok)
            usedNr.push_back(i);
    }

    // very important
    qSort(usedNr);

    if(usedNr.empty())
        // easy!
        container->setName(QString("new0.lua"));
    else
    {
        QString name;
        for(int i = 0; i  < usedNr.size(); ++i)
        {
            // usedNr[i] shoudl be i! (since we sorted it before)
            if(usedNr[i] == i)
                continue;

            // if not, we found an unused number

            name = "new";
            name += QString::number(i);
            name += ".lua";

            break;
        }
        // if all fails ...
        if(name == "")
        {
            name = "new";
            name += QString::number(usedNr.size());
            name += ".lua";
        }
        container->setName(name);
    }
    return container;
}

void LuaList::addItem(LuaContainer* container)
{
    for(LuaContainerList::const_iterator it = _luaList.begin(); it != _luaList.end(); ++it)
        if((*it)->getName() == container->getName())
        {
            // a file with the same name already exists
            std::cerr << "[Lua] LuaList::addItem : ERROR: name already exists " << container->getName().toStdString() << std::endl;
            return;
        }

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
    // remove item from list
    removeItem(container);

    // remove item from disk
    bool rc = remove(container);

    // remove item from ram
    delete container;

    return rc;
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

bool LuaList::load(const QString& name, LuaContainer* container, bool ignoreNoSettingsFile)
{
    if(_filePath == "")
        return false;

    // set name
    container->setName(name);

    QString luaFileName, settingsFileName;
    getFileNames(name, luaFileName, settingsFileName);

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
    QStringList files;
    QDirIterator dirIt(_filePath);
    while (dirIt.hasNext()) {
        dirIt.next();
        if (QFileInfo(dirIt.filePath()).isFile())
            if (QFileInfo(dirIt.filePath()).suffix() == "lua")
                files.append(dirIt.fileName());
    }

    if(!_luaList.empty())
        // clear list before loading
        clearList();

    // load lua files
    for( QStringList::iterator it = files.begin(); it != files.end(); ++it)
    {
        LuaContainer* c = new LuaContainer();
        std::cout << "[Lua] loading file " << it->toStdString() << " ..." << std::endl;
        if(load((*it), c))
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

    QString luaFileName, settingsFileName;
    getFileNames(container->getName(), luaFileName, settingsFileName);

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
        settings.clear();
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

void LuaList::rename(const QString& oldName, const QString& newName)
{
    QString luaFileNameOld, luaFileNameNew, settingsFileNameOld, settingsFileNameNew;
    getFileNames(oldName, luaFileNameOld, settingsFileNameOld);
    getFileNames(newName, luaFileNameNew, settingsFileNameNew);

    QFile fCode(luaFileNameOld);
    QFile fSettings(settingsFileNameOld);

    if(fCode.exists())
        fCode.rename(luaFileNameNew);
    if(fSettings.exists())
        fSettings.rename(settingsFileNameNew);
}

void LuaList::getFileNames(const QString& name, QString& luaFileName, QString& settingsFileName)
{
    luaFileName = _filePath + name;
    settingsFileName =  luaFileName + QString(".ini");
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
