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

    bool loadAll();
    bool saveAll();

    //bool itemAt(size_t index, LuaContainer*& container);
    bool itemByName(const QString& name, LuaContainer*& container);
    bool itemByName(const std::string& name, LuaContainer*& container);

    LuaContainerList::const_iterator begin();
    LuaContainerList::const_iterator end();

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
    bool load(const std::string& name, LuaContainer* container);
    bool save(LuaContainer* container);
    bool remove(LuaContainer* container);

    LuaContainerList _luaList;
    std::string _filePath;
};

#endif // LUALIST_H
