#ifndef MCP_SETTINGS_H
#define MCP_SETTINGS_H

#include <functional>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include <QByteArray>
#include <QString>

// MCP endpoint settings, persisted by p3Lua4RS in RetroShare's signed and
// encrypted config. The token is the endpoint's own credential: it is not a
// JSON API token and grants nothing outside /lua4rs/mcp.
class McpSettings
{
public:
    bool enabled() const;
    void setEnabled(bool enabled);           // creates a token on first enable

    QString token() const;                   // empty until first enabled
    QString regenerateToken();               // invalidates the old token
    bool checkToken(const QByteArray& presented) const; // constant time

    void setChangedCallback(std::function<void()> callback);

    std::vector<std::pair<std::string, std::string>> toKeyValues() const;
    bool fromKeyValue(const std::string& key, const std::string& value); // false: not ours

    static QString newToken();               // 256 bit, hex

private:
    void notifyChanged() const;

    mutable std::mutex _mutex;
    bool _enabled = false;
    QString _token;
    std::function<void()> _changed;
};

#endif // MCP_SETTINGS_H
