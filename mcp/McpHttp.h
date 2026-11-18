#ifndef MCP_HTTP_H
#define MCP_HTTP_H

#include <utility>
#include <vector>

#include <QByteArray>

class McpProtocol;
class McpSettings;

// Streamable HTTP rules for the MCP endpoint, independent of the HTTP server.
struct McpHttpRequest
{
    QByteArray method;
    QByteArray authorization; // "Authorization" header
    QByteArray origin;        // "Origin" header, empty if absent
    QByteArray body;
};

struct McpHttpReply
{
    int status;
    QByteArray body;
    std::vector<std::pair<QByteArray, QByteArray>> headers;
};

// Order: Origin (against DNS rebinding: only local origins or none),
// token, method (POST only; no SSE stream, so GET/DELETE are 405).
// No CORS headers: browsers must not be able to call the endpoint.
McpHttpReply mcpHandleHttp(const McpHttpRequest& request, const McpSettings& settings,
                           const McpProtocol& protocol);

bool mcpIsLocalOrigin(const QByteArray& origin);

const int kMcpMaxBodyBytes = 1024 * 1024;

#endif // MCP_HTTP_H
