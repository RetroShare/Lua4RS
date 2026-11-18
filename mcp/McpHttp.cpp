#include "McpHttp.h"
#include "McpProtocol.h"
#include "McpSettings.h"

#include <QUrl>

namespace {
McpHttpReply text(int status, const QByteArray& message)
{
    return { status, message, { { "Content-Type", "text/plain; charset=utf-8" } } };
}
}

bool mcpIsLocalOrigin(const QByteArray& origin)
{
    const QUrl url(QString::fromLatin1(origin));
    if(!url.isValid() || (url.scheme() != "http" && url.scheme() != "https")) return false;
    const QString host = url.host().toLower();
    return host == "localhost" || host == "127.0.0.1" || host == "::1";
}

McpHttpReply mcpHandleHttp(const McpHttpRequest& request, const McpSettings& settings,
                           const McpProtocol& protocol)
{
    if(!request.origin.isEmpty() && !mcpIsLocalOrigin(request.origin))
        return text(403, "Origin not allowed");

    const QByteArray prefix = "Bearer ";
    if(!request.authorization.startsWith(prefix) ||
       !settings.checkToken(request.authorization.mid(prefix.size()).trimmed()))
    {
        McpHttpReply reply = text(401, "Missing or invalid MCP token");
        reply.headers.push_back({ "WWW-Authenticate", "Bearer realm=\"lua4rs\"" });
        return reply;
    }

    if(request.method != "POST")
    {
        McpHttpReply reply = text(405, "Only POST is supported (no SSE stream)");
        reply.headers.push_back({ "Allow", "POST" });
        return reply;
    }

    if(request.body.size() > kMcpMaxBodyBytes) return text(413, "Request too large");

    const McpProtocol::Reply reply = protocol.handle(request.body);
    if(reply.body.isEmpty()) return { reply.httpStatus, QByteArray(), {} };
    return { reply.httpStatus, reply.body, { { "Content-Type", "application/json" } } };
}
