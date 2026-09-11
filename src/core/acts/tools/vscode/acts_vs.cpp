#include <includes.hpp>
#include <cli/cli_options.hpp>
#include <tools/vscode/lsp.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <core/config.hpp>

#include <fcntl.h>
#include <io.h>

namespace {
    using namespace hash::literals;
    using namespace lsp;
    using JsonDocument = rapidjson::Document;
    using JsonValue = rapidjson::Value;

    struct LanguageServerConfig {
        bool help{};
    };

    void WriteMessage(const JsonDocument& message) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer{ buffer };
        message.Accept(writer);

        std::cout << "Content-Length: " << buffer.GetSize() << "\r\n\r\n";
        std::cout.write(buffer.GetString(), static_cast<std::streamsize>(buffer.GetSize()));
        std::cout.flush();
    }

    bool ReadMessage(std::string& body) {
        std::string line;
        size_t contentLength{};

        while (std::getline(std::cin, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            if (line.empty()) {
                break;
            }

            constexpr std::string_view contentLengthHeader{ "Content-Length:" };
            if (line.size() >= contentLengthHeader.size() &&
                line.compare(0, contentLengthHeader.size(), contentLengthHeader) == 0) {
                contentLength = std::stoull(line.substr(contentLengthHeader.size()));
            }
        }

        if (!std::cin || !contentLength) {
            return false;
        }

        body.resize(contentLength);
        std::cin.read(body.data(), static_cast<std::streamsize>(contentLength));
        return std::cin.gcount() == static_cast<std::streamsize>(contentLength);
    }

    void AddResult(JsonDocument& response, JsonValue&& result) {
        response.AddMember("result", std::move(result), response.GetAllocator());
    }

    void CopyId(JsonDocument& response, const JsonValue* id) {
        if (!id) {
            response.AddMember("id", JsonValue{}.SetNull(), response.GetAllocator());
            return;
        }

        JsonValue copied;
        copied.CopyFrom(*id, response.GetAllocator());
        response.AddMember("id", std::move(copied), response.GetAllocator());
    }

    void SendResponse(const JsonValue* id, JsonValue&& result) {
        JsonDocument response{ rapidjson::kObjectType };
        response.AddMember("jsonrpc", "2.0", response.GetAllocator());
        CopyId(response, id);
        AddResult(response, std::move(result));
        WriteMessage(response);
    }

    void SendError(const JsonValue* id, JsonRPCError code, const char* message) {
        core::config::Config response{};
        CopyId(response.main, id);
        response.SetString("jsonrpc", "2.0");
        response.SetInteger("error.code", code);
        response.SetString("error.message", message);
        WriteMessage(response.main);
    }

    void PublishEmptyDiagnostics(const JsonValue& uri) {
        JsonDocument notification{ rapidjson::kObjectType };
        notification.AddMember("jsonrpc", "2.0", notification.GetAllocator());
        notification.AddMember("method", "textDocument/publishDiagnostics", notification.GetAllocator());

        JsonValue params{ rapidjson::kObjectType };
        JsonValue copiedUri;
        copiedUri.CopyFrom(uri, notification.GetAllocator());
        params.AddMember("uri", std::move(copiedUri), notification.GetAllocator());
        params.AddMember("diagnostics", JsonValue{ rapidjson::kArrayType }, notification.GetAllocator());
        notification.AddMember("params", std::move(params), notification.GetAllocator());
        WriteMessage(notification);
    }

    JsonValue ServerCapabilities(rapidjson::Document::AllocatorType& allocator) {
        JsonValue capabilities{ rapidjson::kObjectType };
        capabilities.AddMember("textDocumentSync", 1, allocator);

        JsonValue completion{ rapidjson::kObjectType };
        completion.AddMember("resolveProvider", false, allocator);
        JsonValue triggerCharacters{ rapidjson::kArrayType };
        triggerCharacters.PushBack(".", allocator);
        completion.AddMember("triggerCharacters", std::move(triggerCharacters), allocator);
        capabilities.AddMember("completionProvider", std::move(completion), allocator);
        capabilities.AddMember("hoverProvider", true, allocator);
        return capabilities;
    }

    void RunLanguageServer(LanguageServerConfig& cfg) {
        (void)_setmode(_fileno(stdin), _O_BINARY);
        (void)_setmode(_fileno(stdout), _O_BINARY);

        bool running{ true };
        std::string body;
        while (running && ReadMessage(body)) {
            JsonDocument request;
            request.Parse(body.data(), body.size());
            if (request.HasParseError() || !request.IsObject()) {
                SendError(nullptr, RPC_ParseError, "Invalid JSON");
                continue;
            }

            const JsonValue* id{ request.HasMember("id") ? &request["id"] : nullptr };
            const char* method{ request.HasMember("method") && request["method"].IsString()
                                    ? request["method"].GetString()
                                    : nullptr };
            const JsonValue* params{ request.HasMember("params") ? &request["params"] : nullptr };

            if (!method || !*method) {
                if (id) {
                    SendError(id, RPC_InvalidRequest, "Invalid request");
                }
                continue;
            }

            switch (hash::HashX64(method)) {
            case "initialize"_x: {
                JsonDocument result{ rapidjson::kObjectType };
                result.AddMember("capabilities", ServerCapabilities(result.GetAllocator()), result.GetAllocator());
                JsonValue info{ rapidjson::kObjectType };
                info.AddMember("name", "acts-langs", result.GetAllocator());
                result.AddMember("serverInfo", std::move(info), result.GetAllocator());
                SendResponse(id, std::move(result));
                break;
            }
            case "exit"_x: {
                running = false;
                break;
            }
            case "textDocument/didOpen"_x:
            case "textDocument/didChange"_x: {
                if (params && params->IsObject() && params->HasMember("textDocument") &&
                    (*params)["textDocument"].IsObject() && (*params)["textDocument"].HasMember("uri")) {
                    PublishEmptyDiagnostics((*params)["textDocument"]["uri"]);
                }
                break;
            }
            case "textDocument/didClose"_x: {
                if (params && params->IsObject() && params->HasMember("textDocument") &&
                    (*params)["textDocument"].IsObject() && (*params)["textDocument"].HasMember("uri")) {
                    PublishEmptyDiagnostics((*params)["textDocument"]["uri"]);
                }
                break;
            }
            case "textDocument/completion"_x: {
                JsonDocument result{ rapidjson::kObjectType };
                result.AddMember("isIncomplete", false, result.GetAllocator());

                JsonValue items{ rapidjson::kArrayType };
                auto& allocator{ result.GetAllocator() };

                auto addCompletion = [&items, &allocator](
                                         const char* label,
                                         SymbolKind kind,
                                         const char* detail,
                                         const char* documentation,
                                         const char* insertText
                                     ) {
                    JsonValue item{ rapidjson::kObjectType };
                    item.AddMember("label", JsonValue{ label, allocator }, allocator);
                    item.AddMember("kind", kind, allocator);
                    item.AddMember("detail", JsonValue{ detail, allocator }, allocator);
                    item.AddMember("documentation", JsonValue{ documentation, allocator }, allocator);
                    item.AddMember("insertText", JsonValue{ insertText, allocator }, allocator);
                    items.PushBack(std::move(item), allocator);
                };

                addCompletion("self", SK_Constant, "GSC keyword", "The current script object.", "self");
                addCompletion("function", SK_Namespace, "GSC function", "Mock function declaration.", "function");
                addCompletion("wait", SK_Namespace, "GSC builtin", "Wait for the specified amount of time.", "wait");
                addCompletion("waittill", SK_Namespace, "GSC builtin", "Wait until an event is received.", "waittill");
                addCompletion("endon", SK_Namespace, "GSC builtin", "End the current notification handler.", "endon");

                result.AddMember("items", std::move(items), allocator);
                SendResponse(id, std::move(result));
                break;
            }
            case "textDocument/hover"_x: {
                JsonDocument result{ rapidjson::kObjectType };
                auto& allocator{ result.GetAllocator() };

                JsonValue contents{ rapidjson::kObjectType };
                contents.AddMember("kind", "markdown", allocator);
                contents.AddMember(
                    "value",
                    "**Mock GSC symbol**\n\nThis hover response is provided by the acts language server.",
                    allocator
                );
                result.AddMember("contents", std::move(contents), allocator);
                SendResponse(id, std::move(result));
                break;
            }
            case "$/setTrace"_x:
            case "initialized"_x:
            case "shutdown"_x: {
                JsonValue result;
                result.SetNull();
                SendResponse(id, std::move(result));
                break;
            }
            default: {
                SendError(id, RPC_MethodNotFound, utils::va("Method not found '%s'", method));
                break;
            }
            }
        }
    }

    int language_server(int argc, const char* argv[]) {
        LanguageServerConfig cfg{};
        cli::options::CliOptions opts{};
        opts.addOption(&cfg.help, "show help", "--help", "", "-h");

        if (!opts.ComputeOptions(2, argc, argv) || cfg.help) {
            opts.PrintOptions();
            return cfg.help ? tool::OK : tool::BAD_USAGE;
        }

        RunLanguageServer(cfg);
        return tool::OK;
    }

    ADD_TOOL(language_server, "common", "", "acts language server", language_server);
} // namespace