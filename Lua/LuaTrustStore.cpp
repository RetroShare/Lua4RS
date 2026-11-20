#include "LuaTrustStore.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QSaveFile>

namespace {
const std::string kMigratedKey = "L4R_TRUST_MIGRATED";
const std::string kEntryPrefix = "L4R_TRUST:";

bool parseLevel(const QByteArray& text, LuaTrust& level)
{
    for(LuaTrust candidate : { LuaTrust::SandboxReadOnly, LuaTrust::SandboxWrite, LuaTrust::Trusted })
        if(text == luaTrustName(candidate)) { level = candidate; return true; }
    return false;
}
}

void LuaTrustStore::setScriptDirectory(const QString& dir)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _dir = dir;
}

void LuaTrustStore::setBundledSource(BundledSource source)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _bundled = std::move(source);
}

void LuaTrustStore::setChangedCallback(std::function<void()> callback)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _changed = std::move(callback);
}

QByteArray LuaTrustStore::hash(const QString& code)
{
    return QCryptographicHash::hash(code.toUtf8(), QCryptographicHash::Sha256).toHex();
}

QString LuaTrustStore::approvedCopyPath(const QString& name) const
{
    return QDir(_dir).filePath(name + ".approved");
}

bool LuaTrustStore::approvedCode(const QString& name, QString& code) const
{
    QByteArray expected;
    QString path;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        const auto it = _entries.find(name);
        if(it == _entries.end())
        {
            QByteArray shipped;
            if(!_bundled || !_bundled(name, shipped)) return false;
            code = QString::fromUtf8(shipped);
            return true;
        }
        if(_dir.isEmpty()) return false;
        expected = it->second.sha256;
        path = approvedCopyPath(name);
    }
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) return false;
    const QByteArray utf8 = file.readAll();
    if(QCryptographicHash::hash(utf8, QCryptographicHash::Sha256).toHex() != expected) return false;
    code = QString::fromUtf8(utf8);
    return true;
}

LuaTrustStore::Status LuaTrustStore::statusLocked(const QString& name, const QByteArray& utf8) const
{
    const auto it = _entries.find(name);
    if(it != _entries.end())
    {
        const QByteArray current = QCryptographicHash::hash(utf8, QCryptographicHash::Sha256).toHex();
        if(current == it->second.sha256)
            return { State::Approved, it->second.level, it->second.level, it->second.approvedAt };
        return { State::Modified, it->second.level, LuaTrust::SandboxReadOnly, it->second.approvedAt };
    }
    QByteArray shipped;
    if(_bundled && _bundled(name, shipped))
    {
        const State state = shipped == utf8 ? State::Bundled : State::ChangedExample;
        return { state, LuaTrust::SandboxReadOnly, LuaTrust::SandboxReadOnly, {} };
    }
    return { State::Unknown, LuaTrust::SandboxReadOnly, LuaTrust::SandboxReadOnly, {} };
}

LuaTrustStore::Status LuaTrustStore::status(const QString& name, const QString& code) const
{
    const QByteArray utf8 = code.toUtf8();
    std::lock_guard<std::mutex> lock(_mutex);
    return statusLocked(name, utf8);
}

void LuaTrustStore::setEntryLocked(const QString& name, const QByteArray& utf8, LuaTrust level)
{
    _entries[name] = { level, QCryptographicHash::hash(utf8, QCryptographicHash::Sha256).toHex(),
                       QDateTime::currentDateTimeUtc() };
    writeApprovedCopy(name, utf8);
}

// Copy of the approved code for the diff in the approval dialog. Not trusted
// by itself: whoever shows it must check it against the stored hash.
void LuaTrustStore::writeApprovedCopy(const QString& name, const QByteArray& utf8) const
{
    if(_dir.isEmpty()) return;
    QSaveFile file(approvedCopyPath(name));
    if(file.open(QIODevice::WriteOnly) && file.write(utf8) == utf8.size()) file.commit();
}

void LuaTrustStore::approve(const QString& name, const QString& code, LuaTrust level)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        setEntryLocked(name, code.toUtf8(), level);
    }
    notifyChanged();
}

void LuaTrustStore::guiSaved(const QString& oldName, const QString& oldCode,
                             const QString& newName, const QString& newCode, bool createdInGui)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        const QByteArray newUtf8 = newCode.toUtf8();
        if(createdInGui)
            setEntryLocked(newName, newUtf8, LuaTrust::Trusted);
        else
        {
            const Status before = statusLocked(oldName, oldCode.toUtf8());
            const bool renamed = oldName != newName;
            auto it = _entries.find(oldName);
            if(before.state == State::Approved)
            {
                // The user edited an approved version: their edit inherits it.
                if(it != _entries.end() && renamed) _entries.erase(it);
                if(renamed && !_dir.isEmpty()) QFile::remove(approvedCopyPath(oldName));
                setEntryLocked(newName, newUtf8, before.approvedLevel);
            }
            else if(renamed && it != _entries.end())
            {
                // Not approved: move the old (non-matching) entry and its copy
                // so the dialog can still show what was approved.
                Entry moved = it->second;
                _entries.erase(it);
                _entries[newName] = moved;
                if(!_dir.isEmpty())
                {
                    QFile::remove(approvedCopyPath(newName));
                    QFile::rename(approvedCopyPath(oldName), approvedCopyPath(newName));
                }
            }
        }
    }
    notifyChanged();
}

void LuaTrustStore::remove(const QString& name)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _entries.erase(name);
        if(!_dir.isEmpty()) QFile::remove(approvedCopyPath(name));
    }
    notifyChanged();
}

bool LuaTrustStore::migrated() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _migrated;
}

void LuaTrustStore::setMigrated()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _migrated = true;
    }
    notifyChanged();
}

std::vector<std::pair<std::string, std::string>> LuaTrustStore::toKeyValues() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    std::vector<std::pair<std::string, std::string>> result;
    if(_migrated) result.emplace_back(kMigratedKey, "1");
    for(const auto& entry : _entries)
    {
        const QByteArray value = QByteArray(luaTrustName(entry.second.level)) + ';' +
            entry.second.sha256 + ';' + entry.second.approvedAt.toString(Qt::ISODate).toUtf8();
        result.emplace_back(kEntryPrefix + entry.first.toUtf8().toStdString(), value.toStdString());
    }
    return result;
}

bool LuaTrustStore::fromKeyValue(const std::string& key, const std::string& value)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(key == kMigratedKey) { _migrated = value == "1"; return true; }
    if(key.compare(0, kEntryPrefix.size(), kEntryPrefix) != 0) return false;

    const QString name = QString::fromUtf8(key.substr(kEntryPrefix.size()).c_str());
    const QList<QByteArray> parts = QByteArray::fromStdString(value).split(';');
    Entry entry;
    if(name.isEmpty() || parts.size() != 3 || !parseLevel(parts[0], entry.level) || parts[1].size() != 64)
        return true; // ours, but malformed: ignore -> script stays sandboxed
    entry.sha256 = parts[1];
    entry.approvedAt = QDateTime::fromString(QString::fromUtf8(parts[2]), Qt::ISODate);
    _entries[name] = entry;
    return true;
}

void LuaTrustStore::clear()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _entries.clear();
    _migrated = false;
}

void LuaTrustStore::notifyChanged() const
{
    std::function<void()> callback;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        callback = _changed;
    }
    if(callback) callback();
}
