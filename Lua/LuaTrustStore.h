#ifndef LUA_TRUST_STORE_H
#define LUA_TRUST_STORE_H

#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include <QByteArray>
#include <QDateTime>
#include <QString>

#include "LuaTrust.h"

// Approvals of stored scripts: trust level + SHA-256 of the approved code.
//
// Persisted by p3Lua4RS in RetroShare's signed + encrypted config, never in
// files next to the script. Rule: the stored level only applies while the
// script's code hashes to the approved hash; otherwise it is sandbox-readonly,
// no matter who changed it (MCP, external editor, shell).
//
// Thread-safe: queried by the Lua worker, changed by the GUI, serialised by
// RetroShare's config thread.
class LuaTrustStore
{
public:
    enum class State
    {
        Approved, // entry exists and the code matches it
        Bundled,  // no entry, but byte-identical to a script shipped with the plugin
        Modified, // entry exists, code changed since approval
        ChangedExample, // no entry, differs from the shipped script of that name
        Unknown   // no entry (e.g. file dropped into the folder)
    };

    struct Status
    {
        State state;
        LuaTrust approvedLevel; // level of the entry (Modified/Approved), Trusted for examples
        LuaTrust effective;     // what the script runs with now
        QDateTime approvedAt;   // UTC, valid for Approved/Modified
    };

    // shipped(name, code): true and the code of the script of that name shipped
    // with the plugin, false if there is none
    using BundledSource = std::function<bool(const QString&, QByteArray&)>;

    void setScriptDirectory(const QString& dir); // for <name>.approved copies
    void setBundledSource(BundledSource source);
    void setChangedCallback(std::function<void()> callback); // -> IndicateConfigChanged

    Status status(const QString& name, const QString& code) const;
    LuaTrust effective(const QString& name, const QString& code) const
    { return status(name, code).effective; }

    // Explicit approval (dialog, migration): sets level and hash.
    void approve(const QString& name, const QString& code, LuaTrust level);

    // Saving in the GUI. Keeps the level and takes over the new hash only if
    // the code being edited (oldCode, the stored version) was approved/bundled;
    // a new script created in the GUI becomes trusted. Never raises a level.
    void guiSaved(const QString& oldName, const QString& oldCode,
                  const QString& newName, const QString& newCode, bool createdInGui);

    void remove(const QString& name);

    // Trust for "Run" in the editor: drafts are the user's own code; otherwise
    // the editor inherits the level of the stored version if that is approved.
    LuaTrust forEditor(const QString& name, const QString& storedCode, bool draft) const
    { return draft ? LuaTrust::Trusted : effective(name, storedCode); }

    // One-time migration from versions without approvals. Not stored in any
    // file next to the scripts, so deleting the config asks again.
    bool migrated() const;
    void setMigrated();

    // Serialisation as key/value pairs (UTF-8).
    std::vector<std::pair<std::string, std::string>> toKeyValues() const;
    bool fromKeyValue(const std::string& key, const std::string& value); // false: not ours
    void clear();

    static QByteArray hash(const QString& code);   // hex SHA-256 of UTF-8 code
    QString approvedCopyPath(const QString& name) const;

    // Code of the approved version, read from <name>.approved. Only succeeds
    // if the copy still hashes to the stored approval (the file itself can be
    // changed by anyone with access to the folder). Without an entry, the
    // shipped example of that name counts as the approved version.
    bool approvedCode(const QString& name, QString& code) const;

private:
    struct Entry
    {
        LuaTrust level;
        QByteArray sha256;
        QDateTime approvedAt;
    };

    Status statusLocked(const QString& name, const QByteArray& utf8) const;
    void setEntryLocked(const QString& name, const QByteArray& utf8, LuaTrust level);
    void writeApprovedCopy(const QString& name, const QByteArray& utf8) const;
    void notifyChanged() const;

    mutable std::mutex _mutex;
    std::map<QString, Entry> _entries;
    bool _migrated = false;
    QString _dir;
    BundledSource _bundled;
    std::function<void()> _changed;
};

#endif // LUA_TRUST_STORE_H
