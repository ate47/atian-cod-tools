#include <includes.hpp>
#include <core/actsinfo.hpp>
#include <cli/cli_options.hpp>
#include <tools/vscode/acts_vs.hpp>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>
#include <fcntl.h>
#include <io.h>

// compiler
#include <tools/gsc/compiler/gsc_compiler_grammar.hpp>

namespace tool::vscode {
    using namespace antlr4;
    using namespace tool::gsc::compiler;
    using namespace hash::literals;

    constexpr const char* JSON_RPC_VERSION = "2.0";

    class ACTSErrorListener : public ConsoleErrorListener {
        ErrorMsgHandler& errorHandler;

      public:
        ACTSErrorListener(ErrorMsgHandler& errorHandler) : errorHandler(errorHandler) {}

        void syntaxError(
            Recognizer* recognizer, Token* offendingSymbol, size_t line, size_t charPositionInLine,
            const std::string& msg, std::exception_ptr e
        ) override {
            if (!offendingSymbol) {
                // no token, use single char
                errorHandler(core::logs::LVL_ERROR, line, charPositionInLine, line, charPositionInLine + 1, msg);
                return;
            }

            std::string tokenText{ offendingSymbol->getText() };
            size_t first{ 0 };
            size_t endLine{ line };
            while (first < tokenText.length()) {
                size_t nl{ tokenText.find_first_of('\n', first) };
                if (nl == std::string::npos) {
                    break;
                }
                endLine++;
                first = nl + 1;
            }

            size_t endChar;
            if (endLine != line) {
                endChar = tokenText.length() - first;
            } else {
                endChar = charPositionInLine + tokenText.length();
            }

            errorHandler(core::logs::LVL_ERROR, line, charPositionInLine, endLine, endChar, msg);
        }
    };

    static void ProcessFile(const std::string& buff, ErrorMsgHandler errorHandler) {
        // temp test for grammar parsing
        core::preprocessor::PreProcessorOption popt{};

        std::string str{ buff };

        popt.ApplyPreProcessor(
            str,
            [&errorHandler](core::logs::loglevel lvl, size_t line, const std::string& message) -> void {
                errorHandler(lvl, line, 0, line + 1, 0, message); // error the whole line
            }
        );
        ANTLRInputStream is{ str };

        std::unique_ptr<ACTSErrorListener> errList{ std::make_unique<ACTSErrorListener>(errorHandler) };

        gscLexer lexer{ &is };
        lexer.addErrorListener(&*errList);
        CommonTokenStream tokens{ &lexer };

        tokens.fill();
        gscParser parser{ &tokens };

        parser.removeErrorListeners();

        parser.addErrorListener(&*errList);

        gscParser::ProgContext* prog = parser.prog();
    }

    void TextDocument::SetText(std::string_view view, ErrorMsgHandler& errorHandler) {
        text = view;
        ProcessFile(text, errorHandler);
    }

    void LanguageServer::OpenFile(JDocSub ev, ErrorMsgHandler& errorHandler) {
        const char* uri{ ev.GetCString("uri") };
        const char* text{ ev.GetCString("text") };

        if (uri && text) {
            TextDocument& doc{ docs[uri] };
            doc.uri = uri;
            doc.SetText(text, errorHandler);
        }
    }

    void LanguageServer::ChangeFile(JDocSub ev, ErrorMsgHandler& errorHandler) {
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
                doc.SetText(text, errorHandler);
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
        response.SetString("jsonrpc", JSON_RPC_VERSION);
        JDocAllocatorType& allocator{ response.GetAllocator() };
        core::config::RapidJsonGeneric copiedId;
        copiedId.CopyFrom(id.base, allocator);
        response.main.AddMember("id", std::move(copiedId), allocator);
        response.main.AddMember("result", std::move(result), allocator);
        WriteMessage(response);
    }

    void LanguageServer::SendError(const JDocSub* id, lsp::JsonRPCError code, const std::string& message) {
        JDoc response{};
        response.SetString("jsonrpc", JSON_RPC_VERSION);
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
        notification.SetString("jsonrpc", JSON_RPC_VERSION);
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
                JDoc notification{};
                core::config::RapidJsonGeneric diagnostics{};
                diagnostics.SetArray();
                core::config::RapidJsonGenericArray arr{ diagnostics.GetArray() };

                ErrorMsgHandler err = [&arr, &notification](
                                          core::logs::loglevel lvl,
                                          size_t startLine,
                                          size_t startCharPositionInLine,
                                          size_t endLine,
                                          size_t endCharPositionInLine,
                                          const std::string& message
                                      ) -> void {
                    lsp::DiagnosticSeverity severity;
                    switch (lvl) {
                    case core::logs::loglevel::LVL_DEBUG:
                    case core::logs::loglevel::LVL_TRACE:
                    case core::logs::loglevel::LVL_TRACE_PATH:
                        return; // ignored
                    case core::logs::loglevel::LVL_ERROR:
                        severity = lsp::DiagnosticSeverity::DS_Error;
                        break;
                    case core::logs::loglevel::LVL_WARNING:
                        severity = lsp::DiagnosticSeverity::DS_Warning;
                        break;
                    case core::logs::loglevel::LVL_INFO:
                    default:
                        severity = lsp::DiagnosticSeverity::DS_Information;
                        break;
                    }
                    core::config::RapidJsonGeneric d{};
                    d.SetObject();
                    JDocSub diag{ notification.GetSub(d) };

                    diag.SetInteger("range.start.line", startLine - 1);
                    diag.SetInteger("range.start.character", startCharPositionInLine);
                    diag.SetInteger("range.end.line", endLine - 1);
                    diag.SetInteger("range.end.character", endCharPositionInLine);
                    diag.SetString("message", message);
                    diag.SetInteger("severity", severity);

                    arr.PushBack(std::move(d), notification.GetAllocator());
                };

                OpenFile(params.GetSubVal("textDocument"), err);
                ChangeFile(params, err);
                notification.SetString("jsonrpc", JSON_RPC_VERSION);
                notification.SetString("method", "textDocument/publishDiagnostics");

                JDocSub dparams{ notification.CreateSubVal("params") };
                dparams.SetString("uri", params.GetString("textDocument.uri"));
                dparams.Set("diagnostics", std::move(diagnostics));
                WriteMessage(notification);
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
            notification.SetString("jsonrpc", JSON_RPC_VERSION);
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