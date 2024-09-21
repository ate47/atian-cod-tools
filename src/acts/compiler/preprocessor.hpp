#pragma once
#include <includes.hpp>

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

        void AddLineAt(const std::string& str, size_t start, size_t line) {
            auto it = blocks.begin();
            while (it != blocks.end()) {
                if (line >= it->startLine && line < it->startLine + it->sizeLine) {
                    if (it->startLine == line) {
                        // TODO
                    }


                    return;
                }
            }
            throw std::runtime_error(utils::va("Can't find line %lld", line));
        }

        const StringData& FindFile(size_t line) const {
            for (const StringData& f : blocks) {
                if (line >= f.startLine && line < f.startLine + f.sizeLine) {
                    return f;
                }
            }
            return blocks[blocks.size() - 1];
        }

        void PrintLineMessage(alogs::loglevel lvl, size_t line, size_t charPositionInLine, const std::string& msg) const {
            const StringData& f = FindFile(line);

            size_t localLine;

            if (line > f.startLine) {
                localLine = line - f.startLine;
            }
            else {
                localLine = f.sizeLine;
            }

            if (charPositionInLine) {
                LOG_LVL(lvl, "{}#{}:{} {}", f.filename.string(), localLine, charPositionInLine, msg);
            }
            else {
                LOG_LVL(lvl, "{}#{} {}", f.filename.string(), localLine, msg);
            }
        }

        bool AppendFile(const std::filesystem::path& filename) {
            size_t start = data.size();
            size_t startLine = currentLines;
            // GscFile
            preprocessor::StringData& dt = blocks.emplace_back(filename, start, startLine, 0);
            std::string& buff = buffers.emplace_back();

            if (!utils::ReadFile(filename, buff)) {
                return false;
            }

            size_t lineCount{ 1 }; // 1 for the one we'll add at the end

            const char* b = buff.data();
            while (*b) {
                if (*(b++) == '\n') {
                    lineCount++;
                }
            }

            dt.sizeLine = lineCount;
            currentLines = startLine + lineCount;
            data = data + buff + "\n";
            return true;
        }
    };

    struct PreProcessorOption {
        std::unordered_set<std::string> defines{};
        bool devBlockAsComment{};
        bool noDefineExpr{};

        static size_t FindEndLineDelta(const char* d) {
            const char* s = d;
            while (*d && *d != '\n') {
                d++;
            }
            return d - s;
        }

        static bool TrimDefineVal(std::string& val) {
            if (val.empty()) {
                return false;
            }
            size_t start{};
            size_t end{ val.length() };

            while (start < val.length() && isspace(val[start])) {
                start++;
            }

            while (end > start && isspace(val[end - 1])) {
                end--;
            }

            if (end > start) {
                val = val.substr(start, end - start);
                for (size_t i = 0; i < val.length(); i++) {
                    if (isspace(val[i])) {
                        return false;
                    }
                }
                return true;
            }

            return false;
        }

        static bool HasOnlySpaceAfter(const std::string_view& val, size_t idx) {
            for (size_t i = idx; i < val.length(); i++) {
                if (!isspace(val[i])) {
                    return false;
                }
            }
            return true;
        }

        static inline void SetBlankChar(char& c) {
            if (!isspace(c)) {
                c = ' '; // keep same struct
            }
        }

        bool ApplyPreProcessorComments(std::string& str, std::function<void(alogs::loglevel lvl, size_t line, const std::string& message)> errorHandler) {
            size_t idx{};
            char* data = str.data();
            char* dataEnd = data + str.length();

            while (*data) {
                char c = data[0];

                if (c == '"') {
                    // skip string
                    data++;
                    while (data != dataEnd) {
                        if (data[0] == '\\') {
                            data++;
                            if (data == dataEnd) {
                                break; // invalid \?
                            }
                        }
                        else if (data[0] == '"') {
                            data++;
                            break;
                        }
                        data++;
                    }
                }
                else if (c == '/' && data[1] == '/') {
                    // skip line comment

                    SetBlankChar(*(data++)); // /
                    do {
                        SetBlankChar(*(data++));
                    } while (data != dataEnd && *data != '\n');
                }
                else if (c == '/' && data[1] == '*') {
                    // skip comment
                    SetBlankChar(*(data++)); // /

                    do {
                        SetBlankChar(*(data++));
                    } while (data != dataEnd && !(data[0] == '*' && data[1] == '/'));

                    if (data == dataEnd) {
                        errorHandler(alogs::LVL_ERROR, 0, "No end for multiline comments");
                        return false;
                    }
                    SetBlankChar(*(data++));
                    SetBlankChar(*(data++)); // */
                }
                else if (devBlockAsComment && c == '/' && data[1] == '#') {
                    // skip dev blocks for retarded people
                    SetBlankChar(*(data++)); // /

                    do {
                        SetBlankChar(*(data++));
                    } while (data != dataEnd && !(data[0] == '#' && data[1] == '/'));

                    if (data == dataEnd) {
                        errorHandler(alogs::LVL_ERROR, 0, "No end for dev block");
                        return false;
                    }
                    SetBlankChar(*(data++));
                    SetBlankChar(*(data++)); // #/
                }
                else {
                    data++;
                }
            }

            return true;
        }

        bool ApplyPreProcessor(std::string& str, std::function<void(alogs::loglevel lvl, size_t line, const std::string& message)> errorHandler) {
            if (!ApplyPreProcessorComments(str, errorHandler)) {
                return false;
            }
            size_t lineStart{};
            size_t lineIdx{};
            bool err{};

            std::stack<bool> eraseCtx{};

            while (lineStart < str.length()) {
                lineIdx++;
                size_t next = str.find("\n", lineStart);

                if (next == std::string::npos) {
                    next = str.length(); // last line
                }

                // skip until the start
                while (lineStart < next && isspace(str[lineStart])) {
                    lineStart++;
                }

                std::string_view line{ str.data() + lineStart, str.data() + next };
                if (line.starts_with("#ifdef")) {
                    std::string define{ line.substr(6) };
                    if (define.length() < 1 || !isspace(define[0])) {
                        errorHandler(alogs::LVL_ERROR, lineIdx, "#ifdef should be used with a parameter");
                        err = true;
                    }
                    else if (!TrimDefineVal(define)) {
                        errorHandler(alogs::LVL_ERROR, lineIdx, "#ifdef should be used with one valid parameter");
                        err = true;
                    }
                    else {
                        bool forceDel = eraseCtx.size() && eraseCtx.top();

                        if (!forceDel) {
                            forceDel = !defines.contains(define);
                        }
                        eraseCtx.push(forceDel);
                    }
                }
                else if (line.starts_with("#ifndef")) {
                    std::string define{ line.substr(7) };
                    if (define.length() < 1 || !isspace(define[0])) {
                        errorHandler(alogs::LVL_ERROR, lineIdx, "#ifndef should be used with a parameter");
                        err = true;
                    }
                    else if (!TrimDefineVal(define)) {
                        errorHandler(alogs::LVL_ERROR, lineIdx, "#ifndef should be used with one valid parameter");
                        err = true;
                    }
                    else {
                        bool del = eraseCtx.size() && eraseCtx.top();

                        if (!del) {
                            del = defines.contains(define);
                        }
                        eraseCtx.push(del);
                    }
                }
                else if (line.starts_with("#else")) {
                    if (!HasOnlySpaceAfter(line, 5)) {
                        errorHandler(alogs::LVL_ERROR, lineIdx, "#else can't have parameters");
                        err = true;
                    }
                    else if (eraseCtx.empty()) {
                        errorHandler(alogs::LVL_ERROR, lineIdx, "Usage of #else without start if");
                        err = true;
                    }
                    else {
                        bool curr = eraseCtx.top();
                        eraseCtx.pop();

                        if (!eraseCtx.empty()) {
                            // at least 2, we need to check the parent ctx
                            if (!eraseCtx.top()) {
                                eraseCtx.push(false);
                            }
                            else {
                                eraseCtx.push(!curr);
                            }
                        }
                        else {
                            eraseCtx.push(!curr);
                        }
                    }
                }
                else if (line.starts_with("#endif")) {
                    if (!HasOnlySpaceAfter(line, 6)) {
                        errorHandler(alogs::LVL_ERROR, lineIdx, "#endif can't have parameters");
                        err = true;
                    }
                    else if (eraseCtx.empty()) {
                        errorHandler(alogs::LVL_ERROR, lineIdx, "Usage of #endif without start if");
                        err = true;
                    }
                    else {
                        eraseCtx.pop();
                    }
                }
                else if (!noDefineExpr && line.starts_with("#define")) {
                    std::string define{ line.substr(7) };
                    if (define.length() < 1 || !isspace(define[0])) {
                        errorHandler(alogs::LVL_ERROR, lineIdx, "#define should be used with a parameter");
                        err = true;
                    }
                    else if (!TrimDefineVal(define)) {
                        errorHandler(alogs::LVL_ERROR, lineIdx, "#define should be used with one valid parameter");
                        err = true;
                    }
                    else {
                        defines.insert(define);
                    }
                }
                else if (line.starts_with("#error")) {
                    errorHandler(alogs::LVL_ERROR, lineIdx, std::string{ line.substr(line.length() > 6 ? 7 : 6) });
                    err = true;
                }
                else if (line.starts_with("#warning")) {
                    errorHandler(alogs::LVL_WARNING, lineIdx, std::string{ line.substr(line.length() > 8 ? 9 : 8) });
                }
                else if (!line.starts_with("#region") && !line.starts_with("#endregion")) {
                    if (eraseCtx.empty() || !eraseCtx.top()) {
                        lineStart = next + 1;
                        continue;
                    }
                }


                for (size_t i = lineStart; i < next; i++) {
                    SetBlankChar(str[i]);
                }

                lineStart = next + 1;
            }

            if (!eraseCtx.empty()) {
                errorHandler(alogs::LVL_ERROR, lineIdx, "end of file before the end of all the if");
                return false;
            }

            return !err;
        }
    };
}