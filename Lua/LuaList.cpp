#include <algorithm>

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QString>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringDecoder>
#else
#include <QTextCodec>
#endif

#include "LuaList.h"

#include <QRegularExpression>
#include "util/rsdebuglevel0.h"

namespace {
// Windows: CRLF on disk, LF in memory (as earlier versions did). Elsewhere the
// bytes stay as they are: the approval hash covers the code in memory.
#ifdef _WIN32
const QIODevice::OpenMode kTextMode = QIODevice::Text;
#else
const QIODevice::OpenMode kTextMode = QIODevice::NotOpen;
#endif

// Scripts are UTF-8 (a BOM is dropped). Earlier versions saved them in the
// local 8-bit encoding on Windows; those still load.
QString decodeScript(const QByteArray& data)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QStringDecoder utf8(QStringConverter::Utf8);
    const QString code = utf8.decode(data);
    if(!utf8.hasError()) return code;
#else
    QTextCodec::ConverterState state; // default flags drop the BOM
    const QString code = QTextCodec::codecForName("UTF-8")->toUnicode(data.constData(), data.size(), &state);
    if(state.invalidChars == 0) return code;
#endif
    return QString::fromLocal8Bit(data);
}
} // namespace

bool LuaList::isValidName(const QString& name)
{
    static const QRegularExpression special("[<>\"|?*]");
    return name.endsWith(".lua") && name != ".lua" && !name.contains('/') && !name.contains('\\') &&
           !name.contains(':') && !name.contains("..") && !name.contains(special) &&
           name.trimmed() == name;
}

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
     * second iterate over all numbers in use and find the smallest not used
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
    std::sort(usedNr.begin(), usedNr.end());

    if(usedNr.empty())
        // easy!
        container->setName(QString("new0.lua"));
    else
    {
        QString name;
        for(int i = 0; i  < usedNr.size(); ++i)
        {
            // usedNr[i] should be i! (since we sorted it before)
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
            RS_WARN("name already exists: ", container->getName().toStdString());
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
    return a->getName().compare(b->getName(), Qt::CaseInsensitive) < 0;
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
            RS_ERR("can't find Lua file ", luaFileName.toStdString());
            return false;
        }

        QFile file(luaFileName);
        if(!file.open(QIODevice::ReadOnly | kTextMode))
        {
            RS_ERR("can't open file ", luaFileName.toStdString());
            return false;
        }
        container->setCode(decodeScript(file.readAll()));
    }

    // load config
    {
        // check whether file exists
        if(!QFile::exists(settingsFileName))
        {
            if(ignoreNoSettingsFile)
            {
                RS_DBG1("no settings file ", settingsFileName.toStdString(), ", continuing");
                return true;
            } else
            {
                RS_ERR("can't find settings file ", settingsFileName.toStdString());
                return false;
            }
        }

        QSettings settings(settingsFileName, QSettings::IniFormat);
        container->fromSettings(settings);
    }
    return true;
}

bool LuaList::loadAll()
{
    RS_DBG1("loading all script files from ", _filePath.toStdString());

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
        RS_DBG1("loading file ", it->toStdString());
        if(load((*it), c))
            _luaList.push_back(c);
        else
        {
            delete c;
            RS_ERR("can't load file ", it->toStdString());
        }
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

    // save code
    {
        // Always UTF-8; written to a temporary file first, so a crash never
        // leaves half a script.
        QSaveFile file(luaFileName);
        const QByteArray code = container->getCode().toUtf8();
        if(!file.open(QIODevice::WriteOnly | kTextMode) || file.write(code) != code.size() || !file.commit())
        {
            RS_ERR("can't write file ", luaFileName.toStdString(), ": ", file.errorString().toStdString());
            return false;
        }
    }

    return saveSettings(container);
}

bool LuaList::saveSettings(LuaContainer* container)
{
    if(_filePath == "")
        return false;

    QString luaFileName, settingsFileName;
    getFileNames(container->getName(), luaFileName, settingsFileName);

    QSettings settings(settingsFileName, QSettings::IniFormat);
    if(!settings.isWritable())
    {
        RS_ERR("no write access for settings file ", settingsFileName.toStdString());
        return false;
    }
    settings.clear();
    container->toSettings(settings);
    settings.sync();
    return settings.status() == QSettings::NoError;
}

bool LuaList::saveAll()
{
    bool r = true;
    for(LuaContainerList::iterator it = _luaList.begin(); it != _luaList.end(); ++it)
        if(!save((*it)))
        {
            RS_ERR("failed to save Lua script ", (*it)->getName().toStdString());
            r = false;
        }

    return r;
}

bool LuaList::remove(LuaContainer *container)
{
    if(_filePath == "")
        return false;

    QString luaFileName, settingsFileName;
    getFileNames(container->getName(), luaFileName, settingsFileName);

    if(!QFile::exists(luaFileName))
    {
        RS_ERR("can't find file ", luaFileName.toStdString());
        return false;
    }
    if(!QFile::remove(luaFileName)) return false;
    // The triggers and last runs belong to the script.
    if(QFile::exists(settingsFileName) && !QFile::remove(settingsFileName))
        RS_WARN("can't remove ", settingsFileName.toStdString());
    return true;
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
