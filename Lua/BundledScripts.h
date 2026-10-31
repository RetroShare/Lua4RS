#pragma once
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStringList>

namespace BundledScripts {
inline QStringList names()
{
    return QDir(":/lua4rs/examples").entryList({"*.lua"}, QDir::Files, QDir::Name);
}

// Once per profile. Never replace user files or resurrect deleted examples.
inline bool install(const QString& path, QString& error)
{
    QDir directory(path);
    if(directory.exists(".bundled-examples-installed")) return true;
    if(!QDir().mkpath(path)) { error = "Cannot create script directory: " + path; return false; }
    const auto scripts = names();
    if(scripts.isEmpty()) { error = "Bundled script resources are missing"; return false; }
    for(const QString& name : scripts)
    {
        const QString target = directory.filePath(name);
        // Preserve both user code and any settings left by a deleted script.
        if(QFileInfo::exists(target) || QFileInfo::exists(directory.filePath(name + ".ini"))) continue;
        QFile source(":/lua4rs/examples/" + name);
        if(!source.copy(target)) { error = "Cannot install example: " + target; return false; }
        // Resource copies inherit read-only permissions; scripts must be editable.
        if(!QFile::setPermissions(target, QFile::permissions(target) | QFileDevice::WriteOwner))
        { error = "Cannot make example writable: " + target; return false; }
    }
    QSaveFile marker(directory.filePath(".bundled-examples-installed"));
    if(!marker.open(QIODevice::WriteOnly) || marker.write("1\n") != 2 || !marker.commit())
    { error = "Cannot record example installation in " + path; return false; }
    return true;
}
}
