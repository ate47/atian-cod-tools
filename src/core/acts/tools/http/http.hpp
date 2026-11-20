#pragma once
#include <acts_api/http_server.h>
#include <uv.h>
#include <rapidjson/document.h>

namespace tool::http {
    /*
     * Server configuration
     */
    struct HttpServerConfig {
        uint16_t port{ ACTS_HTTP_DEFAULT_PORT };
        std::string host{ ACTS_HTTP_DEFAULT_HOST };
        bool noMCPServer{};
    };

    /*
     * HTTP request
     */
    struct HttpRequest {
        uint8_t method{};
        std::string url{};
        // header names are lowercase
        std::unordered_map<std::string, std::string> headers{};
        std::string body{};

        /*
         * Get a header value
         * @param name lowercase header name
         * @return header value or nullptr if not present
         */
        const std::string* GetHeader(const std::string& name) const {
            std::unordered_map<std::string, std::string>::const_iterator it{ headers.find(name) };
            return it == headers.end() ? nullptr : &it->second;
        }
    };

    /*
     * HTTP response, the body is sent as JSON when not empty
     */
    struct HttpResponse {
        int status{ 200 };
        std::string body{};
        std::vector<std::pair<std::string, std::string>> headers{};
    };

    /*
     * Basic HTTP server
     */
    class HttpServer {
        uv_loop_t loop{};
        uv_tcp_t server{};
        uv_signal_t sigint{};
        uv_async_t stopAsync{};
        HttpServerConfig& cfg;
        bool loopInit{};
        bool started{};

      public:
        HttpServer(HttpServerConfig& cfg);
        ~HttpServer();
        HttpServer(const HttpServer&) = delete;
        HttpServer& operator=(const HttpServer&) = delete;

        /*
         * Bind and listen on the configured address
         * @return true if the server is listening
         */
        bool Start();

        /*
         * Run the event loop until Stop is called or the process receives a SIGINT. Start must have succeeded.
         */
        void Run();

        /*
         * Process the pending events without blocking, to drive the server from the loop of another application.
         * Must be called from the same thread every time. Start must have succeeded.
         * @return true if there are still active handles
         */
        bool Poll();

        /*
         * Request the event loop to stop, can be called from any thread while Run is active
         */
        void Stop();

        /*
         * Build the response body for a request
         * @param req request
         * @return response
         */
        HttpResponse HandleRequest(const HttpRequest& req);

        uv_loop_t* GetLoop() { return &loop; }
    };
} // namespace tool::http
