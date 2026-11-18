#include "McpSettings.h"

#include <QRandomGenerator>

namespace {
const std::string kEnabledKey = "L4R_MCP_ENABLED";
const std::string kTokenKey = "L4R_MCP_TOKEN";
}

QString McpSettings::newToken()
{
    QByteArray bytes(32, '\0');
    QRandomGenerator* random = QRandomGenerator::system();
    for(char& byte : bytes) byte = static_cast<char>(random->bounded(256));
    return QString::fromLatin1(bytes.toHex());
}

bool McpSettings::enabled() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _enabled;
}

void McpSettings::setEnabled(bool enabled)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if(_enabled == enabled && (!enabled || !_token.isEmpty())) return;
        _enabled = enabled;
        if(enabled && _token.isEmpty()) _token = newToken();
    }
    notifyChanged();
}

QString McpSettings::token() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _token;
}

QString McpSettings::regenerateToken()
{
    QString token = newToken();
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _token = token;
    }
    notifyChanged();
    return token;
}

bool McpSettings::checkToken(const QByteArray& presented) const
{
    QByteArray expected;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if(!_enabled || _token.isEmpty()) return false;
        expected = _token.toLatin1();
    }
    // Compare in constant time (length is not secret: always 64 hex chars).
    if(presented.size() != expected.size()) return false;
    unsigned char diff = 0;
    for(int i = 0; i < expected.size(); ++i)
        diff |= static_cast<unsigned char>(presented[i] ^ expected[i]);
    return diff == 0;
}

void McpSettings::setChangedCallback(std::function<void()> callback)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _changed = std::move(callback);
}

void McpSettings::notifyChanged() const
{
    std::function<void()> changed;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        changed = _changed;
    }
    if(changed) changed();
}

std::vector<std::pair<std::string, std::string>> McpSettings::toKeyValues() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    std::vector<std::pair<std::string, std::string>> result;
    result.emplace_back(kEnabledKey, _enabled ? "1" : "0");
    if(!_token.isEmpty()) result.emplace_back(kTokenKey, _token.toStdString());
    return result;
}

bool McpSettings::fromKeyValue(const std::string& key, const std::string& value)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(key == kEnabledKey) { _enabled = value == "1"; return true; }
    if(key == kTokenKey)
    {
        const QString token = QString::fromStdString(value);
        // Only well-formed tokens; anything else leaves the endpoint locked.
        static const QString hex = "0123456789abcdef";
        bool valid = token.size() == 64;
        for(const QChar c : token) valid = valid && hex.contains(c);
        _token = valid ? token : QString();
        return true;
    }
    return false;
}
