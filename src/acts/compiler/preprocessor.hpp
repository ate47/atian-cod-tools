#pragma once

namespace acts::compiler::preprocessor {
    struct StringData {
        const std::filesystem::path& filename;
        size_t start;
        size_t startLine;
        size_t sizeLine;
        size_t fileoffset{};
    };

    struct StringContainer {
        std::vector<StringData> blocks{};
        std::vector<std::string> buffers{};
        std::string data{};
        size_t currentLines{};

        void AddLineAt(const std::string& str, size_t start, size_t line);

        const StringData& FindFile(size_t line) const;

        void PrintLineMessage(core::logs::loglevel lvl, size_t line, size_t charPositionInLine, const std::string& msg) const;

        bool AppendFile(const std::filesystem::path& filename);
    };

    struct PreProcessorOption {
        std::unordered_map<std::string, std::string> defines{};
        bool devBlockAsComment{};
        bool noDefineExpr{};
        size_t currentDepth{};
        std::filesystem::path cwd{ std::filesystem::current_path() };

        bool AddDefineConfig(const std::string& config);

        void AddDefine(const std::string& name, const std::string& value = "");

        void RemoveDefine(const char* name);

        inline void RemoveDefine(const std::string& name) { RemoveDefine(name.data()); }

        void ReplaceDefines(std::string& str, size_t start, size_t* end);

        bool ApplyPreProcessorComments(std::string& str, std::function<void(core::logs::loglevel lvl, size_t line, const std::string& message)> errorHandler);

        bool ApplyPreProcessor(std::string& str, std::function<void(core::logs::loglevel lvl, size_t line, const std::string& message)> errorHandler);
        
        bool ApplyPreProcessor(std::string& str, const char* filename = nullptr);
    };
}