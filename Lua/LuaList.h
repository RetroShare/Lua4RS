#ifndef LUALIST_H
#define LUALIST_H

#include <stdlib.h>
#include <list>

#include "LuaContainer.h"

///todo find better type? maybe a map?
typedef std::list<LuaContainer*> LuaContainerList;

class LuaList
{
public:
    LuaList();
    ~LuaList();

    // File name rules for scripts (GUI and MCP): ends with .lua, no folders,
    // no characters that are special in file names.
    static bool isValidName(const QString& name);

    bool loadAll();
    bool saveAll();
    // Only the .ini (triggers, last runs); the code stays as it is on disk.
    bool saveSettings(LuaContainer* container);
    void rename(const QString& oldName, const QString& newName);

    //bool itemAt(size_t index, LuaContainer*& container);
    bool itemByName(const QString& name, LuaContainer*& container);
    bool itemByName(const std::string& name, LuaContainer*& container);

    LuaContainerList::const_iterator begin();
    LuaContainerList::const_iterator end();

    LuaContainer* createItem();

    void addItem(LuaContainer* container);
    bool addItemAndSave(LuaContainer* container);
    void removeItem(LuaContainer* container);
    bool removeItemAndDelete(LuaContainer* container);

    size_t size();
    void sort();

    void setFilePath(const std::string& path);

    // debug
    void dump();

private:
    bool load(const QString& name, LuaContainer* container, bool ignoreNoSettingsFile = true);
    bool save(LuaContainer* container);
    bool remove(LuaContainer* container);

    void getFileNames(const QString& name, QString& luaFileName, QString& settingsFileName);

    void clearList();

    LuaContainerList _luaList;
    QString _filePath;
};

#endif // LUALIST_H
