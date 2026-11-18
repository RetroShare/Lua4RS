#include "McpProtocol.h"

#include <exception>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>

namespace {
enum ErrorCode
{
    ParseError = -32700,
    InvalidRequest = -32600,
    MethodNotFound = -32601,
    InvalidParams = -32602,
    InternalError = -32603
};

QJsonObject makeError(int code, const QString& message)
{
    return QJsonObject{ { "code", code }, { "message", message } };
}

QJsonObject errorResponse(const QJsonValue& id, int code, const QString& message)
{
    return QJsonObject{ { "jsonrpc", "2.0" }, { "id", id }, { "error", makeError(code, message) } };
}

QByteArray toBytes(const QJsonValue& value)
{
    if(value.isArray()) return QJsonDocument(value.toArray()).toJson(QJsonDocument::Compact);
    return QJsonDocument(value.toObject()).toJson(QJsonDocument::Compact);
}
}

McpProtocol::McpProtocol(const QString& serverName, const QString& serverVersion, const QString& instructions)
    : _name(serverName), _version(serverVersion), _instructions(instructions)
{}

const QStringList& McpProtocol::supportedVersions()
{
    static const QStringList versions = { "2025-11-25", "2025-06-18", "2025-03-26", "2024-11-05" };
    return versions;
}

void McpProtocol::addTool(const McpTool& tool)
{
    if(!_tools.count(tool.name)) _order << tool.name;
    _tools[tool.name] = tool;
}

McpProtocol::Reply McpProtocol::handle(const QByteArray& body) const
{
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(body, &parseError);
    if(parseError.error != QJsonParseError::NoError)
        return { 400, toBytes(errorResponse(QJsonValue::Null, ParseError, "Parse error: " + parseError.errorString())) };

    if(doc.isArray()) // JSON-RPC batch (allowed in 2025-03-26)
    {
        const QJsonArray batch = doc.array();
        if(batch.isEmpty())
            return { 400, toBytes(errorResponse(QJsonValue::Null, InvalidRequest, "Empty batch")) };
        QJsonArray responses;
        for(const QJsonValue& msg : batch)
        {
            QJsonObject response;
            if(handleMessage(msg, response)) responses.append(response);
        }
        if(responses.isEmpty()) return { 202, QByteArray() };
        return { 200, toBytes(responses) };
    }

    QJsonObject response;
    if(!handleMessage(doc.object(), response)) return { 202, QByteArray() };
    return { 200, toBytes(response) };
}

bool McpProtocol::handleMessage(const QJsonValue& value, QJsonObject& response) const
{
    if(!value.isObject())
    {
        response = errorResponse(QJsonValue::Null, InvalidRequest, "Message must be a JSON object");
        return true;
    }
    const QJsonObject msg = value.toObject();
    const bool hasId = msg.contains("id");
    const QJsonValue id = msg.value("id");

    // A response from the client (we never send requests): nothing to answer.
    if(!msg.contains("method") && (msg.contains("result") || msg.contains("error"))) return false;

    if(msg.value("jsonrpc").toString() != "2.0" || !msg.value("method").isString() ||
       (hasId && !id.isString() && !id.isDouble()))
    {
        if(!hasId) return false; // broken notification: nothing we can answer
        response = errorResponse(id, InvalidRequest, "Not a JSON-RPC 2.0 request");
        return true;
    }

    const QString method = msg.value("method").toString();
    if(!hasId) return false; // notification, e.g. notifications/initialized

    const QJsonObject params = msg.value("params").toObject();
    QJsonObject result;
    if(method == "initialize")
    {
        const QString requested = params.value("protocolVersion").toString();
        const QString version = supportedVersions().contains(requested) ? requested : supportedVersions().first();
        result = QJsonObject{
            { "protocolVersion", version },
            { "capabilities", QJsonObject{ { "tools", QJsonObject{ { "listChanged", false } } } } },
            { "serverInfo", QJsonObject{ { "name", _name }, { "version", _version } } },
            { "instructions", _instructions }
        };
    }
    else if(method == "ping") result = QJsonObject();
    else if(method == "tools/list") result = listTools();
    else if(method == "tools/call")
    {
        QJsonObject error;
        result = callTool(params, error);
        if(!error.isEmpty())
        {
            response = QJsonObject{ { "jsonrpc", "2.0" }, { "id", id }, { "error", error } };
            return true;
        }
    }
    else
    {
        response = errorResponse(id, MethodNotFound, "Method not found: " + method);
        return true;
    }
    response = QJsonObject{ { "jsonrpc", "2.0" }, { "id", id }, { "result", result } };
    return true;
}

QJsonObject McpProtocol::listTools() const
{
    QJsonArray tools;
    for(const QString& name : _order)
    {
        const McpTool& tool = _tools.at(name);
        tools.append(QJsonObject{
            { "name", tool.name },
            { "description", tool.description },
            { "inputSchema", tool.inputSchema },
            { "annotations", QJsonObject{ { "readOnlyHint", tool.readOnly },
                                          { "destructiveHint", tool.destructive },
                                          { "openWorldHint", false } } }
        });
    }
    return QJsonObject{ { "tools", tools } };
}

QJsonObject McpProtocol::callTool(const QJsonObject& params, QJsonObject& error) const
{
    const QString name = params.value("name").toString();
    const auto it = _tools.find(name);
    if(it == _tools.end())
    {
        error = makeError(InvalidParams, "Unknown tool: " + name);
        return QJsonObject();
    }
    const QJsonValue args = params.value("arguments");
    if(!args.isUndefined() && !args.isNull() && !args.isObject())
    {
        error = makeError(InvalidParams, "arguments must be an object");
        return QJsonObject();
    }

    McpResult result;
    try { result = it->second.call(args.toObject()); }
    catch(const std::exception& e) { result = McpResult::fail(QString("Internal error: ") + e.what()); }
    catch(...) { result = McpResult::fail("Internal error"); }

    if(result.failed())
        return QJsonObject{
            { "content", QJsonArray{ QJsonObject{ { "type", "text" }, { "text", result.error } } } },
            { "isError", true }
        };
    return QJsonObject{
        { "content", QJsonArray{ QJsonObject{ { "type", "text" }, { "text", QString::fromUtf8(toBytes(result.data)) } } } },
        { "structuredContent", result.data },
        { "isError", false }
    };
}

bool McpArgs::string(const QJsonObject& args, const char* key, QString& value, QString& error)
{
    const QJsonValue v = args.value(key);
    if(!v.isString()) { error = QString("Missing or invalid argument '%1' (string expected)").arg(key); return false; }
    value = v.toString();
    return true;
}

bool McpArgs::optionalString(const QJsonObject& args, const char* key, QString& value, QString& error)
{
    const QJsonValue v = args.value(key);
    if(v.isUndefined() || v.isNull()) { value.clear(); return true; }
    if(!v.isString()) { error = QString("Invalid argument '%1' (string expected)").arg(key); return false; }
    value = v.toString();
    return true;
}
