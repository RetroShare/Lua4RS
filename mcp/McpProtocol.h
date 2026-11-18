#ifndef MCP_PROTOCOL_H
#define MCP_PROTOCOL_H

#include <functional>
#include <map>

#include <QByteArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QStringList>

// Result of a tool call: data on success, otherwise an error message the
// model can act on (reported as a tool error, not as a protocol error).
struct McpResult
{
    QJsonObject data;
    QString error;

    static McpResult ok(const QJsonObject& data) { return { data, QString() }; }
    static McpResult fail(const QString& error) { return { QJsonObject(), error }; }
    bool failed() const { return !error.isEmpty(); }
};

struct McpTool
{
    QString name;
    QString description;
    QJsonObject inputSchema;   // JSON Schema of the arguments object
    bool readOnly = true;      // annotations: readOnlyHint
    bool destructive = false;  // annotations: destructiveHint
    std::function<McpResult(const QJsonObject& arguments)> call;
};

// MCP over JSON-RPC 2.0, without transport: Streamable HTTP with JSON
// responses only (no SSE, no sessions). Supports initialize, ping,
// tools/list, tools/call and ignores notifications.
class McpProtocol
{
public:
    struct Reply
    {
        int httpStatus;   // 200, 202 (only notifications/responses) or 400
        QByteArray body;  // empty for 202
    };

    McpProtocol(const QString& serverName, const QString& serverVersion, const QString& instructions);

    void addTool(const McpTool& tool);
    Reply handle(const QByteArray& body) const;

    static const QStringList& supportedVersions(); // newest first

private:
    // returns false if msg needs no response (notification or response)
    bool handleMessage(const QJsonValue& msg, QJsonObject& response) const;
    QJsonObject callTool(const QJsonObject& params, QJsonObject& error) const;
    QJsonObject listTools() const;

    QString _name, _version, _instructions;
    std::map<QString, McpTool> _tools;
    QStringList _order;
};

// Helpers for tool implementations
namespace McpArgs {
// Required string argument; sets error if missing or not a string.
bool string(const QJsonObject& args, const char* key, QString& value, QString& error);
// Optional string argument (empty if absent); error if present but no string.
bool optionalString(const QJsonObject& args, const char* key, QString& value, QString& error);
}

#endif // MCP_PROTOCOL_H
