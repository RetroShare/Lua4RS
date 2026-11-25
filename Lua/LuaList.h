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

    // File name rules for new names of scripts (GUI and MCP): ends with .lua,
    // no folders, nothing that is special in file names on some platform
    // (Windows: CON, NUL, COM1, ...; "~" because of 8.3 short names like
    // SYNCFR~1.LUA, which are another name for an existing file).
    static bool isValidName(const QString& name);
    static constexpr int kMaxNameLength = 100;
    // Names are compared ignoring case: on Windows and macOS foo.lua and
    // Foo.lua are the same file. Returns the script (or file) that `name`
    // collides with, or an empty string. self: the script being saved.
    QString nameConflict(const QString& name, const LuaContainer* self = nullptr) const;

    // File contents <-> code in memory. Always UTF-8 (a BOM is dropped; old
    // files that are not UTF-8 still load). crlf (Windows, as earlier versions
    // did): CRLF on disk, LF in memory. Only CRLF pairs are converted, so any
    // code comes back unchanged after a save, which the approval hash needs.
#ifdef _WIN32
    static constexpr bool kCrlfOnDisk = true;
#else
    static constexpr bool kCrlfOnDisk = false;
#endif
    static QString decodeScript(const QByteArray& file, bool crlf = kCrlfOnDisk);
    static QByteArray encodeScript(const QString& code, bool crlf = kCrlfOnDisk);

    bool loadAll();
    bool saveAll();
    // Only the .ini (triggers, last runs); the code stays as it is on disk.
    bool saveSettings(LuaContainer* container);
    // Moves the .lua and .ini files. False if the script file could not be
    // renamed.
    bool rename(const QString& oldName, const QString& newName);

    bool itemByName(const QString& name, LuaContainer*& container);
    bool itemByName(const std::string& name, LuaContainer*& container);

    LuaContainerList::const_iterator begin();
    LuaContainerList::const_iterator end();

    LuaContainer* createItem();

    void addItem(LuaContainer* container);
    bool addItemAndSave(LuaContainer* container);
    bool save(LuaContainer* container);
    void removeItem(LuaContainer* container);
    // Deletes the files, then removes and deletes the container. If the file
    // cannot be deleted, nothing changes and the result is false.
    bool removeItemAndDelete(LuaContainer* container);
    // Takes back a new script whose first save failed (files and container).
    void discard(LuaContainer* container);

    size_t size();
    void sort();

    void setFilePath(const std::string& path);

    // debug

private:
    bool load(const QString& name, LuaContainer* container, bool ignoreNoSettingsFile = true);
    bool remove(LuaContainer* container);

    void getFileNames(const QString& name, QString& luaFileName, QString& settingsFileName);

    void clearList();

    LuaContainerList _luaList;
    QString _filePath;
};

#endif // LUALIST_H
