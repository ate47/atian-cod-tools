#include <includes.hpp>
#include <tools/http/http.hpp>
#include <tools/mcp/mcp.hpp>
#include <tools/mcp/mcp_tool.hpp>
#include <llhttp.h>
#include <cli/cli_options.hpp>
#include <cli/clicolor.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <core/actsinfo.hpp>
#include <core/config.hpp>

namespace tool::http {
    namespace {
        struct Connection {
            uv_tcp_t handle{};
            llhttp_t parser{};
            llhttp_settings_t settings{};
            HttpServer* server{};
            std::string url{};
            std::string body{};
            bool closing{};
        };

        struct WriteReq {
            uv_write_t req{};
            std::string data{};
            uv_buf_t buf{};
        };

        const char* StatusText(int status) {
            switch (status) {
            case 200:
                return "OK";
            case 404:
                return "Not Found";
            default:
                return "Error";
            }
        }

        void OnClosed(uv_handle_t* handle) {
            if (handle->type == UV_TCP && handle->data) {
                delete static_cast<Connection*>(handle->data);
            }
        }

        void CloseConnection(Connection* con) {
            if (con->closing) {
                return;
            }
            con->closing = true;
            uv_close(reinterpret_cast<uv_handle_t*>(&con->handle), OnClosed);
        }

        void OnWritten(uv_write_t* req, int status) {
            WriteReq* wr{ reinterpret_cast<WriteReq*>(req) };
            Connection* con{ static_cast<Connection*>(req->handle->data) };
            if (status < 0 && status != UV_ECANCELED) {
                LOG_DEBUG("MCP write error: {}", uv_strerror(status));
            }
            delete wr;
            // one response per connection
            CloseConnection(con);
        }

        void SendResponse(Connection* con, int status, const std::string& body) {
            WriteReq* wr{ new WriteReq{} };
            wr->data = std::format(
                "HTTP/1.1 {} {}\r\nContent-Type: application/json\r\nContent-Length: {}\r\nConnection: close\r\n\r\n{}",
                status,
                StatusText(status),
                body.size(),
                body
            );
            wr->buf = uv_buf_init(wr->data.data(), (unsigned int)wr->data.size());
            int r{ uv_write(&wr->req, reinterpret_cast<uv_stream_t*>(&con->handle), &wr->buf, 1, OnWritten) };
            if (r < 0) {
                LOG_DEBUG("MCP write error: {}", uv_strerror(r));
                delete wr;
                CloseConnection(con);
            }
        }

        int OnUrl(llhttp_t* parser, const char* at, size_t length) {
            static_cast<Connection*>(parser->data)->url.append(at, length);
            return 0;
        }

        int OnBody(llhttp_t* parser, const char* at, size_t length) {
            static_cast<Connection*>(parser->data)->body.append(at, length);
            return 0;
        }

        int OnMessageComplete(llhttp_t* parser) {
            Connection* con{ static_cast<Connection*>(parser->data) };
            int status{ 200 };
            std::string resp{ con->server->HandleRequest(parser->method, con->url, con->body, status) };
            SendResponse(con, status, resp);
            return HPE_PAUSED; // stop parsing, only one request per connection
        }

        void OnAlloc(uv_handle_t*, size_t suggested, uv_buf_t* buf) {
            buf->base = static_cast<char*>(std::malloc(suggested));
            buf->len = buf->base ? (decltype(buf->len))suggested : 0;
        }

        void OnRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
            Connection* con{ static_cast<Connection*>(stream->data) };
            if (nread > 0 && !con->closing) {
                llhttp_errno_t err{ llhttp_execute(&con->parser, buf->base, (size_t)nread) };
                if (err != HPE_OK && err != HPE_PAUSED) {
                    LOG_DEBUG("MCP http parse error: {}", llhttp_errno_name(err));
                    CloseConnection(con);
                }
            } else if (nread < 0) {
                CloseConnection(con);
            }
            std::free(buf->base);
        }

        void OnConnection(uv_stream_t* srv, int status) {
            if (status < 0) {
                LOG_ERROR("MCP connection error: {}", uv_strerror(status));
                return;
            }
            Connection* con{ new Connection{} };
            con->server = static_cast<HttpServer*>(srv->data);
            con->handle.data = con;
            uv_tcp_init(srv->loop, &con->handle);

            llhttp_settings_init(&con->settings);
            con->settings.on_url = OnUrl;
            con->settings.on_body = OnBody;
            con->settings.on_message_complete = OnMessageComplete;
            llhttp_init(&con->parser, HTTP_REQUEST, &con->settings);
            con->parser.data = con;

            if (uv_accept(srv, reinterpret_cast<uv_stream_t*>(&con->handle)) != 0 ||
                uv_read_start(reinterpret_cast<uv_stream_t*>(&con->handle), OnAlloc, OnRead) != 0) {
                CloseConnection(con);
            }
        }
    } // namespace

    HttpServer::HttpServer(HttpServerConfig& cfg) : cfg(cfg) {
        if (uv_loop_init(&loop) == 0) {
            loopInit = true;
        }
    }

    HttpServer::~HttpServer() {
        if (!loopInit) {
            return;
        }
        if (started) {
            // close every remaining handle (server, signal, async, connections) and flush the close callbacks
            uv_walk(
                &loop,
                [](uv_handle_t* h, void* arg) {
                    if (uv_is_closing(h)) {
                        return;
                    }
                    // the listening handle isn't a connection, it must not be freed by OnClosed
                    uv_close(h, h == arg ? nullptr : OnClosed);
                },
                &server
            );
            uv_run(&loop, UV_RUN_DEFAULT);
        }
        uv_loop_close(&loop);
    }

    bool HttpServer::Start() {
        if (!loopInit) {
            LOG_ERROR("Can't init the event loop");
            return false;
        }
        if (started) {
            return true;
        }

        sockaddr_in addr{};
        int r{ uv_ip4_addr(cfg.host, cfg.port, &addr) };
        if (r < 0) {
            LOG_ERROR("Invalid address {}:{}: {}", cfg.host, cfg.port, uv_strerror(r));
            return false;
        }

        uv_tcp_init(&loop, &server);
        server.data = this;
        started = true; // handles are now registered, the destructor must close them

        r = uv_tcp_bind(&server, reinterpret_cast<const sockaddr*>(&addr), 0);
        if (r < 0) {
            LOG_ERROR("Can't bind {}:{}: {}", cfg.host, cfg.port, uv_strerror(r));
            return false;
        }
        r = uv_listen(reinterpret_cast<uv_stream_t*>(&server), 128, OnConnection);
        if (r < 0) {
            LOG_ERROR("Can't listen {}:{}: {}", cfg.host, cfg.port, uv_strerror(r));
            return false;
        }

        uv_async_init(&loop, &stopAsync, [](uv_async_t* h) { uv_stop(h->loop); });
        uv_signal_init(&loop, &sigint);
        uv_signal_start(&sigint, [](uv_signal_t* h, int) { uv_stop(h->loop); }, SIGINT);
        return true;
    }

    void HttpServer::Run() {
        if (!cfg.noMCPServer) {
            LOG_INFO("init {} mcp tool(s)", tool::mcp::GetMcpTools().size());
        }
        LOG_INFO(
            "http server listening on {} (Ctrl+C to stop)",
            cli::clicolor::COLOR_GREEN.Format("http://{}:{}", cfg.host, cfg.port)
        );
        uv_run(&loop, UV_RUN_DEFAULT);
        LOG_INFO("http server stopped");
    }

    void HttpServer::Stop() {
        if (started) {
            uv_async_send(&stopAsync);
        }
    }
    std::string
    HttpServer::HandleRequest(uint8_t method, const std::string& url, const std::string& body, int& status) {
        LOG_INFO(
            "[HTTP] {} {}",
            cli::clicolor::COLOR_AZURE.Text(url),
            cli::clicolor::COLOR_DARK_GRAY
                .Format("({}/{} bytes)", llhttp_method_name((llhttp_method_t)method), body.size())
        );

        if (!cfg.noMCPServer && method == HTTP_POST && url == "/mcp") {
            return tool::mcp::HandleRpc(body, status);
        }

        core::config::JDoc res{};

        if (method == HTTP_GET && url == "/") {
            status = HTTP_STATUS_OK;
            core::config::ConfigGeneric acts{ res.GetSubVal("server", true) };
            acts.SetString("name", "acts");
            acts.SetString("version", core::actsinfo::VERSION);
            acts.SetInteger("versionId", core::actsinfo::VERSION_ID);
            core::config::ConfigGeneric features{ acts.GetSubVal("features", true) };
            features.SetBool("hasMCPServer", !cfg.noMCPServer);

            if (!cfg.noMCPServer) {
                res.SetString("mcp.endpoint", std::format("http://{}:{}/mcp", cfg.host, cfg.port));
                for (const tool::mcp::McpTool* tool : tool::mcp::GetMcpTools()) {
                    core::config::ConfigGeneric toolJson{ res.GetSubVal("mcp.tools[]", true) };
                    toolJson.SetString("name", tool->name);
                    toolJson.SetString("description", tool->description);
                    if (tool->inputSchema) {
                        rapidjson::Document schema{};
                        schema.Parse(tool->inputSchema);
                        if (schema.HasParseError()) {
                            LOG_WARNING("Failed to parse schema for tool {}: {}", tool->name, tool->inputSchema);
                            continue;
                        }
                        toolJson.Set(utils::va("schema", tool->name), schema);
                    }
                }
            }
        } else {
            status = HTTP_STATUS_NOT_FOUND;
            res.SetString("error", llhttp_status_name(HTTP_STATUS_NOT_FOUND));
        }

        return res.ToString(true);
    }

    namespace {
        int http_server(int argc, const char* argv[]) {
            HttpServerConfig opt{};
            bool help{};
            cli::options::CliOptions opts{};
            opts.addOption(&help, "show help", "--help", "", "-h");
            opts.addOption(&opt.host, "server host", "--host", " [host]", "-H");
            opts.addOption(&opt.port, "server port", "--port", " [port]", "-p");
            opts.addOption(&opt.noMCPServer, "disable mcp server", "--no-mcp");

            if (!opts.ComputeOptions(2, argc, argv) || help) {
                opts.PrintOptions();
                return help ? tool::OK : tool::BAD_USAGE;
            }

            HttpServer srv{ opt };
            if (!srv.Start()) {
                return tool::BASIC_ERROR;
            }
            srv.Run();
            return tool::OK;
        }

        ADD_TOOL(http_server, "dev", "", "start the http server", http_server);
    } // namespace
} // namespace tool::http
