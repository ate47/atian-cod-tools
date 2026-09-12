#pragma once
#include <core/config.hpp>
#include <tools/vscode/lsp.hpp>

namespace tool::vscode {
    using JDocAllocatorType = core::config::RapidJsonAllocatorType;
    using JDoc = core::config::JDoc;
    using JDocSub = JDoc::ConfigGenericType;

    struct TextDocument {
        std::string uri;
        std::string text;
    };

    struct LanguageServer {
        bool help{};
        const char* debugFile{};
        std::unordered_map<std::string, TextDocument> docs{};
        utils::OutFileCE df{};

        void OpenFile(JDocSub ev);
        void ChangeFile(JDocSub ev);
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