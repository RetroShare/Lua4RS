#ifndef LUALIST_H
#define LUALIST_H

#include <stdlib.h>
#include <vector>

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QString>

#include "LuaContainer.h"

///todo find better type? maybe a map?
typedef std::vector<LuaContainer*> LuaContainerList;

class LuaList
{
public:
    LuaList();
    ~LuaList();

    bool loadAll(const std::string& path);
    bool saveAll(const std::string& path);

    bool itemAt(size_t index, LuaContainer*& container);
    bool itemByName(const QString& name, LuaContainer*& container);
    bool itemByName(const std::string& name, LuaContainer*& container);

    void addItem(LuaContainer* lc);

    size_t size();


    // debug
    void dump();

private:
    bool load(const std::string& name, LuaContainer* container, const std::string& path);
    bool save(LuaContainer* container, const std::string& path);

    LuaContainerList _luaList;
};

#endif // LUALIST_H
