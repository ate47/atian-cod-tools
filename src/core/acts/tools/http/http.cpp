#include <includes.hpp>
#include <acts_api/http_server.h>
#include <acts_api_impl/api_impl.hpp>
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
            std::unordered_map<std::string, std::string> headers{};
            std::string headerField{};
            bool headerInValue{};
            bool bodyTooLarge{};
            bool closing{};
        };

        constexpr size_t MAX_BODY_SIZE = 4 * 1024 * 1024;
        constexpr const char* CONTENT_TYPE_JSON = "application/json";
        const char* SUPPORTED_PROTOCOL_VERSIONS[]{ "2024-11-05", "2025-03-26", "2025-06-18", "2025-11-25" };

        std::string ToLower(std::string str) {
            std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return (char)std::tolower(c); });
            return str;
        }

        /*
         * Extract the host name from a "host[:port]" or "[ipv6][:port]" string
         */
        std::string ExtractHost(const std::string& value) {
            if (value.starts_with('[')) {
                size_t end{ value.find(']') };
                return ToLower(value.substr(0, end == std::string::npos ? std::string::npos : end + 1));
            }
            return ToLower(value.substr(0, value.find(':')));
        }

        bool IsLoopbackHost(const std::string& host) {
            return host == "localhost" || host == "127.0.0.1" || host == "[::1]" || host == "::1";
        }

        HttpResponse MakeError(int status) {
            core::config::JDoc res{};
            res.SetString("error", llhttp_status_name((llhttp_status_t)status));
            HttpResponse resp{};
            resp.status = status;
            resp.body = res.ToString(true);
            return resp;
        }

        struct WriteReq {
            uv_write_t req{};
            std::string data{};
            uv_buf_t buf{};
        };

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

        void SendResponse(Connection* con, const HttpResponse& resp) {
            WriteReq* wr{ new WriteReq{} };
            wr->data = std::format("HTTP/1.1 {} {}\r\n", resp.status, llhttp_status_name((llhttp_status_t)resp.status));
            if (!resp.body.empty()) {
                wr->data += std::format("Content-Type: {}\r\n", CONTENT_TYPE_JSON);
            }
            for (const std::pair<std::string, std::string>& header : resp.headers) {
                wr->data += std::format("{}: {}\r\n", header.first, header.second);
            }
            wr->data += std::format("Content-Length: {}\r\nConnection: close\r\n\r\n", resp.body.size());
            wr->data += resp.body;
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

        int OnHeaderField(llhttp_t* parser, const char* at, size_t length) {
            Connection* con{ static_cast<Connection*>(parser->data) };
            if (con->headerInValue) {
                // new header
                con->headerField.clear();
                con->headerInValue = false;
            }
            con->headerField.append(ToLower(std::string{ at, length }));
            return 0;
        }

        int OnHeaderValue(llhttp_t* parser, const char* at, size_t length) {
            Connection* con{ static_cast<Connection*>(parser->data) };
            con->headerInValue = true;
            con->headers[con->headerField].append(at, length);
            return 0;
        }

        int OnBody(llhttp_t* parser, const char* at, size_t length) {
            Connection* con{ static_cast<Connection*>(parser->data) };
            if (con->bodyTooLarge || con->body.size() + length > MAX_BODY_SIZE) {
                con->bodyTooLarge = true;
                return 0;
            }
            con->body.append(at, length);
            return 0;
        }

        int OnMessageComplete(llhttp_t* parser) {
            Connection* con{ static_cast<Connection*>(parser->data) };
            HttpResponse resp{};
            if (con->bodyTooLarge) {
                resp = MakeError(HTTP_STATUS_PAYLOAD_TOO_LARGE);
            } else {
                HttpRequest req{};
                req.method = parser->method;
                req.url = std::move(con->url);
                req.headers = std::move(con->headers);
                req.body = std::move(con->body);
                resp = con->server->HandleRequest(req);
            }
            SendResponse(con, resp);
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
            con->settings.on_header_field = OnHeaderField;
            con->settings.on_header_value = OnHeaderValue;
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
        int r{ uv_ip4_addr(cfg.host.c_str(), cfg.port, &addr) };
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

    bool HttpServer::Poll() {
        if (!started) {
            return false;
        }
        return uv_run(&loop, UV_RUN_NOWAIT) != 0;
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

    HttpResponse HttpServer::HandleRequest(const HttpRequest& req) {
        LOG_INFO(
            "[HTTP] {} {}",
            cli::clicolor::COLOR_AZURE.Text(req.url),
            cli::clicolor::COLOR_DARK_GRAY
                .Format("({}/{} bytes)", llhttp_method_name((llhttp_method_t)req.method), req.body.size())
        );

        // DNS rebinding protection, a browser can't be used to reach the server from another site
        const std::string hostCfg{ ToLower(cfg.host) };
        const bool wildcardBind{ hostCfg == "0.0.0.0" || hostCfg == "::" };
        const std::string* origin{ req.GetHeader("origin") };
        if (origin) {
            size_t schemeEnd{ origin->find("://") };
            std::string originHost{ schemeEnd == std::string::npos ? "" : ExtractHost(origin->substr(schemeEnd + 3)) };
            if (!IsLoopbackHost(originHost) && originHost != hostCfg) {
                LOG_DEBUG("Rejected origin '{}'", *origin);
                return MakeError(HTTP_STATUS_FORBIDDEN);
            }
        }
        const std::string* hostHeader{ req.GetHeader("host") };
        if (hostHeader && !wildcardBind) {
            std::string host{ ExtractHost(*hostHeader) };
            if (!IsLoopbackHost(host) && host != hostCfg) {
                LOG_DEBUG("Rejected host '{}'", *hostHeader);
                return MakeError(HTTP_STATUS_FORBIDDEN);
            }
        }

        // ignore the query string and the trailing slash
        std::string path{ req.url.substr(0, req.url.find('?')) };
        if (path.size() > 1 && path.ends_with('/')) {
            path.pop_back();
        }

        if (!cfg.noMCPServer && path == "/mcp") {
            if (req.method != HTTP_POST) {
                // no server to client stream (GET) and no session (DELETE)
                HttpResponse resp{ MakeError(HTTP_STATUS_METHOD_NOT_ALLOWED) };
                resp.headers.emplace_back("Allow", "POST");
                return resp;
            }

            const std::string* contentType{ req.GetHeader("content-type") };
            if (!contentType || !ToLower(*contentType).starts_with(CONTENT_TYPE_JSON)) {
                return MakeError(HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE);
            }

            // the header is optional, 2025-03-26 is assumed when it isn't present
            const std::string* version{ req.GetHeader("mcp-protocol-version") };
            if (version) {
                bool supported{};
                for (const char* v : SUPPORTED_PROTOCOL_VERSIONS) {
                    if (*version == v) {
                        supported = true;
                        break;
                    }
                }
                if (!supported) {
                    return MakeError(HTTP_STATUS_BAD_REQUEST);
                }
            }

            HttpResponse resp{};
            resp.body = tool::mcp::HandleRpc(req.body, resp.status);
            if (resp.status == HTTP_STATUS_ACCEPTED) {
                resp.body.clear(); // notifications have no body
            }
            return resp;
        }

        HttpResponse resp{};
        core::config::JDoc res{};

        if (req.method == HTTP_GET && path == "/") {
            resp.status = HTTP_STATUS_OK;
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
            return MakeError(HTTP_STATUS_NOT_FOUND);
        }

        resp.body = res.ToString(true);
        return resp;
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

        // embedded server, driven by ActsAPIHttpServer_Poll
        class EmbeddedServer {
          public:
            HttpServerConfig cfg{};
            HttpServer server;

            EmbeddedServer() : server(cfg) {}
            ~EmbeddedServer() { LOG_INFO("http server stopped"); }
        };
    } // namespace
} // namespace tool::http

ActsHandle ActsAPIHttpServer_Start(const ActsAPIHttpServer_Options* options) {
    if (!options || options->structSize == 0) {
        ActsAPISetLastMessage("Invalid structSize for options");
        return INVALID_ACTS_HANDLE_VALUE;
    }

    tool::http::EmbeddedServer* srv{ ActsAPIImpl_New<tool::http::EmbeddedServer>() };
    ActsHandle ah{ (ActsHandle)srv };

    const char* host{ SIZED_STRUCT_MEMBER(options, host, nullptr) };
    uint16_t port{ SIZED_STRUCT_MEMBER(options, port, (uint16_t)0) };
    bool noMCP{ SIZED_STRUCT_MEMBER(options, noMCP, false) };

    if (host && *host) {
        srv->cfg.host = host;
    }
    if (port) {
        srv->cfg.port = port;
    }
    srv->cfg.noMCPServer = noMCP;

    if (!srv->server.Start()) {
        std::string address{ std::format("{}:{}", srv->cfg.host, srv->cfg.port) };
        ActsAPICloseHandle(ah);
        ActsAPISetLastMessage("Can't start the http server on %s", address.c_str());
        return INVALID_ACTS_HANDLE_VALUE;
    }
    LOG_INFO(
        "http server listening on {}",
        cli::clicolor::COLOR_GREEN.Format("http://{}:{}", srv->cfg.host, srv->cfg.port)
    );
    return ah;
}

ActsStatus ActsAPIHttpServer_Poll(ActsHandle server) {
    ACTS_API_ASSERT_VALID_HANDLE(server);
    reinterpret_cast<tool::http::EmbeddedServer*>(server)->server.Poll();
    return ACTS_STATUS_OK;
}
