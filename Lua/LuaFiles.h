#ifndef LUAFILES_H
#define LUAFILES_H

#include <QFile>
#include <QString>

namespace LuaFiles {

// Script names are compared ignoring case: on Windows and macOS foo.lua and
// Foo.lua are the same file.
inline bool sameName(const QString& a, const QString& b)
{
    return a.compare(b, Qt::CaseInsensitive) == 0;
}

// Renames a file. replace: an existing file `to` is removed first. A rename
// that only changes the case goes through a temporary name, because on
// case-insensitive file systems both names are the same file (removing `to`
// would remove `from`).
inline bool rename(const QString& from, const QString& to, bool replace)
{
    if(from == to) return true;
    if(!sameName(from, to))
    {
        if(replace && QFile::exists(to)) QFile::remove(to);
        return QFile::rename(from, to);
    }
    const QString temporary = from + ".renaming";
    QFile::remove(temporary);
    if(!QFile::rename(from, temporary)) return false;
    // Case-sensitive file systems: `to` can be a different file.
    if(replace && QFile::exists(to)) QFile::remove(to);
    if(QFile::rename(temporary, to)) return true;
    QFile::rename(temporary, from);
    return false;
}

} // namespace LuaFiles

#endif // LUAFILES_H
