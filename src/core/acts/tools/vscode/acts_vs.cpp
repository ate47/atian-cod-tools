#include <includes.hpp>
#include <core/actsinfo.hpp>
#include <cli/cli_options.hpp>
#include <tools/vscode/acts_vs.hpp>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>

#include <fcntl.h>
#include <io.h>

namespace tool::vscode {
    using namespace hash::literals;

    void LanguageServer::OpenFile(JDocSub ev) {
        const char* uri{ ev.GetCString("uri") };
        const char* text{ ev.GetCString("text") };

        if (uri && text) {
            TextDocument& doc{ docs[uri] };
            doc.uri = uri;
            doc.text = text;
        }
    }

    void LanguageServer::ChangeFile(JDocSub ev) {
        const char* uri{ ev.GetCString("textDocument.uri") };
        JDocSub changes{ ev.GetSubVal("contentChanges") };
        if (!uri || !changes.base.IsArray()) {
            return;
        }

        for (core::config::RapidJsonGeneric& change : changes.base.GetArray()) {
            JDocSub sub{ changes.GetSub(change) };
            const char* text{ sub.GetCString("text") };
            if (text) {
                TextDocument& doc{ docs[uri] };
                doc.uri = uri;
                doc.text = text;
            }
        }
    }

    void LanguageServer::CloseFile(JDocSub textDocument) { docs.erase(textDocument.GetString("uri")); }

    TextDocument* LanguageServer::GetTextDocument(JDocSub textDocument) {
        auto found{ docs.find(textDocument.GetString("uri")) };
        return found == docs.end() ? nullptr : &found->second;
    }

    void LanguageServer::DebugMessage(const char* type, const JDoc& message) {
        if (!df) {
            return;
        }
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer{ buffer };
        message.main.Accept(writer);
        df << type << " " << buffer.GetSize() << "\n" << buffer.GetString() << "\n\n";
        *df << std::flush;
    }

    void LanguageServer::WriteMessage(const JDoc& message) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer{ buffer };
        message.main.Accept(writer);

        std::cout << "Content-Length: " << buffer.GetSize() << "\r\n\r\n";
        std::cout.write(buffer.GetString(), static_cast<std::streamsize>(buffer.GetSize()));
        std::cout.flush();
        DebugMessage("WRITE", message);
    }

    bool LanguageServer::ReadMessage(std::string& body) {
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

    void LanguageServer::SendResponse(const JDocSub& id, core::config::RapidJsonGeneric&& result) {
        JDoc response{};
        response.SetString("jsonrpc", "2.0");
        JDocAllocatorType& allocator{ response.GetAllocator() };
        core::config::RapidJsonGeneric copiedId;
        copiedId.CopyFrom(id.base, allocator);
        response.main.AddMember("id", std::move(copiedId), allocator);
        response.main.AddMember("result", std::move(result), allocator);
        WriteMessage(response);
    }

    void LanguageServer::SendError(const JDocSub* id, lsp::JsonRPCError code, const std::string& message) {
        JDoc response{};
        response.SetString("jsonrpc", "2.0");
        response.SetInteger("error.code", code);
        response.SetString("error.message", message);
        if (id) {
            core::config::RapidJsonGeneric copiedId;
            copiedId.CopyFrom(id->base, response.GetAllocator());
            response.Set("id", std::move(copiedId));
        } else {
            response.SetNull("id");
        }
        WriteMessage(response);
    }

    void LanguageServer::PublishEmptyDiagnostics(JDocSub textDocument) {
        JDoc notification{};
        notification.SetString("jsonrpc", "2.0");
        notification.SetString("method", "textDocument/publishDiagnostics");

        JDocSub params{ notification.CreateSubVal("params") };
        params.SetString("uri", textDocument.GetString("uri"));
        params.Set("diagnostics", core::config::RapidJsonGeneric{}.SetArray());
        WriteMessage(notification);
    }

    void LanguageServer::RunLanguageServer() {
        (void)_setmode(_fileno(stdin), _O_BINARY);
        (void)_setmode(_fileno(stdout), _O_BINARY);

        if (debugFile) {
            df.Open(debugFile);
        }

        bool running{ true };
        std::string body;
        while (running && ReadMessage(body)) {
            JDoc request;
            if (!request.ParseDocument(body) || !request.main.IsObject()) {
                SendError(nullptr, lsp::RPC_ParseError, "Invalid JSON");
                continue;
            }
            DebugMessage("READ", request);

            JDocSub id{ request.GetSubVal("id") };
            JDocSub params{ request.GetSubVal("params") };
            const char* method{ request.GetCString("method") };

            if (!method || !*method) {
                if (!id) {
                    SendError(&id, lsp::RPC_InvalidRequest, "Invalid request");
                }
                continue;
            }

            switch (hash::HashX64(method)) {
            case "initialize"_x: {
                JDoc result{};
                result.SetString("serverInfo.name", std::format("acts-langs-{}", core::actsinfo::VERSION));

                JDocSub capabilities{ result.CreateSubVal("capabilities") };
                capabilities.SetInteger("textDocumentSync", lsp::TextDocumentSyncKind::TDSCK_Full);
                capabilities.SetBool("resolveProvider", false);
                capabilities.SetBool("hoverProvider", true);
                JDocSub completion{ capabilities.CreateSubVal("completionProvider") };

                core::config::RapidJsonGeneric triggerCharacters{ rapidjson::kArrayType };
                triggerCharacters.PushBack(".", result.GetAllocator());
                completion.Set("triggerCharacters", std::move(triggerCharacters));

                SendResponse(id, std::move(result.main));
                break;
            }
            case "exit"_x: {
                running = false;
                break;
            }
            case "textDocument/didOpen"_x:
            case "textDocument/didChange"_x: {
                OpenFile(params.GetSubVal("textDocument"));
                if (params.GetSubVal("contentChanges").base.IsArray()) {
                    ChangeFile(params);
                }
                PublishEmptyDiagnostics(params.GetSubVal("textDocument"));
                break;
            }
            case "textDocument/didClose"_x: {
                JDocSub textDocument{ params.GetSubVal("textDocument") };
                PublishEmptyDiagnostics(textDocument);
                CloseFile(textDocument);
                break;
            }
            case "textDocument/completion"_x: {
                TextDocument* doc{ GetTextDocument(params.GetSubVal("textDocument")) };

                JDoc result{};
                result.SetBool("isIncomplete", false);

                core::config::RapidJsonGeneric items{ rapidjson::kArrayType };
                JDocAllocatorType& allocator{ result.GetAllocator() };

                auto addCompletion = [&items, &allocator](
                                         const char* label,
                                         lsp::SymbolKind kind,
                                         const char* detail,
                                         const char* documentation,
                                         const char* insertText
                                     ) {
                    core::config::RapidJsonGeneric item{ rapidjson::kObjectType };
                    item.AddMember("label", core::config::RapidJsonGeneric{ label, allocator }, allocator);
                    item.AddMember("kind", kind, allocator);
                    item.AddMember("detail", core::config::RapidJsonGeneric{ detail, allocator }, allocator);
                    item.AddMember(
                        "documentation",
                        core::config::RapidJsonGeneric{ documentation, allocator },
                        allocator
                    );
                    item.AddMember("insertText", core::config::RapidJsonGeneric{ insertText, allocator }, allocator);
                    items.PushBack(std::move(item), allocator);
                };

                addCompletion(
                    "self",
                    lsp::SK_Constant,
                    doc ? "GSC keyword (open document)" : "GSC keyword",
                    "The current script object.",
                    "self"
                );
                addCompletion("function", lsp::SK_Namespace, "GSC function", "Mock function declaration.", "function");
                addCompletion(
                    "wait",
                    lsp::SK_Namespace,
                    "GSC builtin",
                    "Wait for the specified amount of time.",
                    "wait"
                );
                addCompletion(
                    "waittill",
                    lsp::SK_Namespace,
                    "GSC builtin",
                    "Wait until an event is received.",
                    "waittill"
                );
                addCompletion(
                    "endon",
                    lsp::SK_Namespace,
                    "GSC builtin",
                    "End the current notification handler.",
                    "endon"
                );

                result.Set("items", std::move(items));
                SendResponse(id, std::move(result.main));
                break;
            }
            case "textDocument/hover"_x: {
                JDoc result{};
                TextDocument* doc{ GetTextDocument(params.GetSubVal("textDocument")) };
                if (doc) {
                    result.SetString(
                        "contents.value",
                        std::format(
                            "{}:{}:{}",
                            doc->uri,
                            params.GetInteger("position.line"),
                            params.GetInteger("position.character")
                        )
                    );
                } else {
                    result.SetString("contents.value", "Invalid hover parameters");
                }

                result.SetString("contents.kind", "markdown");
                SendResponse(id, std::move(result.main));
                break;
            }
            case "$/setTrace"_x:
            case "initialized"_x:
            case "shutdown"_x: {
                break;
            }
            default: {
                SendError(&id, lsp::RPC_MethodNotFound, std::format("Method not found '{}'", method));
                break;
            }
            }
        }
    }
    namespace {
        int language_server(int argc, const char* argv[]) {
            LanguageServer lgs{};
            cli::options::CliOptions opts{};
            opts.addOption(&lgs.help, "show help", "--help", "", "-h");
            opts.addOption(&lgs.debugFile, "debug file", "--debug-file", " [dbg]");

            if (!opts.ComputeOptions(2, argc, argv) || lgs.help) {
                opts.PrintOptions();
                return lgs.help ? tool::OK : tool::BAD_USAGE;
            }

            lgs.RunLanguageServer();
            return tool::OK;
        }
#ifndef CI_BUILD
        int language_server_test(int argc, const char* argv[]) {

            JDoc notification{};
            notification.SetString("jsonrpc", "2.0");
            notification.SetString("method", "textDocument/publishDiagnostics");

            JDocSub params{ notification.CreateSubVal("params") };
            params.SetString("uri", "test uri");
            params.Set("diagnostics", core::config::RapidJsonGeneric{ rapidjson::kArrayType });

            rapidjson::StringBuffer buffer;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer{ buffer };
            notification.main.Accept(writer);
            LOG_INFO("{}", buffer.GetString());
            return tool::OK;
        }
        ADD_TOOL(language_server_test, "common", "", "", language_server_test);
#endif // CI_BUILD

        ADD_TOOL(language_server, "common", "", "acts language server", language_server);
    } // namespace
} // namespace tool::vscode