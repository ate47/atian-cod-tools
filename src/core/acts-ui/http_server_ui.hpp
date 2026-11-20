#pragma once
#include <config_ui.hpp>
#include <acts_api/http_server.h>
#include <QString>

// http server config, the containers are shared by path between the files including this header
UI_CONFIG_VAL(cfgHttpServerHost, "http.server.host", QString(ACTS_HTTP_DEFAULT_HOST), "http server host");
UI_CONFIG_VAL(cfgHttpServerPort, "http.server.port", (uint16_t)ACTS_HTTP_DEFAULT_PORT, "http server port");
UI_CONFIG_VAL(cfgHttpServerEnabled, "http.server.enabled", false, "enable http server");
UI_CONFIG_VAL(cfgHttpServerMcpTools, "http.server.mcpTools", true, "enable mcp server");

/*
 * Embedded http server of the UI, it is driven by the Qt event loop of the main thread, all the functions must be
 * called from this thread after the creation of the QApplication.
 */
namespace ui3::http_server {
    /*
     * Start the server using the current config (host, port and MCP tools), the enabled config isn't checked
     * @return true if the server is running, otherwise the error is available using GetLastError
     */
    bool Start();

    /*
     * Stop the server, the port is released when the function returns
     */
    void Stop();

    /*
     * Stop the server and start it again with the current config if it is enabled
     * @return true if the server is running, otherwise the error is available using GetLastError
     */
    bool Restart();

    /*
     * @return if the server is running
     */
    bool IsRunning();

    /*
     * @return if the config was modified since the start of the server, or if the server is running while it is
     * disabled by the config, or if it isn't running while it is enabled by the config
     */
    bool NeedsRestart();

    /*
     * @return the base url of the running server (http://host:port), empty if it isn't running
     */
    QString GetRunningUrl();

    /*
     * @return if the running server has the MCP endpoints, false if it isn't running
     */
    bool IsRunningMcp();

    /*
     * @return the message of the last start failure, empty if the last start succeeded
     */
    QString GetLastError();
} // namespace ui3::http_server
