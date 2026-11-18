#ifndef MCP_SERVER_H
#define MCP_SERVER_H

#include <memory>
#include <string>
#include <vector>

#include <QString>

#include "jsonapi/jsonapi.h"

class McpProtocol;
class McpSettings;

// Mounts the MCP endpoint (/lua4rs/mcp) into RetroShare's JSON API server.
// Uses its port, but not its users: requests need the MCP token (Bearer).
// All methods except the request handler run on the GUI thread.
class McpServer : public JsonApiResourceProvider
{
public:
    McpServer(McpSettings& settings, std::shared_ptr<McpProtocol> protocol);

    // Register or unregister according to settings; restarts the JSON API if
    // it runs (resources are only published on start).
    void apply();
    // Before the plugin goes away: unregister, no restart.
    void shutdown();

    bool registered() const;
    static bool jsonApiRunning();
    static QString endpointUrl();
    static const char* path() { return "/lua4rs/mcp"; }

    // JsonApiResourceProvider
    std::vector<std::shared_ptr<rb::Resource>> getResources() const override;
    std::string getName() const override { return "Lua4RS MCP"; }

private:
    static void restartJsonApi(int attempt);

    McpSettings& _settings;
    std::shared_ptr<McpProtocol> _protocol;
    std::shared_ptr<rb::Resource> _resource;
};

#endif // MCP_SERVER_H
