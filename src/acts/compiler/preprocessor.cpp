#include <includes.hpp>
#include <compiler/preprocessor.hpp>

namespace acts::compiler::preprocessor {
    void StringContainer::AddLineAt(const std::string& str, size_t start, size_t line) {
        auto it = blocks.begin();
        while (it != blocks.end()) {
            if (line >= it->startLine && line < it->startLine + it->sizeLine) {
                if (it->startLine == line) {
                    // TODO
                }

                return;
            }
            it++;
        }
        throw std::runtime_error(utils::va("Can't find line %lld", line));
    }

    const StringData& StringContainer::FindFile(size_t line) const {
        for (const StringData& f : blocks) {
            if (line >= f.startLine && line < f.startLine + f.sizeLine) {
                return f;
            }
        }
        return blocks[blocks.size() - 1];
    }

    void StringContainer::PrintLineMessage(core::logs::loglevel lvl, size_t line, size_t charPositionInLine, const std::string& msg) const {
        const StringData& f = FindFile(line);

        size_t localLine;

        if (line > f.startLine) {
            localLine = line - f.startLine;
        }
        else {
            localLine = f.sizeLine;
        }

        if (charPositionInLine) {
            LOG_LVLF(lvl, "{}#{}:{} {}", f.filename.string(), localLine, charPositionInLine, msg);
        }
        else {
            LOG_LVLF(lvl, "{}#{} {}", f.filename.string(), localLine, msg);
        }
    }

    bool StringContainer::AppendFile(const std::filesystem::path& filename) {
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

	namespace {
        size_t FindEndLineDelta(const char* d) {
            const char* s = d;
            while (*d && *d != '\n') {
                d++;
            }
            return d - s;
        }

        bool TrimDefineVal(std::string& val, bool allowInSpace = false) {
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
                if (!allowInSpace) {
                    for (size_t i = 0; i < val.length(); i++) {
                        if (isspace(val[i])) {
                            return false;
                        }
                    }
                }
                return true;
            }

            return false;
        }

        bool HasOnlySpaceAfter(const std::string_view& val, size_t idx) {
            for (size_t i = idx; i < val.length(); i++) {
                if (!isspace(val[i])) {
                    return false;
                }
            }
            return true;
        }

        inline void SetBlankChar(char& c) {
            if (!isspace(c)) {
                c = ' '; // keep same struct
            }
        }
    }
    bool PreProcessorOption::ApplyPreProcessorComments(std::string& str, std::function<void(core::logs::loglevel lvl, size_t line, const std::string& message)> errorHandler) {
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
                    errorHandler(core::logs::LVL_ERROR, 0, "No end for multiline comments");
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
                    errorHandler(core::logs::LVL_ERROR, 0, "No end for dev block");
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
    bool PreProcessorOption::ApplyPreProcessor(std::string& str, std::function<void(core::logs::loglevel lvl, size_t line, const std::string& message)> errorHandler) {
        if (!ApplyPreProcessorComments(str, errorHandler)) {
            return false;
        }
        currentDepth++;
        utils::CloseEnd depthCE{ [this]() { currentDepth--; } };
        if (currentDepth == 10) {
            errorHandler(core::logs::LVL_ERROR, 0, "Preprocessor depth too high, cyclic #insert?");
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
                    errorHandler(core::logs::LVL_ERROR, lineIdx, "#ifdef should be used with a parameter");
                    err = true;
                }
                else if (!TrimDefineVal(define)) {
                    errorHandler(core::logs::LVL_ERROR, lineIdx, "#ifdef should be used with one valid parameter");
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
                    errorHandler(core::logs::LVL_ERROR, lineIdx, "#ifndef should be used with a parameter");
                    err = true;
                }
                else if (!TrimDefineVal(define)) {
                    errorHandler(core::logs::LVL_ERROR, lineIdx, "#ifndef should be used with one valid parameter");
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
                    errorHandler(core::logs::LVL_ERROR, lineIdx, "#else can't have parameters");
                    err = true;
                }
                else if (eraseCtx.empty()) {
                    errorHandler(core::logs::LVL_ERROR, lineIdx, "Usage of #else without start if");
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
                    errorHandler(core::logs::LVL_ERROR, lineIdx, "#endif can't have parameters");
                    err = true;
                }
                else if (eraseCtx.empty()) {
                    errorHandler(core::logs::LVL_ERROR, lineIdx, "Usage of #endif without start if");
                    err = true;
                }
                else {
                    eraseCtx.pop();
                }
            }
            else if (eraseCtx.empty() || !eraseCtx.top()) {
                if (!noDefineExpr && line.starts_with("#define")) {
                    std::string define{ line.substr(7) };
                    if (define.length() < 1 || !isspace(define[0])) {
                        errorHandler(core::logs::LVL_ERROR, lineIdx, "#define should be used with a parameter");
                        err = true;
                    }
                    else if (!TrimDefineVal(define)) {
                        errorHandler(core::logs::LVL_ERROR, lineIdx, "#define should be used with one valid parameter");
                        err = true;
                    }
                    else {
                        defines.insert(define);
                    }
                }
                else if (line.starts_with("#error")) {
                    errorHandler(core::logs::LVL_ERROR, lineIdx, std::string{ line.substr(line.length() > 6 ? 7 : 6) });
                    err = true;
                }
                else if (line.starts_with("#warning")) {
                    errorHandler(core::logs::LVL_WARNING, lineIdx, std::string{ line.substr(line.length() > 8 ? 9 : 8) });
                }
                else if (line.starts_with("#insert")) {
                    std::string filename{ line.substr(7) };
                    if (filename.length() < 1 || !isspace(filename[0])) {
                        errorHandler(core::logs::LVL_ERROR, lineIdx, "#insert should be used with a parameter");
                        err = true;
                    }
                    else if (!TrimDefineVal(filename, true)) {
                        errorHandler(core::logs::LVL_ERROR, lineIdx, "#insert should be used with one valid parameter");
                        err = true;
                    }
                    else {
                        std::string insertData{};

                        size_t end{ filename.size() };
                        while (end && (filename[end - 1] == ';' || isspace(filename[end - 1]))) {
                            end--;
                        }
                        if (end != filename.size()) {
                            filename.resize(end); // trim the end ';
                        }

                        std::filesystem::path insertPath{ cwd / filename };

                        if (!utils::ReadFile(insertPath, insertData)) {
                            errorHandler(core::logs::LVL_ERROR, lineIdx, std::format("can't read insert '{}'", insertPath.string()));
                            err = true;
                        }
                        else if (!ApplyPreProcessor(insertData, [&errorHandler, &filename, lineIdx](core::logs::loglevel lvl, size_t line, const std::string& message) {
                            errorHandler(lvl, lineIdx, std::format("[{}:{}] {}", filename, line, message));
                            })) {
                            errorHandler(core::logs::LVL_ERROR, lineIdx, std::format("can't parse insert '{}'", insertPath.string()));
                            err = true;
                        }
                        else {
                            for (char* pInsertData = insertData.data(); *pInsertData; pInsertData++) {
                                // remove new lines, .gsh files shouldn't contain function code
                                if (*pInsertData == '\r' || *pInsertData == '\n') {
                                    *pInsertData = ' ';
                                }
                            }
                            str.replace(str.begin() + lineStart, str.begin() + next, insertData);
                        }
                    }
                }
                else if (!line.starts_with("#region") && !line.starts_with("#endregion")) {
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
            errorHandler(core::logs::LVL_ERROR, lineIdx, "end of file before the end of all the if");
            return false;
        }

        return !err;
    }

    bool PreProcessorOption::ApplyPreProcessor(std::string& str, const char* filename) {
        if (filename) {
            return ApplyPreProcessor(str, [filename](core::logs::loglevel lvl, size_t line, const std::string& message) { LOG_LVLF(lvl, "[{}:{}] {}", filename, line, message); });
        }
        else {
            return ApplyPreProcessor(str, [](core::logs::loglevel lvl, size_t line, const std::string& message) { LOG_LVLF(lvl, "[line:{}] {}", line, message); });
        }
    }
}