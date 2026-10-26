#pragma once
#include <uv.h>
#include <rapidjson/document.h>

namespace tool::http {
    constexpr uint16_t DEFAULT_PORT = 8080;
    constexpr const char* DEFAULT_HOST = "127.0.0.1";

    /*
     * Server configuration
     */
    struct HttpServerConfig {
        uint16_t port{ DEFAULT_PORT };
        const char* host{ DEFAULT_HOST };
        bool noMCPServer{};
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
         * Request the event loop to stop, can be called from any thread while Run is active
         */
        void Stop();

        /*
         * Build the response body for a request
         * @param method HTTP method
         * @param url request url
         * @param body request body
         * @param status [out] HTTP status code
         * @return JSON response body
         */
        std::string HandleRequest(const char* method, const std::string& url, const std::string& body, int& status);

        uv_loop_t* GetLoop() { return &loop; }
    };
} // namespace tool::http
