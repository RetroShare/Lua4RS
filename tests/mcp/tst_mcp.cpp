// Tests for the MCP protocol, HTTP rules and settings (no RetroShare needed).
// Build: qmake mcp.pro && make && ./tst_mcp
#include <cstdio>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "McpHttp.h"
#include "McpProtocol.h"
#include "McpSettings.h"

namespace {
int failures = 0;
int checks = 0;

void check(bool condition, const char* what, int line)
{
    ++checks;
    if(!condition) { ++failures; std::printf("FAIL  line %d: %s\n", line, what); }
}
#define CHECK(cond) check((cond), #cond, __LINE__)

QJsonObject obj(const QByteArray& json) { return QJsonDocument::fromJson(json).object(); }

McpProtocol makeProtocol()
{
    McpProtocol p("lua4rs", "1.0", "Be nice.");
    McpTool echo;
    echo.name = "echo";
    echo.description = "Echo text";
    echo.inputSchema = QJsonObject{ { "type", "object" } };
    echo.call = [](const QJsonObject& args) {
        QString text, error;
        if(!McpArgs::string(args, "text", text, error)) return McpResult::fail(error);
        return McpResult::ok(QJsonObject{ { "text", text } });
    };
    p.addTool(echo);
    McpTool boom;
    boom.name = "boom";
    boom.readOnly = false;
    boom.title = "Boom";
    boom.destructive = true;
    boom.idempotent = false;
    boom.openWorld = true;
    boom.call = [](const QJsonObject&) -> McpResult { throw std::runtime_error("kaputt"); };
    p.addTool(boom);
    return p;
}

void initialize()
{
    const McpProtocol p = makeProtocol();
    auto r = p.handle(R"({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-06-18","capabilities":{},"clientInfo":{"name":"t","version":"1"}}})");
    CHECK(r.httpStatus == 200);
    const QJsonObject res = obj(r.body).value("result").toObject();
    CHECK(obj(r.body).value("id").toInt() == 1);
    CHECK(res.value("protocolVersion").toString() == "2025-06-18");
    CHECK(res.value("serverInfo").toObject().value("name").toString() == "lua4rs");
    CHECK(res.value("capabilities").toObject().contains("tools"));
    CHECK(res.value("instructions").toString() == "Be nice.");
    // unknown version -> newest supported
    r = p.handle(R"({"jsonrpc":"2.0","id":"a","method":"initialize","params":{"protocolVersion":"1999-01-01"}})");
    CHECK(obj(r.body).value("result").toObject().value("protocolVersion").toString() == McpProtocol::supportedVersions().first());
    CHECK(obj(r.body).value("id").toString() == "a");
}

void notificationsAndPing()
{
    const McpProtocol p = makeProtocol();
    auto r = p.handle(R"({"jsonrpc":"2.0","method":"notifications/initialized"})");
    CHECK(r.httpStatus == 202 && r.body.isEmpty());
    r = p.handle(R"({"jsonrpc":"2.0","id":7,"result":{}})"); // client response
    CHECK(r.httpStatus == 202);
    r = p.handle(R"({"jsonrpc":"2.0","id":2,"method":"ping"})");
    CHECK(r.httpStatus == 200 && obj(r.body).value("result").isObject());
}

void errors()
{
    const McpProtocol p = makeProtocol();
    auto r = p.handle("{not json");
    CHECK(r.httpStatus == 400 && obj(r.body).value("error").toObject().value("code").toInt() == -32700);
    r = p.handle(R"({"jsonrpc":"2.0","id":3,"method":"nope"})");
    CHECK(obj(r.body).value("error").toObject().value("code").toInt() == -32601);
    r = p.handle(R"({"jsonrpc":"1.0","id":4,"method":"ping"})");
    CHECK(obj(r.body).value("error").toObject().value("code").toInt() == -32600);
    r = p.handle(R"({"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"missing"}})");
    CHECK(obj(r.body).value("error").toObject().value("code").toInt() == -32602);
    r = p.handle(R"({"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"echo","arguments":[1]}})");
    CHECK(obj(r.body).value("error").toObject().value("code").toInt() == -32602);
}

void tools()
{
    const McpProtocol p = makeProtocol();
    auto r = p.handle(R"({"jsonrpc":"2.0","id":1,"method":"tools/list"})");
    const QJsonArray list = obj(r.body).value("result").toObject().value("tools").toArray();
    CHECK(list.size() == 2);
    CHECK(list[0].toObject().value("name").toString() == "echo"); // registration order
    const QJsonObject echoAnn = list[0].toObject().value("annotations").toObject();
    CHECK(echoAnn.value("readOnlyHint").toBool());
    // MCP defaults are destructive/open-world: the hints must be sent explicitly
    CHECK(echoAnn.contains("destructiveHint") && !echoAnn.value("destructiveHint").toBool());
    CHECK(echoAnn.contains("openWorldHint") && !echoAnn.value("openWorldHint").toBool());
    CHECK(echoAnn.value("idempotentHint").toBool());
    CHECK(!list[0].toObject().contains("title") && !echoAnn.contains("title"));
    const QJsonObject boomAnn = list[1].toObject().value("annotations").toObject();
    CHECK(!boomAnn.value("readOnlyHint").toBool());
    CHECK(boomAnn.value("destructiveHint").toBool());
    CHECK(!boomAnn.value("idempotentHint").toBool());
    CHECK(boomAnn.value("openWorldHint").toBool());
    CHECK(list[1].toObject().value("title").toString() == "Boom");
    CHECK(boomAnn.value("title").toString() == "Boom");

    r = p.handle(R"({"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"echo","arguments":{"text":"hi"}}})");
    QJsonObject res = obj(r.body).value("result").toObject();
    CHECK(!res.value("isError").toBool());
    CHECK(res.value("structuredContent").toObject().value("text").toString() == "hi");
    CHECK(obj(res.value("content").toArray()[0].toObject().value("text").toString().toUtf8()).value("text").toString() == "hi");

    // bad arguments and exceptions are tool errors the model can see
    r = p.handle(R"({"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"echo","arguments":{}}})");
    res = obj(r.body).value("result").toObject();
    CHECK(res.value("isError").toBool());
    CHECK(res.value("content").toArray()[0].toObject().value("text").toString().contains("text"));
    r = p.handle(R"({"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"boom"}})");
    res = obj(r.body).value("result").toObject();
    CHECK(res.value("isError").toBool());
    CHECK(res.value("content").toArray()[0].toObject().value("text").toString().contains("kaputt"));
}

void batch()
{
    const McpProtocol p = makeProtocol();
    auto r = p.handle(R"([{"jsonrpc":"2.0","id":1,"method":"ping"},{"jsonrpc":"2.0","method":"notifications/initialized"}])");
    CHECK(r.httpStatus == 200);
    CHECK(QJsonDocument::fromJson(r.body).array().size() == 1);
    r = p.handle(R"([{"jsonrpc":"2.0","method":"notifications/initialized"}])");
    CHECK(r.httpStatus == 202);
    r = p.handle("[]");
    CHECK(r.httpStatus == 400);
}

void settings()
{
    McpSettings s;
    int changes = 0;
    s.setChangedCallback([&changes]() { ++changes; });
    CHECK(!s.enabled() && s.token().isEmpty());
    CHECK(!s.checkToken(""));
    s.setEnabled(true);
    const QString token = s.token();
    CHECK(token.size() == 64);
    CHECK(s.checkToken(token.toLatin1()));
    CHECK(!s.checkToken(token.left(63).toLatin1()));
    CHECK(!s.checkToken(QString(token).replace(0, 1, token[0] == 'a' ? "b" : "a").toLatin1()));
    s.setEnabled(false);
    CHECK(!s.checkToken(token.toLatin1())); // disabled endpoint accepts nothing
    s.setEnabled(true);
    CHECK(s.token() == token);              // re-enabling keeps the token
    const QString fresh = s.regenerateToken();
    CHECK(fresh != token && !s.checkToken(token.toLatin1()) && s.checkToken(fresh.toLatin1()));
    CHECK(changes == 4);

    McpSettings b;
    for(const auto& kv : s.toKeyValues()) CHECK(b.fromKeyValue(kv.first, kv.second));
    CHECK(b.enabled() && b.token() == fresh);
    CHECK(!b.fromKeyValue("L4R_TRUST_MIGRATED", "1"));
    CHECK(b.fromKeyValue("L4R_MCP_TOKEN", "short"));
    CHECK(b.token().isEmpty() && !b.checkToken("short"));
    CHECK(changes == 4); // loading does not mark the config dirty
}

void http()
{
    const McpProtocol p = makeProtocol();
    McpSettings s;
    s.setEnabled(true);
    const QByteArray auth = "Bearer " + s.token().toLatin1();
    const QByteArray ping = R"({"jsonrpc":"2.0","id":1,"method":"ping"})";

    CHECK(mcpHandleHttp({ "POST", auth, "", ping }, s, p).status == 200);
    CHECK(mcpHandleHttp({ "POST", auth, "http://localhost:6274", ping }, s, p).status == 200);
    CHECK(mcpHandleHttp({ "POST", auth, "http://127.0.0.1", ping }, s, p).status == 200);
    CHECK(mcpHandleHttp({ "POST", auth, "http://[::1]:80", ping }, s, p).status == 200);
    CHECK(mcpHandleHttp({ "POST", auth, "http://evil.example", ping }, s, p).status == 403);
    CHECK(mcpHandleHttp({ "POST", auth, "http://localhost.evil.example", ping }, s, p).status == 403);
    CHECK(mcpHandleHttp({ "POST", auth, "null", ping }, s, p).status == 403);

    const auto unauthorized = mcpHandleHttp({ "POST", "", "", ping }, s, p);
    CHECK(unauthorized.status == 401);
    CHECK(!unauthorized.headers.empty() && unauthorized.headers.back().first == "WWW-Authenticate");
    CHECK(mcpHandleHttp({ "POST", "Bearer wrong", "", ping }, s, p).status == 401);
    CHECK(mcpHandleHttp({ "POST", "Basic " + s.token().toLatin1(), "", ping }, s, p).status == 401);
    // browser preflight has no token -> rejected, no CORS headers anywhere
    CHECK(mcpHandleHttp({ "OPTIONS", "", "http://localhost", "" }, s, p).status == 401);

    CHECK(mcpHandleHttp({ "GET", auth, "", "" }, s, p).status == 405);
    CHECK(mcpHandleHttp({ "DELETE", auth, "", "" }, s, p).status == 405);
    CHECK(mcpHandleHttp({ "POST", auth, "", QByteArray(kMcpMaxBodyBytes + 1, ' ') }, s, p).status == 413);
    const auto note = mcpHandleHttp({ "POST", auth, "", R"({"jsonrpc":"2.0","method":"notifications/initialized"})" }, s, p);
    CHECK(note.status == 202 && note.body.isEmpty());
    for(const auto& h : mcpHandleHttp({ "POST", auth, "", ping }, s, p).headers)
        CHECK(!h.first.startsWith("Access-Control"));

    s.setEnabled(false);
    CHECK(mcpHandleHttp({ "POST", auth, "", ping }, s, p).status == 401);
}
} // namespace

int main()
{
    initialize();
    notificationsAndPing();
    errors();
    tools();
    batch();
    settings();
    http();
    std::printf("%d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}
