#include <ui_includes.hpp>
#include "http_server_ui.hpp"
#include <QApplication>
#include <QTimer>

namespace ui3::http_server {
    namespace {
        // the address and options used by a server
        struct ServerConfig {
            QString host{};
            uint16_t port{};
            bool mcp{};

            bool operator==(const ServerConfig& other) const = default;
        };

        struct ServerState {
            ActsHandle handle{ INVALID_ACTS_HANDLE_VALUE };
            QTimer* timer{};
            // config used by the running server
            ServerConfig running{};
            QString lastError{};
        } state;

        // get the config, the values not set use the default ones like the server does
        ServerConfig GetConfig() {
            ServerConfig cfg{};
            cfg.host = cfgHttpServerHost.Get();
            if (cfg.host.isEmpty()) {
                cfg.host = ACTS_HTTP_DEFAULT_HOST;
            }
            cfg.port = cfgHttpServerPort.Get();
            if (!cfg.port) {
                cfg.port = ACTS_HTTP_DEFAULT_PORT;
            }
            cfg.mcp = cfgHttpServerMcpTools.Get();
            return cfg;
        }
    } // namespace

    bool IsRunning() { return IS_ACTS_HANDLE_VALID(state.handle); }

    bool Start() {
        if (IsRunning()) {
            return true;
        }
        state.lastError.clear();

        ServerConfig cfg{ GetConfig() };
        QByteArray host{ cfg.host.toUtf8() };

        ActsAPIHttpServer_Options opts{};
        opts.structSize = sizeof(opts);
        opts.host = host.constData();
        opts.port = cfg.port;
        opts.noMCP = !cfg.mcp;

        ActsHandle handle{ ActsAPIHttpServer_Start(&opts) };
        if (!IS_ACTS_HANDLE_VALID(handle)) {
            state.lastError = QString::fromUtf8(ActsGetAPILastMessage());
            LOG_ERROR("Can't start the http server: {}", state.lastError.toStdString());
            return false;
        }

        state.handle = handle;
        state.running = cfg;

        // sync the http server loop to qt
        if (!state.timer) {
            state.timer = new QTimer(qApp);
            QObject::connect(state.timer, &QTimer::timeout, []() {
                if (IsRunning()) {
                    ActsAPIHttpServer_Poll(state.handle);
                }
            });
        }
        state.timer->start(10);
        return true;
    }

    void Stop() {
        if (state.timer) {
            state.timer->stop();
        }
        if (IsRunning()) {
            ActsAPICloseHandle(state.handle);
            state.handle = INVALID_ACTS_HANDLE_VALUE;
        }
        state.running = {};
    }

    bool Restart() {
        Stop();
        if (!cfgHttpServerEnabled.Get()) {
            state.lastError.clear();
            return false;
        }
        return Start();
    }

    bool NeedsRestart() {
        bool enabled{ cfgHttpServerEnabled.Get() };
        if (IsRunning()) {
            return !enabled || state.running != GetConfig();
        }
        // not running while enabled: the server wasn't started yet or it failed to start
        return enabled;
    }

    QString GetRunningUrl() {
        if (!IsRunning()) {
            return {};
        }
        return QString("http://%1:%2").arg(state.running.host).arg(state.running.port);
    }

    bool IsRunningMcp() { return IsRunning() && state.running.mcp; }

    QString GetLastError() { return state.lastError; }
} // namespace ui3::http_server
