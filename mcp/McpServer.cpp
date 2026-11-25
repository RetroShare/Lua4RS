#include "McpServer.h"
#include "McpHttp.h"
#include "McpProtocol.h"
#include "McpSettings.h"

#include <iostream>
#include <map>

#include <QTimer>

#include <retroshare/rsjsonapi.h>
#include "util/rsdebuglevel0.h"

namespace {
void closeWith(const std::shared_ptr<rb::Session>& session, const McpHttpReply& reply)
{
    std::multimap<std::string, std::string> headers;
    for(const auto& h : reply.headers) headers.insert({ h.first.toStdString(), h.second.toStdString() });
    headers.insert({ "Content-Length", std::to_string(reply.body.size()) });
    session->close(reply.status, reply.body.toStdString(), headers);
}
}

McpServer::McpServer(McpSettings& settings, std::shared_ptr<McpProtocol> protocol)
    : _settings(settings), _protocol(std::move(protocol)), _resource(std::make_shared<rb::Resource>())
{
    // The handler keeps settings and protocol alive through shared state, not
    // through `this`: restbed holds the resource until the server restarts.
    McpSettings* settingsPtr = &_settings;
    std::shared_ptr<McpProtocol> protocolPtr = _protocol;
    const auto handler = [settingsPtr, protocolPtr](const std::shared_ptr<rb::Session> session) {
        const auto request = session->get_request();
        const int length = request->get_header("Content-Length", 0);
        if(length < 0 || length > kMcpMaxBodyBytes)
        {
            closeWith(session, { 413, "Request too large", { { "Content-Type", "text/plain; charset=utf-8" } } });
            return;
        }
        session->fetch(static_cast<size_t>(length), [settingsPtr, protocolPtr](
                       const std::shared_ptr<rb::Session> session, const rb::Bytes& body) {
            const auto request = session->get_request();
            McpHttpRequest r;
            r.method = QByteArray::fromStdString(request->get_method());
            r.authorization = QByteArray::fromStdString(request->get_header("Authorization", std::string()));
            r.origin = QByteArray::fromStdString(request->get_header("Origin", std::string()));
            r.body = QByteArray(reinterpret_cast<const char*>(body.data()), static_cast<int>(body.size()));
            closeWith(session, mcpHandleHttp(r, *settingsPtr, *protocolPtr));
        });
    };
    _resource->set_path(path());
    for(const char* method : { "POST", "GET", "DELETE", "OPTIONS" })
        _resource->set_method_handler(method, handler);
}

McpServer::~McpServer() = default;

void McpServer::startWatching()
{
    if(_watch) return;
    _watch.reset(new QTimer);
    QObject::connect(_watch.get(), &QTimer::timeout, [this]() { apply(); });
    _watch->start(2000);
    apply();
}

std::vector<std::shared_ptr<rb::Resource>> McpServer::getResources() const
{
    return { _resource };
}

bool McpServer::registered() const
{
    return rsJsonApi && rsJsonApi->hasResourceProvider(*this);
}

bool McpServer::jsonApiRunning()
{
    return rsJsonApi && rsJsonApi->isRunning();
}

QString McpServer::endpointUrl()
{
    if(!rsJsonApi) return QString();
    QString host = QString::fromStdString(rsJsonApi->getBindingAddress());
    // "Any address" works for listening, not for connecting (Windows refuses).
    if(host == "0.0.0.0") host = "127.0.0.1";
    else if(host == "::") host = "::1";
    if(host.contains(':')) host = "[" + host + "]";
    return QString("http://%1:%2%3").arg(host).arg(rsJsonApi->listeningPort()).arg(path());
}

void McpServer::apply()
{
    if(!rsJsonApi) return;
    const bool wanted = _settings.enabled();
    if(wanted == registered()) return;
    if(wanted) rsJsonApi->registerResourceProvider(*this);
    else rsJsonApi->unregisterResourceProvider(*this);
    if(rsJsonApi->isRunning()) restartJsonApi(0);
}

void McpServer::shutdown()
{
    _watch.reset();
    if(registered()) rsJsonApi->unregisterResourceProvider(*this);
}

void McpServer::restartJsonApi(int attempt)
{
    if(!rsJsonApi) return;
    const std::error_condition err = rsJsonApi->restart();
    // RetroShare refuses restarts in quick succession; try again later.
    if(err == RsJsonApiErrorNum::NOT_A_MACHINE_GUN && attempt < 6)
        QTimer::singleShot(10000, [attempt]() { restartJsonApi(attempt + 1); });
    else if(err)
        RS_ERR("could not restart JSON API for MCP: ", err.message());
}
