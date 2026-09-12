#pragma once
#include <core/config.hpp>
#include <tools/vscode/lsp.hpp>

namespace tool::vscode {
    using JDocAllocatorType = core::config::RapidJsonAllocatorType;
    using JDoc = core::config::JDoc;
    using JDocSub = JDoc::ConfigGenericType;

    using ErrorMsgHandler = std::function<void(
        core::logs::loglevel lvl, size_t startLine, size_t startCharPositionInLine, size_t endLine,
        size_t endCharPositionInLine, const std::string& message
    )>;

    struct TextDocument {
        std::string uri;
        std::string text;

        void SetText(std::string_view view, ErrorMsgHandler& errorHandler);
    };

    class LanguageServer {
      public:
        bool help{};
        const char* debugFile{};
        std::unordered_map<std::string, TextDocument> docs{};
        utils::OutFileCE df{};

        LanguageServer() = default;
        LanguageServer(const LanguageServer&) = delete;
        LanguageServer(LanguageServer&&) = delete;

        void OpenFile(JDocSub ev, ErrorMsgHandler& errorHandler);
        void ChangeFile(JDocSub ev, ErrorMsgHandler& errorHandler);
        void CloseFile(JDocSub textDocument);
        TextDocument* GetTextDocument(JDocSub textDocument);
        void WriteMessage(const JDoc& message);
        bool ReadMessage(std::string& body);
        void SendResponse(const JDocSub& id, core::config::RapidJsonGeneric&& result);
        void SendError(const JDocSub* id, lsp::JsonRPCError code, const std::string& message);
        void PublishEmptyDiagnostics(JDocSub textDocument);
        void RunLanguageServer();
        void DebugMessage(const char* type, const JDoc& message);
    };
} // namespace tool::vscode