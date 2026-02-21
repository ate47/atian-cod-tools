#include <includes.hpp>
#include <tools/gsc/compiler/gsc_compiler_parser.hpp>
#include <tools/gsc/compiler/gsc_compiler_script_object.hpp>
#include <gsc/gsc_acts_debug.hpp>
#include <utils/crc.hpp>

namespace tool::gsc::compiler {
    static NumberOpCodesS NumberOpCodes[]{
        { OPCODE_GetZero, 0, 0, 0, false, false },
        { OPCODE_GetNegByte, -0xFF, 0, 1, true, true  },
        { OPCODE_GetSignedByte, -0x80, 0x7F, 1, false, false },
        { OPCODE_GetByte, 0, 0xFF, 1, false, true  },
        { OPCODE_GetUnsignedShort, 0, 0xFFFF, 2, false, true },
        { OPCODE_GetNegUnsignedShort, -0xFFFF, 0, 2, true, true  },
        { OPCODE_GetShort, -0x8000, 0x7FFF, 2, false, false },
        { OPCODE_GetNegUnsignedInteger, -0xFFFFFFFFLL, 0, 4, true, true },
        { OPCODE_GetInteger, -0x80000000LL, 0x7FFFFFFF, 4, false, false },
        { OPCODE_GetUnsignedInteger, 0, 0xFFFFFFFF, 4, false, true },
        { OPCODE_GetLongInteger, (-0x7FFFFFFFFFFFFFFFLL - 1LL), 0x7FFFFFFFFFFFFFFF, 8, false, false },
    };

    CompileObject::CompileObject(CompilerConfig& config, GscFileType file, InputInfo& nfo, std::shared_ptr<tool::gsc::GSCOBJHandler> gscHandler)
        : config(config), type(file), info(nfo), vmInfo(config.GetVm()), gscHandler(gscHandler) {
        fileName = vmInfo->HashPath(config.name);
        const char* fns = config.fileName;
        if (fns && *fns) {
            fileNameSpace = vmInfo->HashFilePath(fns);
            if (config.hashes) {
                config.hashes->insert(fns);
            }
        }

        currentNamespace = fileNameSpace;
    }

    StringObject& CompileObject::RegisterString(const char* str) {
        StringObject& obj{ strings[str] };

        if (obj.nodes.empty()) {
            obj.devString = devBlockDepth > 0;
        }
        else if (obj.devString) {
            // we need to check this string is still a dev one
            obj.devString = devBlockDepth > 0;
        }

        return obj;
    }

    StringObject& CompileObject::RegisterString(const std::string& str) {
        return RegisterString(str.data());
    }

    void CompileObject::AddHash(const char* str) {
        if (!hash::HashPattern(str)) {
            hashes.insert(str);
            hashutils::Add(str);
            if (config.hashes) config.hashes->insert(str);
        }
    }

    void CompileObject::AddHash(const std::string& str) {
        AddHash(str.c_str());
    }

    void CompileObject::AddInclude(const std::string& data) {
        includes.insert(data);
    }

    bool CompileObject::HasOpCode(OPCode opcode) {
        return tool::gsc::opcode::HasOpCode(vmInfo->vmMagic, config.platform, opcode, config.useModToolOpCodes);
    }


    TerminalNode* CompileObject::GetUniqueNode(ParseTree* tree, bool error) {
        if (tree->getTreeType() == TREE_ERROR) {
            if (error) {
                info.PrintLineMessage(core::logs::LVL_ERROR, tree, std::format("Tree error for GetUniqueNode: {}", tree->getText()));
            }
            return nullptr;
        }
        if (tree->getTreeType() == TREE_TERMINAL) {
            return dynamic_cast<TerminalNode*>(tree); // done
        }
        RuleContext* rule = dynamic_cast<RuleContext*>(tree);

        if (rule->getRuleIndex() == gscParser::RuleExpression15) {
            return GetUniqueNode(rule->children[1], error);
        }

        if (rule->children.size() != 1) {
            if (error) {
                info.PrintLineMessage(core::logs::LVL_ERROR, tree, std::format("Not a valid terminal node: {}", rule->getText()));
            }
            return nullptr;
        }

        return GetUniqueNode(rule->children[0], error);
    }

    float CompileObject::FloatNumberNodeValue(ParseTree* number, bool error) {
        if (IS_IDF(number)) {
            std::string varName = number->getText();
            auto ceit = constexprs.find(hash::Hash64(varName.c_str()));

            if (ceit != constexprs.end()) {
                return FloatNumberNodeValue(ceit->second, error);
            }
        }

        if (number && number->getTreeType() == TREE_RULE) {
            RuleContext* rule = dynamic_cast<RuleContext*>(number);
            switch (rule->getRuleIndex()) {
            case gscParser::RuleExpression15:
                return FloatNumberNodeValue(rule->children[1], error);
            case gscParser::RuleExpression:
            case gscParser::RuleExpression0:
            case gscParser::RuleExpression1:
            case gscParser::RuleExpression2:
            case gscParser::RuleExpression3:
            case gscParser::RuleExpression4:
            case gscParser::RuleExpression5:
            case gscParser::RuleExpression6:
            case gscParser::RuleExpression7:
            case gscParser::RuleExpression8:
            case gscParser::RuleExpression9:
            case gscParser::RuleExpression10:
            case gscParser::RuleExpression11:
            case gscParser::RuleExpression12:
            case gscParser::RuleExpression13:
            case gscParser::RuleExpression14:
            case gscParser::RuleNumber:
            case gscParser::RuleConst_expr:
            case gscParser::RuleConst_expr_static:
            case gscParser::RuleIdf: {
                if (number->children.size() == 1) {
                    return FloatNumberNodeValue(number->children[0], error);
                }
            }
            default:
                if (error) {
                    info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("Not a valid const number: {} ({})", rule->getText(), rule->getRuleIndex()));
                }
                return NAN;
            }
        }

        if (!number || number->getTreeType() != TREE_TERMINAL) {
            if (error) {
                info.PrintLineMessage(core::logs::LVL_ERROR, number, std::format("Not a valid const number: {}", number->getText()));
            }
            return NAN; // wtf?
        }

        TerminalNode* term = dynamic_cast<TerminalNode*>(number);
        switch (term->getSymbol()->getType()) {
        case gscParser::INTEGER10:
            return (float)std::strtoll(term->getText().c_str(), nullptr, 10);
        case gscParser::INTEGER16: {
            bool neg = term->getText()[0] == '-';
            int64_t val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), nullptr, 16);
            return (float)(neg ? -val : val);
        }
        case gscParser::INTEGER8: {
            bool neg = term->getText()[0] == '-';
            int64_t val = std::strtoll(term->getText().c_str() + (neg ? 2 : 1), nullptr, 8);
            return (float)(neg ? -val : val);
        }
        case gscParser::INTEGER2: {
            bool neg = term->getText()[0] == '-';
            int64_t val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), nullptr, 2);
            return (float)(neg ? -val : val);
        }
        case gscParser::FLOATVAL: {
            return (float)std::strtof(term->getText().c_str(), nullptr);
        }
        default:
            return NAN;
        }
    }

    int64_t CompileObject::NumberNodeValue(ParseTree* number, bool error, bool* extracted) {
        if (IS_IDF(number)) {
            std::string varName = number->getText();
            auto ceit = constexprs.find(hash::Hash64(varName.c_str()));

            if (ceit != constexprs.end()) {
                return NumberNodeValue(ceit->second, error, extracted);
            }
        }

        if (number && number->getTreeType() == TREE_RULE) {
            RuleContext* rule = dynamic_cast<RuleContext*>(number);
            switch (rule->getRuleIndex()) {
            case gscParser::RuleExpression15:
                return NumberNodeValue(rule->children[1], error, extracted);
            case gscParser::RuleExpression:
            case gscParser::RuleExpression0:
            case gscParser::RuleExpression1:
            case gscParser::RuleExpression2:
            case gscParser::RuleExpression3:
            case gscParser::RuleExpression4:
            case gscParser::RuleExpression5:
            case gscParser::RuleExpression6:
            case gscParser::RuleExpression7:
            case gscParser::RuleExpression8:
            case gscParser::RuleExpression9:
            case gscParser::RuleExpression10:
            case gscParser::RuleExpression11:
            case gscParser::RuleExpression12:
            case gscParser::RuleExpression13:
            case gscParser::RuleExpression14:
            case gscParser::RuleNumber:
            case gscParser::RuleConst_expr:
            case gscParser::RuleConst_expr_static:
            case gscParser::RuleIdf: {
                if (number->children.size() == 1) {
                    return NumberNodeValue(number->children[0], error, extracted);
                }
                else if (number->children.size() == 3) {
                    bool textracted{};
                    int64_t v1 = NumberNodeValue(number->children[0], error, &textracted);
                    if (textracted) {
                        int64_t v2 = NumberNodeValue(number->children[2], error, &textracted);
                        if (textracted) {
                            if (extracted) *extracted = true;
                            std::string op = number->children[1]->getText();
                            if (op == "??") return v1 ? v1 : v2;
                            if (op == "&&") return v1 && v2 ? 1 : 0;
                            if (op == "||") return v1 || v2 ? 1 : 0;
                            if (op == "<") return v1 < v2 ? 1 : 0;
                            if (op == ">") return v1 > v2 ? 1 : 0;
                            if (op == "==" || op == "===") return v1 == v2 ? 1 : 0;
                            if (op == "<=" || op == "<==") return v1 <= v2 ? 1 : 0;
                            if (op == ">=" || op == ">==") return v1 >= v2 ? 1 : 0;
                            if (op == "+") return v1 + v2;
                            if (op == "-") return v1 - v2;
                            if (op == "/") return v1 / v2;
                            if (op == "*") return v1 * v2;
                            if (op == "%") return v1 % v2;
                            if (op == "|") return v1 | v2;
                            if (op == "^") return v1 ^ v2;
                            if (op == "<<") return v1 << v2;
                            if (op == ">>") return v1 >> v2;
                            if (op == "&") return v1 & v2;
                            if (extracted) *extracted = false;
                        }
                    }
                }
                else if (number->children.size() == 2 && rule->children[0] && rule->children[0]->getTreeType() == TREE_TERMINAL) {
                    bool textracted{};
                    int64_t v = NumberNodeValue(number->children[0], error, &textracted);
                    if (textracted) {
                        std::string op = number->children[0]->getText();
                        if (extracted) *extracted = true;
                        if (op == "!") return !v ? 1 : 0;
                        if (op == "~") return ~v;
                        if (extracted) *extracted = false;
                    }

                }
            }
            default:
                if (error) {
                    info.PrintLineMessage(core::logs::LVL_ERROR, rule, std::format("Not a valid const number: {} ({})", rule->getText(), rule->getRuleIndex()));
                }
                if (extracted) *extracted = false;
                return 0;
            }
        }

        if (!number || number->getTreeType() != TREE_TERMINAL) {
            if (error) {
                info.PrintLineMessage(core::logs::LVL_ERROR, number, std::format("Not a valid const number: {}", number->getText()));
            }
            if (extracted) *extracted = false;
            return 0; // wtf?
        }
        if (extracted) *extracted = true;

        TerminalNode* term = dynamic_cast<TerminalNode*>(number);
        switch (term->getSymbol()->getType()) {
        case gscParser::INTEGER10:
            return std::strtoll(term->getText().c_str(), nullptr, 10);
        case gscParser::INTEGER16: {
            bool neg = term->getText()[0] == '-';
            int64_t val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), nullptr, 16);
            return neg ? -val : val;
        }
        case gscParser::INTEGER8: {
            bool neg = term->getText()[0] == '-';
            int64_t val = std::strtoll(term->getText().c_str() + (neg ? 2 : 1), nullptr, 8);
            return neg ? -val : val;
        }
        case gscParser::INTEGER2: {
            bool neg = term->getText()[0] == '-';
            int64_t val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), nullptr, 2);
            return neg ? -val : val;
        }
        case gscParser::BOOL_VALUE: {
            return term->getText() == "true" ? 1 : 0;
        }
        default:
            if (extracted) *extracted = false;
            return 0;
        }
    }

    AscmNodeOpCode* CompileObject::BuildAscmNodeData(const NumberOpCodesS& opcode, int64_t val) const {
        if (opcode.minValue > val || opcode.maxValue < val) {
            info.PrintLineMessage(core::logs::LVL_WARNING, nullptr, std::format("opcode not big enough to store the value {}", val));
        }
        if (opcode.negat) {
            val = -val;
        }
        switch (opcode.sizeOf) {
        case 0:
            return new AscmNodeOpCode(opcode.opcode);
        case 1:
            return opcode.unsign ? (AscmNodeOpCode*)new AscmNodeData<uint8_t>((uint8_t)val, opcode.opcode) : new AscmNodeData<int8_t>((int8_t)val, opcode.opcode);
        case 2:
            return opcode.unsign ? (AscmNodeOpCode*)new AscmNodeData<uint16_t>((uint16_t)val, opcode.opcode) : new AscmNodeData<int16_t>((int16_t)val, opcode.opcode);
        case 4:
            return opcode.unsign ? (AscmNodeOpCode*)new AscmNodeData<uint32_t>((uint32_t)val, opcode.opcode) : new AscmNodeData<int32_t>((int32_t)val, opcode.opcode);
        case 8:
            return opcode.unsign ? (AscmNodeOpCode*)new AscmNodeData<uint64_t>((uint64_t)val, opcode.opcode) : new AscmNodeData<int64_t>((int64_t)val, opcode.opcode);
        default:
            throw std::runtime_error(std::format("Can't create node with sizeof {}", opcode.sizeOf));
        };
    }

    AscmNodeOpCode* CompileObject::BuildAscmNodeData(int64_t val) {
        for (const NumberOpCodesS& opcode : NumberOpCodes) {
            if (opcode.minValue <= val && val <= opcode.maxValue && HasOpCode(opcode.opcode)) {
                return BuildAscmNodeData(opcode, val);
            }
        }
        // fallback result
        return new AscmNodeData<int64_t>(val, OPCODE_GetLongInteger);
    }

    AscmNodeOpCode* CompileObject::CreateBiggestValue(int64_t val) {
        for (size_t i = ARRAYSIZE(NumberOpCodes); i > 0; i--) {
            const NumberOpCodesS& opcode{ NumberOpCodes[i - 1] };
            if (HasOpCode(opcode.opcode)) {
                return BuildAscmNodeData(opcode, val);
            }
        }

        return new AscmNodeData<int64_t>(val, OPCODE_GetLongInteger);
    }

    bool CompileObject::TryStringNodeValue(ParseTree* hashNode, std::string& output) {
        if (!hashNode) {
            return false; // wtf?
        }
        while (hashNode->getTreeType() != TREE_TERMINAL) {
            if (hashNode->getTreeType() == TREE_ERROR) {
                info.PrintLineMessage(core::logs::LVL_ERROR, hashNode, "Tree error");
                return false;
            }
            if (hashNode->children.size() != 1) {
                info.PrintLineMessage(core::logs::LVL_ERROR, hashNode, "Not a hash expression");
                return false;
            }

            hashNode = hashNode->children[0];
        }

        TerminalNode* term = dynamic_cast<TerminalNode*>(hashNode);
        if (term->getSymbol()->getType() == gscParser::STRING) {
            output = ParseString(term);
            return true;
        }
        else {
            info.PrintLineMessage(core::logs::LVL_ERROR, hashNode, "Not a string expression, only string node available");
            return false;
        }
    }

    bool CompileObject::TryHashNodeValue(ParseTree* hashNode, uint64_t& output) {
        if (!hashNode) {
            return false; // wtf?
        }
        while (hashNode->getTreeType() != TREE_TERMINAL) {
            if (hashNode->getTreeType() == TREE_ERROR) {
                info.PrintLineMessage(core::logs::LVL_ERROR, hashNode, "Tree error");
                return false;
            }
            if (hashNode->children.size() != 1) {
                info.PrintLineMessage(core::logs::LVL_ERROR, hashNode, "Not a hash expression");
                return false;
            }

            hashNode = hashNode->children[0];
        }

        TerminalNode* term = dynamic_cast<TerminalNode*>(hashNode);
        switch (term->getSymbol()->getType()) {
        case gscParser::HASHSTRING: {
            std::string hash = term->getText();
            char type = hash[0];
            std::string sub = hash.substr(2, hash.length() - 3);
            auto ith = vmInfo->hashesFunc.find(type);

            if (ith == vmInfo->hashesFunc.end()) {
                info.PrintLineMessage(core::logs::LVL_ERROR, hashNode, std::format("Hash type not available for this vm: {}", type));
                return false;
            }

            const char* ss = sub.c_str();

            if (!hash::TryHashPattern(ss, output)) {
                output = ith->second.hashFunc(ss);
                if (!output) {
                    info.PrintLineMessage(core::logs::LVL_ERROR, hashNode, std::format("Can't hash the string '{}' with the type {}", sub, type));
                    return false;
                }
                AddHash(sub);
            }
            return true;
        }
        case gscParser::STRING: {
            std::string node = term->getText();
            constexpr char type = '#'; // default type

            std::string key{ ParseString(term) };
            auto ith = vmInfo->hashesFunc.find(type);

            if (ith == vmInfo->hashesFunc.end()) {
                info.PrintLineMessage(core::logs::LVL_ERROR, hashNode, std::format("Hash type not available for this vm: {}", type));
                return false;
            }

            const char* ss = key.c_str();

            if (!hash::TryHashPattern(ss, output)) {
                output = ith->second.hashFunc(ss);
                if (!output) {
                    info.PrintLineMessage(core::logs::LVL_ERROR, hashNode, std::format("Can't hash the string '{}' with the type {}", node, type));
                    return false;
                }
                AddHash(key);
            }
            return true;
        }
        default:
            info.PrintLineMessage(core::logs::LVL_ERROR, hashNode, "Not a hash expression, only string or hash node available");
            return false;
        }
    }

    std::string CompileObject::GetIncludeName(const std::string& origin) {
        std::string data{ origin };
        if (!(data.ends_with(".gsc") || data.ends_with(".csc")) && !(data.starts_with("script_"))) {
            switch (type) {
            case FILE_CSC:
                data += ".csc";
                break;
            case FILE_GSC:
                data += ".gsc";
                break;
            }
        }
        return data;
    }

    bool CompileObject::Compile(std::vector<byte>& data, std::vector<byte>* pdbgdata) {
        if (gscHandler->HasFlag(tool::gsc::GOHF_NOTIFY_CRC)) {
            // add the notify crc function for T9 38 vm
            constexpr const char* name = "$notif_checkum";
            uint64_t nameHashed = vmInfo->HashField(name);
            AddHash(name);
            auto [res, err] = exports.try_emplace(nameHashed, *this, nameHashed, currentNamespace, fileNameSpace, vmInfo);

            if (!err) {
                LOG_ERROR("Can't register notif checksum export: {}", name);
                return false;
            }

            FunctionObject& f = res->second;
            f.autoexecOrder = INT64_MIN; // first
            f.m_flags = tool::gsc::CLASS_VTABLE;
            f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_CheckClearParams));
            f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_PreScriptCall));
            if (config.checksum < 0) {
                f.m_nodes.push_back(crcData.opcode = new AscmNodeData<uint32_t>((uint32_t)(-config.checksum), OPCODE_GetNegUnsignedInteger));
            }
            else {
                f.m_nodes.push_back(crcData.opcode = new AscmNodeData<uint32_t>((uint32_t)(config.checksum), OPCODE_GetUnsignedInteger));
            }

            auto gvarIt = vmInfo->globalvars.find(vmInfo->HashField("level"));

            if (gvarIt == vmInfo->globalvars.end()) {
                LOG_ERROR("Can't find level def for checksum export: {}", name);
                return false;
            }

            GlobalVariableDef& gv = gvarIt->second;

            GlobalVarObject& decl = globals[gv.name];

            if (!decl.def) {
                decl.def = &gv;
            }

            AscmNodeGlobalVariable* gvar = new AscmNodeGlobalVariable(decl.def, OPCODE_GetGlobal);
            decl.nodes.emplace_back(gvar);
            f.m_nodes.push_back(gvar);
            f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Notify));
            f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_End));
        }
        else if (gscHandler->HasFlag(tool::gsc::GOHF_NOTIFY_CRC_STRING)) {
            // add the notify crc function for JUP B vm

            constexpr const char* name = "$notif_checkum";
            uint64_t nameHashed = vmInfo->HashField(name);
            AddHash(name);
            auto [res, err] = exports.try_emplace(nameHashed, *this, nameHashed, currentNamespace, fileNameSpace, vmInfo);

            if (!err) {
                LOG_ERROR("Can't register notif checksum export: {}", name);
                return false;
            }

            FunctionObject& f = res->second;
            f.autoexecOrder = INT64_MIN; // first
            f.m_flags = tool::gsc::CLASS_VTABLE;
            f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_CheckClearParams));
            f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_PreScriptCall));

            const char* crcStr = utils::va("%lld", config.checksum);

            StringObject& strdef = RegisterString(crcStr);
            AscmNodeData<uint32_t>* getstr = new AscmNodeData<uint32_t>(0xFFFFFFFF, OPCODE_GetString);
            strdef.nodes.push_back(getstr);
            // add some padding so we can patch the crc at runtime (maybe one day it'll be 64 bits?)
            strdef.forceLen = CRC_LEN_PADDING;
            strdef.listeners.push_back(&crcData.strlistener);
            f.m_nodes.push_back(getstr);
            f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_IW_GetLevel));
            f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_IW_Notify));
            f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_End));
        }

        // set builtin call types for jup VM
        if (!gscHandler->HasFlag(tool::gsc::GOHF_SUPPORT_GET_API_SCRIPT)) {
            // we need to compute if an import is from a builtin or a script
            for (auto& [idx, imps] : imports) {

                uint64_t name = idx.name;
                uint64_t nsp = idx.name_space;

                auto it = exports.find(name);

                bool scriptCall{ it != exports.end() && it->second.m_name_space == nsp };

                for (ImportObject& imp : imps) {
                    if (imp.flags & tool::gsc::GET_CALL) {
                        int funcType{ imp.flags & 0xF };
                        if (funcType == tool::gsc::FUNC_METHOD) {
                            // ref
                            if (!scriptCall) {
                                LOG_ERROR("Getter for builtin methods not implemented in this vm yet");
                                return false;
                            }
                        }

                        if (!scriptCall) {
                            if (funcType == tool::gsc::FUNCTION_CHILDTHREAD
                                || funcType == tool::gsc::FUNCTION_THREAD
                                || funcType == tool::gsc::METHOD_CHILDTHREAD
                                || funcType == tool::gsc::METHOD_THREAD) {
                                LOG_ERROR("Usage of thread modifier on a builtin call {}", hashutils::ExtractTmp("function", idx.name));
                                return false;
                            }
                            if (funcType == tool::gsc::METHOD) {
                                imp.flags = (imp.flags & ~0xF) | tool::gsc::ACTS_CALL_BUILTIN_METHOD;
                            }
                            else if (funcType == tool::gsc::FUNCTION) {
                                imp.flags = (imp.flags & ~0xF) | tool::gsc::ACTS_CALL_BUILTIN_FUNCTION;
                            }
                            else {
                                LOG_ERROR("Unknown builtin call {} {}", funcType, hashutils::ExtractTmp("function", idx.name));
                                return false;
                            }
                        }

                        for (AscmNodeFunctionCall* node : imp.nodes) {
                            node->SetScriptCall(*this, scriptCall);
                        }
                    }
                    else {
                        for (AscmNodeFunctionCall* node : imp.nodes) {
                            node->SetScriptCall(*this, true);
                        }
                    }
                }
            }

        }

        size_t headerLoc{ utils::Allocate(data, gscHandler->GetHeaderSize()) };

        if (includes.size()) LOG_TRACE("Compile {} include(s)...", includes.size());
        size_t incTable{};
        if (includes.size()) {
            if (gscHandler->HasFlag(tool::gsc::GOHF_STRING_NAMES)) {
                // store the offsets
                incTable = utils::Allocate(data, sizeof(uint32_t) * includes.size());

                size_t idx{};

                for (const std::string& i : includes) {
                    reinterpret_cast<uint32_t*>(&data[incTable])[idx++] = (uint32_t)data.size();
                    utils::WriteString(data, i.data());
                }
            }
            else {
                // store the hashes
                incTable = utils::Allocate(data, sizeof(uint64_t) * includes.size());
                uint64_t* tab = reinterpret_cast<uint64_t*>(&data[incTable]);

                for (const std::string& i : includes) {
                    std::string data{ GetIncludeName(i) };
                    AddHash(data);

                    *tab = vmInfo->HashPath(data.data());
                    tab++;
                }
            }
        }

        size_t expTable = utils::Allocate(data, gscHandler->GetExportSize() * exports.size());

        size_t csegOffset = data.size();

        if (exports.size()) LOG_TRACE("Compile {} export(s)...", exports.size());

        size_t exportIndex{};

        std::vector<FunctionObject*> detourObjs{};

        CompileObject& that = *this;
        auto writeExport = [&data, &that, &expTable, &exportIndex, &detourObjs](FunctionObject& exp) -> bool {
            exp.OptimizeNodes();
            if (exp.m_nodes.empty()) {
                LOG_ERROR("No nodes for {:x}", exp.m_name);
                return false;
            }

            struct PreExp { uint64_t top; uint64_t bottom; };

            utils::Aligned<PreExp>(data);
            utils::Allocate(data, sizeof(uint64_t));

            int32_t len = exp.ComputeRelativeLocations((int32_t)data.size());
            if (len < 0) {
                LOG_ERROR("Error when allocating relative locations");
                return false;
            }

            exp.location = data.size();
            tool::gsc::IW23GSCExport e{};
            e.name = exp.m_name;
            e.name_space = exp.m_name_space;
            e.file_name_space = exp.m_data_name;
            e.flags = that.gscHandler->MapFlagsExportToInt(exp.m_flags);
            e.address = (int32_t)exp.location;
            e.param_count = exp.m_params;

            AscmCompilerContext cctx{ that.vmInfo, that.config.platform, exp.m_allocatedVar, data, that.config };

            for (AscmNode* node : exp.m_nodes) {
                if (!node->Write(cctx)) {
                    return false;
                }
            }
            // add size for detours
            exp.size = data.size() - exp.location;

            // compute function crc
            utils::crc::CRC32 crc{};
            // function crc
            crc.Update(&data[exp.location], exp.size);
            e.checksum = crc;

            that.gscHandler->WriteExport(&data[expTable + that.gscHandler->GetExportSize() * exportIndex++], e);

            if (exp.IsDetour()) {
                detourObjs.push_back(&exp);
            }

            return true;
            };

        std::vector<FunctionObject*> autoexecs{};
        std::vector<FunctionObject*> othersfuncs{};

        for (auto& [name, exp] : exports) {
            if (exp.m_flags & tool::gsc::T8GSCExportFlags::AUTOEXEC) {
                autoexecs.push_back(&exp);
            }
            else {
                othersfuncs.push_back(&exp);
            }
        }

        // sort the autoexecs by ids and write them first

        std::sort(autoexecs.begin(), autoexecs.end(), [](auto& f1, auto& f2) -> bool { return f1->autoexecOrder < f2->autoexecOrder; });

        if (config.obfuscate) {
            std::shuffle(othersfuncs.begin(), othersfuncs.end(), std::mt19937{ std::random_device{}() });
        }

        bool exportsOk{ true };
        for (FunctionObject* exp : autoexecs) {
            if (!writeExport(*exp)) {
                exportsOk = false;
            }
        }

        for (auto* expptr : othersfuncs) {
            auto& exp = *expptr;

            if (!writeExport(exp)) {
                exportsOk = false;
            }
        }

        if (!exportsOk) {
            return false;
        }
        if (!detourObjs.empty() && !config.detourType) {
            LOG_ERROR("Detour parsed, but no --detour has been specified, they will be ignored.");
        }

        size_t csegSize = data.size() - csegOffset;

        if (strings.size()) LOG_TRACE("Compile {} strings(s)...", strings.size());

        // compile strings

        for (auto& [key, strobj] : strings) {
            if (strobj.devString) continue; // ignore dev strings
            strobj.location = (uint32_t)data.size();
            for (uint32_t* lis : strobj.listeners) {
                *lis = strobj.location;
            }

            auto [strHeader, strHeaderSize] = gscHandler->GetStringHeader(key.length());

            utils::WriteValue(data, (void*)strHeader, strHeaderSize);

            utils::WriteString(data, key.c_str());
            // add bytes to the string
            if (key.length() < strobj.forceLen) {
                for (size_t i = key.length(); i < strobj.forceLen; i++) {
                    data.push_back(0);
                }
            }
        }

        size_t stringRefs = data.size();
        size_t stringCount{};

        for (auto& [key, strobj] : strings) {
            if (strobj.devString) continue; // ignore dev strings
            size_t w{};
            while (w < strobj.nodes.size()) {
                if (w % 0xFF == 0) {
                    size_t buff = utils::Allocate(data, gscHandler->GetStringSize());
                    tool::gsc::T8GSCString str{};
                    str.string = strobj.location;
                    str.type = 0;
                    str.num_address = (byte)((strobj.nodes.size() - w) > 0xFF ? 0xFF : (strobj.nodes.size() - w));
                    gscHandler->WriteString(&data[buff], str);
                    stringCount++;
                }
                utils::WriteValue<uint32_t>(data, strobj.nodes[w++]->GetDataFLoc(vmInfo->HasFlag(VmFlags::VMF_ALIGN)));
            }
        }

        if (animtrees.size()) LOG_TRACE("Compile {} anim tree(s)...", animtrees.size());

        for (auto& [key, atree] : animtrees) {
            atree.location = (uint32_t)data.size();

            auto [strHeader, strHeaderSize] = gscHandler->GetStringHeader(key.length());

            utils::WriteValue(data, (void*)strHeader, strHeaderSize);
            utils::WriteString(data, key.c_str());

            for (auto& [anim, ref] : atree.anims) {
                ref.location = (uint32_t)data.size();
                utils::WriteValue(data, (void*)strHeader, strHeaderSize);
                utils::WriteString(data, anim.c_str());
            }
        }

        size_t animRefs{};
        size_t animCount{};
        size_t animDoubleRefs{};
        size_t animDoubleCount{};

        if (vmInfo->HasFlag(VmFlags::VMF_ANIMTREE_T7)) {
            animDoubleRefs = data.size();
            animDoubleCount = animtrees.size();

            for (auto& [key, atree] : animtrees) {
                if (atree.animtree.size() > 0x7FFF) {
                    // if you reach that, you're the issue, not the code
                    LOG_ERROR("Too many animtree nodes {} > {}", atree.animtree.size(), 0x7FFF);
                    return false;
                }
                size_t treeLoc{ utils::Allocate(data, sizeof(tool::gsc::T7GscAnimTree)) };
                uint32_t* atNodes{ utils::AllocateArray<uint32_t>(data, atree.animtree.size()) };
                for (AscmNodeOpCode* op : atree.animtree) {
                    *(atNodes++) = op->GetDataFLoc(vmInfo->HasFlag(VmFlags::VMF_ALIGN));
                }

                size_t numNodes{};
                for (auto& [nkey, anode] : atree.anims) {
                    for (AscmNodeOpCode* op : anode.anim) {
                        uint64_t* atNodes{ utils::AllocateArray<uint64_t>(data, 2) };
                        atNodes[0] = anode.location;
                        atNodes[1] = op->GetDataFLoc(vmInfo->HasFlag(VmFlags::VMF_ALIGN));
                        numNodes++;
                    }
                }

                if (numNodes > 0x7FFF) {
                    // if you reach that, you're the issue, not the code
                    LOG_ERROR("Too many anim nodes {} > {}", atree.anims.size(), 0x7FFF);
                    return false;
                }

                tool::gsc::T7GscAnimTree& tree{ *(tool::gsc::T7GscAnimTree*)&data[treeLoc] };
                tree.name = (uint32_t)atree.location;
                tree.num_tree_address = (uint16_t)atree.animtree.size();
                tree.num_node_address = (uint16_t)numNodes;
            }
        }
        else {
            animRefs = data.size();

            // write animtrees
            for (auto& [key, atree] : animtrees) {
                size_t w{};
                while (w < atree.animtree.size()) {
                    if (w % 0xFF == 0) {
                        size_t buff{ utils::Allocate(data, gscHandler->GetAnimTreeSingleSize()) };
                        tool::gsc::GSC_USEANIMTREE_ITEM anim{};
                        anim.address = atree.location;
                        anim.num_address = (byte)((atree.animtree.size() - w) > 0xFF ? 0xFF : (atree.animtree.size() - w));
                        gscHandler->WriteAnimTreeSingle(&data[buff], anim);
                        animCount++;
                    }
                    utils::WriteValue<uint32_t>(data, atree.animtree[w++]->GetDataFLoc(vmInfo->HasFlag(VmFlags::VMF_ALIGN)));
                }
            }

            // write anims
            animDoubleRefs = data.size();
            for (auto& [key, atree] : animtrees) {
                for (auto& [anim, ref] : atree.anims) {
                    size_t w{};
                    while (w < ref.anim.size()) {
                        if (w % 0xFF == 0) {
                            size_t buff{ utils::Allocate(data, gscHandler->GetAnimTreeDoubleSize()) };
                            tool::gsc::GSC_ANIMTREE_ITEM anim{};
                            anim.address_str1 = atree.location;
                            anim.address_str2 = ref.location;
                            anim.num_address = (byte)((ref.anim.size() - w) > 0xFF ? 0xFF : (ref.anim.size() - w));
                            gscHandler->WriteAnimTreeDouble(&data[buff], anim);
                            animDoubleCount++;
                        }
                        utils::WriteValue<uint32_t>(data, ref.anim[w++]->GetDataFLoc(vmInfo->HasFlag(VmFlags::VMF_ALIGN)));
                    }
                }
            }

        }


        size_t gvarRefs = data.size();
        size_t gvarCount{};
        if (gscHandler->HasFlag(tool::gsc::GOHF_GLOBAL) && globals.size()) {
            LOG_TRACE("Compile {} global(s)...", globals.size());

            for (auto& [key, gvobj] : globals) {
                size_t w{};
                while (w < gvobj.nodes.size()) {
                    if (w % 0xFF == 0) {
                        size_t buff = utils::Allocate(data, gscHandler->GetGVarSize());
                        tool::gsc::T8GSCGlobalVar gv{};
                        AddHash(key);
                        gv.name = (uint32_t)vmInfo->HashField(key.c_str());
                        gv.num_address = (byte)((gvobj.nodes.size() - w) > 0xFF ? 0xFF : (gvobj.nodes.size() - w));
                        gscHandler->WriteGVar(&data[buff], gv);
                        gvarCount++;
                    }
                    utils::WriteValue<uint32_t>(data, gvobj.nodes[w++]->GetDataFLoc(vmInfo->HasFlag(VmFlags::VMF_ALIGN)));
                }
            }
        }

        if (imports.size()) LOG_TRACE("Compile {} import(s)...", imports.size());
        size_t implRefs = data.size();
        size_t implCount{};

        for (auto& [key, imobj] : imports) {
            uint64_t nsp = key.name_space;
            uint64_t func = key.name;
            for (ImportObject& implData : imobj) {
                size_t w{};
                while (w < implData.nodes.size()) {
                    if (w % 0xFFFF == 0) {
                        size_t buff = utils::Allocate(data, gscHandler->GetImportSize());
                        tool::gsc::IW23GSCImport gv{};
                        gv.name_space = nsp;
                        gv.name = func;
                        gv.param_count = implData.params;
                        gv.flags = gscHandler->MapFlagsImportToInt(implData.flags);
                        gv.num_address = (uint16_t)((implData.nodes.size() - w) > 0xFFFF ? 0xFFFF : (implData.nodes.size() - w));
                        gscHandler->WriteImport(&data[buff], gv);
                        implCount++;
                    }
                    utils::WriteValue<uint32_t>(data, implData.nodes[w++]->GetFLoc());
                }
            }
        }


        int32_t checksum = config.checksum;
        if (!checksum) {
            utils::crc::CRC32 fileCRC{};
            fileCRC.Update(data.data(), data.size());
            checksum = fileCRC;

            if (gscHandler->HasFlag(tool::gsc::GOHF_NOTIFY_CRC_STRING)) {
                // write the end crc string into the padding
                std::snprintf((char*)&data[crcData.strlistener], CRC_LEN_PADDING, "%d", checksum);
            }
            else if (gscHandler->HasFlag(tool::gsc::GOHF_NOTIFY_CRC)) {
                // todo: replace checksum for cw
                //crcData.opcode
                if (!crcData.opcode) throw std::runtime_error("Missing CRC opcode floc");
                byte* crcOp{ &data[crcData.opcode->floc] };

                OPCode opcode;

                if (checksum < 0) {
                    opcode = OPCODE_GetNegUnsignedInteger;
                }
                else {
                    opcode = OPCODE_GetUnsignedInteger;
                }

                auto [ok, op] = GetOpCodeId(vmInfo->vmMagic, config.platform, opcode, config.useModToolOpCodes);

                if (!ok) {
                    LOG_ERROR("Can't find crc opcode {} ({}) for vm {}/{}", utils::PtrOrElse(OpCodeName(opcode), "null"), (int)opcode, vmInfo->name, PlatformName(config.platform));
                    return false;
                }

                if (vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16)) {
                    if (vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
                        crcOp = utils::Aligned<uint16_t>(crcOp);
                    }
                    *(uint16_t*)crcOp = op;
                    crcOp += 2;
                }
                else {
                    *crcOp = (byte)op;
                    crcOp += 1;
                }
                if (vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
                    crcOp = utils::Aligned<uint32_t>(crcOp);
                }

                LOG_INFO("crc: {} 0x{:x}", checksum, checksum);
                if (checksum < 0) {
                    *(uint32_t*)crcOp = (uint32_t)(-checksum);
                }
                else {
                    *(uint32_t*)crcOp = (uint32_t)checksum;
                }
            }
        }


        if (config.computeDevOption || (config.detourType == DETOUR_ACTS && !detourObjs.empty())) {
            if (!pdbgdata) {
                LOG_WARNING("Trying to compile debug file, but no buffer was defined");
            }
            else {
                std::vector<byte>& dbgdata{ *pdbgdata };
                // all header
                utils::Allocate<shared::gsc::acts_debug::GSC_ACTS_DEBUG>(dbgdata);
                size_t hashesLoc{};
                size_t hashesIdx{};
                size_t devBlocksLoc{};
                size_t devBlocksIdx{};
                size_t lazyLinksLoc{};
                size_t lazyLinksIdx{};
                size_t linesLoc{};
                size_t linesIdx{};
                size_t filesLoc{};
                size_t filesIdx{};
                if (config.computeDevOption) {

                    if (!config.obfuscate) {
                        LOG_TRACE("Compile {} hash(es)...", hashes.size());

                        hashesLoc = utils::Allocate(dbgdata, sizeof(uint32_t) * hashes.size());
                        for (const std::string& h : hashes) {
                            reinterpret_cast<uint32_t*>(&dbgdata[hashesLoc])[hashesIdx++] = (uint32_t)dbgdata.size();
                            utils::WriteString(dbgdata, h.c_str());
                        }
                    }

                    LOG_TRACE("Compile {} dev block(s)...", m_devBlocks.size());
                    devBlocksLoc = utils::Allocate(dbgdata, sizeof(uint32_t) * m_devBlocks.size());
                    for (const AscmNode* node : m_devBlocks) {
                        reinterpret_cast<uint32_t*>(&dbgdata[devBlocksLoc])[devBlocksIdx++] = node->floc;
                    }

                    LOG_TRACE("Compile {} lazy link(s)...", lazyimports.size());
                    for (auto& [loc, lz] : lazyimports) {
                        size_t lzi = utils::Allocate(dbgdata, sizeof(shared::gsc::acts_debug::GSC_ACTS_LAZYLINK) + sizeof(uint32_t) * lz.size());
                        lazyLinksIdx++;

                        if (!lazyLinksLoc) {
                            lazyLinksLoc = lzi;
                        }


                        shared::gsc::acts_debug::GSC_ACTS_LAZYLINK* lzd = reinterpret_cast<shared::gsc::acts_debug::GSC_ACTS_LAZYLINK*>(&dbgdata[lzi]);

                        lzd->name = loc.name;
                        lzd->name_space = loc.name_space;
                        lzd->script = loc.script;
                        lzd->num_address = (uint32_t)lz.size();

                        uint32_t* locs = reinterpret_cast<uint32_t*>(&lzd[1]);
                        for (AscmNodeLazyLink* node : lz) {
                            *(locs++) = node->floc;
                        }
                    }

                    if (!config.obfuscate) {
                        LOG_TRACE("Compile dev lines...");
                        // GSC_ACTS_LINES
                        std::vector<shared::gsc::acts_debug::GSC_ACTS_LINES> lines{};

                        shared::gsc::acts_debug::GSC_ACTS_LINES curr{};
                        for (auto& [name, exp] : exports) {
                            for (AscmNode* node : exp.m_nodes) {
                                curr.end = node->floc;
                                if (!node->line) {
                                    // bad line, we flush a previous info and clear the current
                                    if (curr.lineNum) {
                                        lines.emplace_back(curr);
                                    }
                                    curr.lineNum = 0;

                                    continue;
                                }

                                if (curr.lineNum == node->line) {
                                    continue; // same line, we can continue
                                }

                                if (curr.lineNum) {
                                    lines.emplace_back(curr);
                                }
                                curr.lineNum = node->line;
                                curr.start = node->floc;
                            }
                            // to keep track of the last op end
                            curr.end = (uint32_t)(exp.location + exp.size);
                        }
                        if (curr.lineNum) {
                            lines.emplace_back(curr);
                        }

                        linesIdx = lines.size();
                        linesLoc = dbgdata.size();
                        dbgdata.insert(dbgdata.end(), (byte*)lines.data(), (byte*)(lines.data() + linesIdx));


                        LOG_TRACE("Compile filenames...");
                        filesLoc = dbgdata.size();
                        utils::Allocate(dbgdata, sizeof(shared::gsc::acts_debug::GSC_ACTS_FILES) * info.container.blocks.size());
                        for (core::preprocessor::StringData& block : info.container.blocks) {
                            size_t strLoc = dbgdata.size();
                            std::string filename;
                            if (!config.baseDir.empty()) {
                                std::filesystem::path rfile{ std::filesystem::relative(block.filename, config.baseDir) };
                                filename = rfile.string();
                            }
                            else {
                                filename = block.filename.string();
                            }
                            //baseDir
                            utils::WriteString(dbgdata, filename.c_str());
                            shared::gsc::acts_debug::GSC_ACTS_FILES& f = reinterpret_cast<shared::gsc::acts_debug::GSC_ACTS_FILES*>(dbgdata.data() + filesLoc)[filesIdx++];
                            f.filename = (uint32_t)strLoc;
                            f.lineStart = block.startLine;
                            f.lineEnd = block.startLine + block.sizeLine;
                        }
                    }

                    for (auto& [key, strobj] : strings) {
                        if (!strobj.devString) continue; // ignore dev strings
                        strobj.location = (uint32_t)dbgdata.size();
                        for (uint32_t* lis : strobj.listeners) {
                            *lis = strobj.location;
                        }

                        auto [strHeader, strHeaderSize] = gscHandler->GetStringHeader(key.length());

                        utils::WriteValue(dbgdata, (void*)strHeader, strHeaderSize);

                        utils::WriteString(dbgdata, key.c_str());
                        // add bytes to the string
                        if (key.length() < strobj.forceLen) {
                            for (size_t i = key.length(); i < strobj.forceLen; i++) {
                                dbgdata.push_back(0);
                            }
                        }
                    }
                }

                uint32_t detoursLoc{};
                uint32_t detoursCount{};
                if (!detourObjs.empty() && config.detourType == DETOUR_ACTS) {
                    LOG_TRACE("Compile {} detour(s)...", detourObjs.size());
                    detoursCount = (uint32_t)detourObjs.size();
                    detoursLoc = (uint32_t)utils::Allocate(dbgdata, sizeof(shared::gsc::acts_debug::GSC_ACTS_DETOUR) * detourObjs.size());
                    shared::gsc::acts_debug::GSC_ACTS_DETOUR* detours = reinterpret_cast<shared::gsc::acts_debug::GSC_ACTS_DETOUR*>(&dbgdata[detoursLoc]);

                    for (FunctionObject* objexp : detourObjs) {
                        detours->location = (uint32_t)objexp->location;
                        detours->size = (uint32_t)objexp->size;

                        detours->name = objexp->detour.func;
                        detours->name_space = objexp->detour.nsp;
                        detours->script = objexp->detour.script;
                        detours++;
                    }
                }

                shared::gsc::acts_debug::GSC_ACTS_DEBUG* debug_obj = reinterpret_cast<shared::gsc::acts_debug::GSC_ACTS_DEBUG*>(dbgdata.data());

                *reinterpret_cast<uint64_t*>(debug_obj->magic) = shared::gsc::acts_debug::MAGIC;
                debug_obj->version = shared::gsc::acts_debug::CURRENT_VERSION;
                debug_obj->flags = 0;
                if (config.obfuscate) debug_obj->flags |= shared::gsc::acts_debug::ActsDebugFlags::ADFG_OBFUSCATED;
                if (config.computeDevOption) debug_obj->flags |= shared::gsc::acts_debug::ActsDebugFlags::ADFG_DEBUG;
                if (type == FILE_CSC) debug_obj->flags |= shared::gsc::acts_debug::ActsDebugFlags::ADFG_CLIENT;
                if (((config.platform << shared::gsc::acts_debug::ActsDebugFlags::ADFG_PLATFORM_SHIFT) & ~shared::gsc::acts_debug::ActsDebugFlags::ADFG_PLATFORM_MASK)) {
                    LOG_WARNING("Can't encode platform ID in debug header: Too big {}", (int)config.platform);
                }
                else {
                    debug_obj->flags |= config.platform << shared::gsc::acts_debug::ActsDebugFlags::ADFG_PLATFORM_SHIFT;
                }

                debug_obj->actsVersion = (uint64_t)core::actsinfo::VERSION_ID;
                debug_obj->strings_count = (uint32_t)hashesIdx;
                debug_obj->strings_offset = (uint32_t)hashesLoc;
                debug_obj->detour_count = (uint32_t)detoursCount;
                debug_obj->detour_offset = (uint32_t)detoursLoc;
                debug_obj->devblock_offset = (uint32_t)devBlocksLoc;
                debug_obj->devblock_count = (uint32_t)devBlocksIdx;
                debug_obj->lazylink_offset = (uint32_t)lazyLinksLoc;
                debug_obj->lazylink_count = (uint32_t)lazyLinksIdx;
                debug_obj->devstrings_offset = 0;
                debug_obj->lines_count = (uint32_t)linesIdx;
                debug_obj->lines_offset = (uint32_t)linesLoc;
                debug_obj->files_count = (uint32_t)filesIdx;
                debug_obj->files_offset = (uint32_t)filesLoc;

                // add crc location
                if (gscHandler->HasFlag(tool::gsc::GOHF_NOTIFY_CRC)) {
                    assert(crcData.opcode);
                    debug_obj->crc_offset = (uint32_t)crcData.opcode->floc;
                }
                else if (gscHandler->HasFlag(tool::gsc::GOHF_NOTIFY_CRC_STRING)) {
                    debug_obj->crc_offset = (uint32_t)crcData.strlistener;
                }
                else {
                    debug_obj->crc_offset = 0;
                }

                debug_obj->checksum = checksum;
            }
        }

        uint32_t nameOffSet{};
        if (gscHandler->HasFlag(tool::gsc::GOHF_STRING_NAMES)) {
            nameOffSet = (uint32_t)data.size();
            utils::WriteString(data, fileNameStr ? fileNameStr : "");
        }


        size_t devStringRefs = data.size();
        size_t devStringCount{};
        for (auto& [key, strobj] : strings) {
            if (!strobj.devString) continue; // ignore normal strings
            size_t w{};
            while (w < strobj.nodes.size()) {
                if (w % 0xFF == 0) {
                    size_t buff = utils::Allocate(data, gscHandler->GetStringSize());
                    tool::gsc::T8GSCString str{};
                    str.string = strobj.location;
                    str.type = 0;
                    str.num_address = (byte)((strobj.nodes.size() - w) > 0xFF ? 0xFF : (strobj.nodes.size() - w));
                    gscHandler->WriteString(&data[buff], str);
                    devStringCount++;
                }
                utils::WriteValue<uint32_t>(data, strobj.nodes[w++]->GetDataFLoc(vmInfo->HasFlag(VmFlags::VMF_ALIGN)));
            }
        }


        // compile header
        gscHandler->SetFile((byte*)data.data() + headerLoc, 0);

        if (nameOffSet) {
            gscHandler->SetNameString(nameOffSet);
        }
        else {
            gscHandler->SetName(fileName);
        }
        gscHandler->SetChecksum(checksum);
        gscHandler->SetHeader();

        gscHandler->SetIncludesCount((int16_t)includes.size());
        gscHandler->SetIncludesOffset((int32_t)incTable);

        gscHandler->SetStringsCount((int16_t)stringCount);
        gscHandler->SetStringsOffset((int32_t)stringRefs);

        gscHandler->SetDevStringsCount((int16_t)devStringCount);
        gscHandler->SetDevStringsOffset((int32_t)devStringRefs);

        if (gscHandler->HasFlag(tool::gsc::GOHF_GLOBAL)) {
            gscHandler->SetGVarsCount((int16_t)gvarCount);
            gscHandler->SetGVarsOffset((int32_t)gvarRefs);
        }

        gscHandler->SetExportsCount((int16_t)exports.size());
        gscHandler->SetExportsOffset((int32_t)expTable);
        gscHandler->SetCSEGOffset((int32_t)csegOffset);
        gscHandler->SetCSEGSize((int32_t)csegSize);

        gscHandler->SetImportsCount((int16_t)implCount);
        gscHandler->SetImportsOffset((int32_t)implRefs);

        gscHandler->SetAnimTreeDoubleCount((int16_t)animDoubleCount);
        gscHandler->SetAnimTreeDoubleOffset((int32_t)animDoubleRefs);
        gscHandler->SetAnimTreeSingleCount((int16_t)animCount);
        gscHandler->SetAnimTreeSingleOffset((int32_t)animRefs);


        gscHandler->SetFileSize((int32_t)data.size());


        return true;
    }

    void CompileObject::AddImport(AscmNodeFunctionCall* funcCall, uint64_t funcNspHash, uint64_t funcHash, size_t paramCount, uint8_t importFlags) {
        // link by the game, but we write it for test
        Located located{ funcNspHash, funcHash };

        if (devBlockDepth) {
            // dev call
            importFlags |= tool::gsc::T8GSCImportFlags::DEV_CALL;
        }

        std::vector<ImportObject>& impList = imports[located];

        bool useParams = funcCall && (funcCall->flags & FCF_GETTER) == 0;

        auto it = std::find_if(impList.begin(), impList.end(), [importFlags, paramCount, useParams](const ImportObject& e) {
            if (useParams && paramCount != e.params) {
                return false;
            }
            return e.flags == importFlags;
            });

        if (it == impList.end()) {
            // no equivalent, we need to create our own node
            impList.emplace_back(importFlags, (byte)paramCount).nodes.push_back(funcCall);
        }
        else {
            // same local/flags, we can add our node
            it->nodes.push_back(funcCall);
        }
    }

    void CompileObject::AddLazy(AscmNodeLazyLink* lazyLink) {
        Located located{ lazyLink->nsp, lazyLink->func, lazyLink->path };

        lazyimports[located].push_back(lazyLink);
    }
}