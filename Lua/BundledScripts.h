#pragma once
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStringList>

// Scripts compiled into the plugin (scripts/scripts.qrc), one resource folder
// per group. Names are unique across groups.
namespace BundledScripts {
struct Group
{
    const char* folder; // resource folder below :/lua4rs/scripts
    const char* title;  // menu title, translated by the GUI (context "Lua4RSWidget")
};

// In menu order.
inline const QList<Group>& groups()
{
    static const QList<Group> list = {
        { "ready", "Ready to use" },
        { "examples", "Examples" },
        { "trusted", "Trusted only" },
    };
    return list;
}

inline QString folderPath(const char* folder)
{
    return QStringLiteral(":/lua4rs/scripts/") + QLatin1String(folder);
}

inline QStringList names(const char* folder)
{
    return QDir(folderPath(folder)).entryList({"*.lua"}, QDir::Files, QDir::Name);
}

// Resource path of a bundled script, empty if there is none of that name.
inline QString resourcePath(const QString& name)
{
    for(const Group& group : groups())
    {
        const QString path = folderPath(group.folder) + "/" + name;
        if(!name.contains('/') && QFileInfo::exists(path)) return path;
    }
    return {};
}

inline bool read(const QString& name, QByteArray& code)
{
    const QString path = resourcePath(name);
    if(path.isEmpty()) return false;
    QFile source(path);
    if(!source.open(QIODevice::ReadOnly)) return false;
    code = source.readAll();
    return true;
}

// The leading comment block (up to the first empty "--" line), for tooltips.
inline QString summary(const QByteArray& code)
{
    QStringList lines;
    for(const QByteArray& raw : code.split('\n'))
    {
        const QString line = QString::fromUtf8(raw).trimmed();
        if(!line.startsWith("--")) break;
        const QString text = line.mid(2).trimmed();
        if(text.isEmpty()) break;
        lines << text;
    }
    return lines.join(' ');
}

// Installs the "examples" group, once per profile. Never replaces user files
// or resurrects deleted examples.
inline bool install(const QString& path, QString& error)
{
    QDir directory(path);
    if(directory.exists(".bundled-examples-installed")) return true;
    if(!QDir().mkpath(path)) { error = "Cannot create script directory: " + path; return false; }
    const auto scripts = names("examples");
    if(scripts.isEmpty()) { error = "Bundled script resources are missing"; return false; }
    for(const QString& name : scripts)
    {
        const QString target = directory.filePath(name);
        // Preserve both user code and any settings left by a deleted script.
        if(QFileInfo::exists(target) || QFileInfo::exists(directory.filePath(name + ".ini"))) continue;
        QFile source(folderPath("examples") + "/" + name);
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
