#include <includes.hpp>
#include <decryptutils.hpp>
#include <core/bytebuffer.hpp>
#include <compatibility/xensik_gscbin.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>
#include <tools/gsc_iw.hpp>


// GSCBIN

namespace {
    using namespace tool::gsc;
    using namespace tool::gsc::iw;
    using namespace tool::gsc::opcode;

    struct FakeLinkHeader {
        // what is the easiest way to convert a stream based script loading to a linked script loading?
        // idk, but today it'll be to read the stream based script into a fake linked script and then
        // reading it. (It's dumb, yeah)
        uint64_t name{};
        size_t exports_table{};
        size_t exports_count{};
        size_t imports_table{};
        size_t imports_count{};
        size_t animtree_table{};
        size_t animtree_count{};
        size_t useanimtree_table{};
        size_t useanimtree_count{};
        size_t strings_table{};
        size_t strings_count{};
        size_t tokens_table{};
        size_t tokens_count{};

        compatibility::xensik::gscbin::GscBinHeader binHeader{};
    };

    struct PreString {
        uint32_t string;
        uint32_t address;
    };
    struct PreImport {
        uint32_t id;
        byte flags;
        uint32_t address;
    };

    class IWGSCOBJHandler : public GSCOBJHandler {
    public:

        FakeLinkHeader fakeHeader{};
        std::vector<byte> fakeLinked{};
        IWGSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_IW_BIN) {}

        void DumpHeaderInternal(std::ostream& asmout, const GscInfoOption& opt) override {
            compatibility::xensik::gscbin::GscBinHeader& data{ fakeHeader.binHeader };

            if (data.bytecodeLen) {
                asmout << "// bytecode .. 0x" << std::hex << data.bytecodeLen << "\n";
            }
            if (data.len || data.compressedLen) {
                asmout << "// buffer .... 0x" << std::hex << data.len << " / Compressed: 0x" << data.compressedLen << "\n";
            }

        }

        int PreLoadCode(T8GSCOBJContext& ctx, std::ostream& asmout) override {
            compatibility::xensik::gscbin::GscBinHeader& header{ Ref<compatibility::xensik::gscbin::GscBinHeader>() };

            auto decompressedData{ std::make_unique<byte[]>(header.len) };

            uLongf sizef = (uLongf)header.len;
            uLongf sizef2{ header.compressedLen };
            int ret;
            if (header.len && (ret = uncompress2(decompressedData.get(), &sizef, reinterpret_cast<const Bytef*>(header.GetBuffer()), &sizef2) < 0)) {
                std::string fileName{ originalFile ? originalFile->string() : "<unk>" };
                throw std::runtime_error(utils::va("Can't decompress file %s: %s", fileName.c_str(), zError(ret)));
            }

            std::vector<BINGSCExport> functions{};
            std::vector<GSC_ANIMTREE_ITEM> animTrees{};
            std::vector<GSC_USEANIMTREE_ITEM> useAnimTrees{};
            std::vector<GSCBINToken> tokens{};
            std::vector<PreImport> imports{};
            std::vector<PreString> strings{};
            std::vector<byte> stringData{};

            fakeLinked.clear();
            fakeLinked.reserve(header.len + sizeof(functions[0]) * functions.size());

            utils::Allocate(fakeLinked, sizeof(fakeHeader));
            size_t bytecode{ utils::WriteValue(fakeLinked, header.GetByteCode(), header.bytecodeLen) };


            if (header.bytecodeLen) {
                if (!ctx.opt.m_vm) {
                    LOG_INFO("GSCBIN decompiler requires a vm");
                    return tool::BASIC_ERROR;
                }

                if (!IsValidVmMagic(ctx.opt.m_vm, ctx.m_vmInfo)) {
                    LOG_ERROR("Can't find gscbin vm 0x{:x}", (uint64_t)ctx.opt.m_vm);
                    return tool::BASIC_ERROR;
                }


                core::bytebuffer::ByteBuffer sourceReader{ decompressedData.get(), sizef };
                core::bytebuffer::ByteBuffer bytecodeReader{ header.GetByteCode(), header.bytecodeLen };

                auto ReadSourceToken = [&sourceReader, &tokens, &stringData]() -> const char* {
                    GSCBINToken& token{ tokens.emplace_back() };
                    
                    token.location = (uint32_t)sourceReader.Loc();
                    uint32_t id{ sourceReader.Read<uint32_t>() };

                    if (!id) {
                        const char* str{ sourceReader.ReadString() };

                        token.type = GBTT_STRING;
                        token.val = (uint32_t)utils::WriteString(stringData, str);

                        return str;
                    }

                    token.type = GBTT_FIELD;
                    token.val = id;

                    return utils::va("ref_%x", id);
                };
                auto SkipNBytes = [&asmout, &bytecodeReader](size_t n) -> std::ostream& {
                    asmout << "{";
                    for (size_t i = 0; i < n; i++) {
                        if (i) asmout << ", ";
                        asmout << "0x" << std::hex << (int)bytecodeReader.Read<byte>();
                    }
                    return asmout << "}";
                };

                byte endOpCode{ bytecodeReader.Read<byte>() };

                const OPCodeInfo* nfoEnd{ LookupOpCode(ctx.m_vmInfo->vmMagic, ctx.opt.m_platform, endOpCode) };

                if (nfoEnd && nfoEnd->m_id != OPCODE_End) {
                    LOG_WARNING("end opcode isn't matching registered one: {:x}", (int)endOpCode);
                }

                asmout << "// end: 0x" << std::hex << (int)endOpCode << "\n";

                size_t opaqueStringCount{ ctx.m_vmInfo->opaqueStringCount };
                if (!opaqueStringCount) {
                    LOG_WARNING("No opaqueStringCount set for the VM {}, it might be an error", ctx.m_vmInfo->name);
                }


                while (!bytecodeReader.End()) {
                    BINGSCExport exph{};

                    try {
                        exph.address = (uint32_t)(bytecode + bytecodeReader.Loc());
                        byte* funcStart{ bytecodeReader.Ptr() };
                        exph.size = sourceReader.Read<uint32_t>();
                        byte* funcEnd{ funcStart + exph.size };
                        const char* name{ ReadSourceToken() };
                        uint64_t hname{ hash::Hash64(name) };
                        hashutils::AddPrecomputed(hname, name);
                        exph.name = hname;

                        asmout
                            << "\n"
                            << "// name: " << name << "\n"
                            << "// offset: 0x" << std::hex << exph.address << "\n"
                            << "// size: 0x" << std::hex << exph.size << "\n"
                            ;
                        BINGSCExport& exp{ functions.emplace_back(exph) };
                        while (bytecodeReader.Ptr() < funcEnd) {
                            asmout << "." << std::hex << std::setfill('0') << std::setw(4) << bytecodeReader.Loc() << " ";
                            byte opcode{ bytecodeReader.Read<byte>() };

                            const OPCodeInfo* nfo{ LookupOpCode(ctx.m_vmInfo->vmMagic, ctx.opt.m_platform, opcode) };

                            asmout
                                << "0x" << std::hex << std::setfill('0') << std::setw(2) << (int)opcode
                                << " " << std::setw(0x20) << std::setfill('.') << nfo->m_name << "(" << std::dec << std::setfill(' ') << std::setw(3) << (int)opcode << ")" << " | ";
                            switch (nfo->m_id) {
                            case OPCODE_Plus:
                            case OPCODE_Minus:
                            case OPCODE_Modulus:
                            case OPCODE_Vector:
                            case OPCODE_Bit_And:
                            case OPCODE_Bit_Or:
                            case OPCODE_Bit_Xor:
                            case OPCODE_BoolComplement:
                            case OPCODE_Divide:
                            case OPCODE_Multiply:
                            case OPCODE_End:
                            case OPCODE_Return:
                            case OPCODE_Nop:
                            case OPCODE_NotEqual:
                            case OPCODE_Equal:
                            case OPCODE_SuperEqual:
                            case OPCODE_SuperNotEqual:
                            case OPCODE_CastBool:
                            case OPCODE_IW_GetLevel:
                            case OPCODE_IW_GetLevelGRef:
                            case OPCODE_IW_GetAnim:
                            case OPCODE_GetAnimGRef:
                            case OPCODE_IW_GetGame:
                            case OPCODE_IW_GetGameRef:
                            case OPCODE_GetClasses:
                            case OPCODE_GetClassesObject:
                            case OPCODE_GetSelf:
                            case OPCODE_GetSelfObject:
                            case OPCODE_GetTime:
                            case OPCODE_IW_GetThread:
                            case OPCODE_CastFieldObject:
                            case OPCODE_Dec:
                            case OPCODE_DecTop:
                            case OPCODE_Inc:
                            case OPCODE_IsDefined:
                            case OPCODE_IW_IsTrue:
                            case OPCODE_CreateStruct:
                            case OPCODE_CreateArray:
                            case OPCODE_EmptyArray:
                            case OPCODE_EmptyStruct:
                            case OPCODE_Wait:
                            case OPCODE_WaitFrame:
                            case OPCODE_IW_EvalLocalVariableCached0:
                            case OPCODE_IW_EvalLocalVariableCached1:
                            case OPCODE_IW_EvalLocalVariableCached2:
                            case OPCODE_IW_EvalLocalVariableCached3:
                            case OPCODE_IW_EvalLocalVariableCached4:
                            case OPCODE_IW_EvalLocalVariableCached5:
                            case OPCODE_CheckClearParams:
                            case OPCODE_GreaterThanOrEqualTo:
                            case OPCODE_GreaterThan:
                            case OPCODE_LessThanOrEqualTo:
                            case OPCODE_LessThan:
                            case OPCODE_T10_LowerThanOrSuperEqualTo:
                            case OPCODE_T10_GreaterThanOrSuperEqualTo:
                            case OPCODE_GetZero:
                            case OPCODE_EvalArray:
                            case OPCODE_EvalArrayRef:
                            case OPCODE_SetVariableField:
                            case OPCODE_IW_SingleWaitTill:
                            case OPCODE_ClearParams:
                            case OPCODE_BoolNot:
                            case OPCODE_GSCBIN_SKIP_0:
                                asmout << "\n";
                                break;
                            case OPCODE_GetByte:
                            case OPCODE_GetNegByte:
                            case OPCODE_GetSignedByte:
                            case OPCODE_EvalLocalVariableCached:
                            case OPCODE_SetLocalVariableCached:
                            case OPCODE_IW_EvalLocalArrayCached:
                            case OPCODE_EvalLocalVariableRefCached:
                            case OPCODE_T9_IncLocalVariableCached:
                            case OPCODE_T9_DecLocalVariableCached:
                            case OPCODE_GSCBIN_SKIP_1:
                                SkipNBytes(1) << "\n";
                                break;
                            case OPCODE_GetShort:
                            case OPCODE_GetNegUnsignedShort:
                            case OPCODE_GetUnsignedShort:
                            case OPCODE_JumpOnGreaterThan:
                            case OPCODE_JumpOnLessThan:
                            case OPCODE_JumpOnTrue:
                            case OPCODE_JumpOnTrueExpr:
                            case OPCODE_JumpOnFalse:
                            case OPCODE_JumpOnFalseExpr:
                            case OPCODE_JumpOnDefined:
                            case OPCODE_JumpOnDefinedExpr:
                            case OPCODE_Jump:
                            case OPCODE_DevblockBegin:
                            case OPCODE_GSCBIN_SKIP_2:
                                SkipNBytes(2) << "\n";
                                break;
                            //case OPCODE_CallBuiltinFunction:
                            // case 0x33:
                            //case OPCODE_CallBuiltinMethod:
                            // case 0xA9:
                            // case 0xAA:
                            // case 0xAB:
                            // case 0xAC:
                            //case OPCODE_IW_ScriptFunctionCall2:
                            case OPCODE_IW_LocalCall2:
                            case OPCODE_IW_LocalCall:
                            case OPCODE_IW_GetLocal:
                            case OPCODE_GSCBIN_SKIP_3:
                                SkipNBytes(3) << "\n";
                                break;
                            case OPCODE_GetInteger:
                            case OPCODE_GetUnsignedInteger:
                            case OPCODE_GetNegUnsignedInteger:
                            case OPCODE_GetFloat:
                            case OPCODE_IW_Jump32:
                            case OPCODE_IW_LocalThreadCall:
                            case OPCODE_GSCBIN_SKIP_4:
                                SkipNBytes(4) << "\n";
                                break;
                            case OPCODE_GSCBIN_SKIP_5:
                                SkipNBytes(5) << "\n";
                                break;
                            case OPCODE_GetVector:
                                asmout 
                                    << "("
                                    << bytecodeReader.Read<float>() << ", " 
                                    << bytecodeReader.Read<float>() << ", " 
                                    << bytecodeReader.Read<float>() << ")\n"
                                    ;
                                break;
                            case OPCODE_SafeCreateLocalVariables: {
                                byte count{ bytecodeReader.Read<byte>() };
                                exp.param_count = count;
                                asmout << "count: " << std::dec << (int)count << " ";
                                SkipNBytes(count) << "\n";
                                break;
                            }
                            case OPCODE_GSCBIN_SKIP_N: {
                                byte count{ bytecodeReader.Read<byte>() };
                                asmout << "count: " << std::dec << (int)count << " ";
                                SkipNBytes(count) << "\n";
                                break;
                            }
                            case OPCODE_IW_EndSwitch: {
                                uint16_t count{ bytecodeReader.Read<uint16_t>() };
                                asmout << "count: " << std::dec << count << "\n";

                                for (size_t i = 0; i < count; i++) {
                                    asmout << "." << std::hex << std::setfill('0') << std::setw(4) << bytecodeReader.Loc() << " ";
                                    int32_t val{ bytecodeReader.Read<int32_t>() };
                                    asmout << "0x" << std::hex << val << " ";
                                    SkipNBytes(3); //rloc/type?

                                    if (val < 0x100000) {
                                        const char* valStr{ sourceReader.ReadString() };
                                        asmout << ": " << valStr;
                                    }
                                    asmout << "\n";
                                }
                                break;
                            }
                            case OPCODE_GSCBIN_SKIP_STR_TOKEN: {
                                uint32_t id{ bytecodeReader.Read<uint32_t>() };

                                if (id > opaqueStringCount) {
                                    const char* valStr{ ReadSourceToken() };
                                }
                                asmout << "\n";

                                break;
                            }
                            case OPCODE_IW_CallBuiltinFunction0:
                            case OPCODE_IW_CallBuiltinFunction1:
                            case OPCODE_IW_CallBuiltinFunction2:
                            case OPCODE_IW_CallBuiltinFunction3:
                            case OPCODE_IW_CallBuiltinFunction4:
                            case OPCODE_IW_CallBuiltinFunction5: {
                                PreImport& imp{ imports.emplace_back() };
                                imp.flags = ACTSGSCImportFlags::ACTS_CALL_BUILTIN_FUNCTION_NO_PARAMS;
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                imp.id = bytecodeReader.Read<uint16_t>();
                                asmout << "\n";
                                break;
                            }
                            case OPCODE_CallBuiltinFunction: {
                                PreImport& imp{ imports.emplace_back() };
                                imp.flags = ACTSGSCImportFlags::ACTS_CALL_BUILTIN_FUNCTION;
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                SkipNBytes(1) << "\n";
                                imp.id = bytecodeReader.Read<uint16_t>();
                                break;
                            }
                            case OPCODE_IW_CallBuiltinMethod0:
                            case OPCODE_IW_CallBuiltinMethod1:
                            case OPCODE_IW_CallBuiltinMethod2:
                            case OPCODE_IW_CallBuiltinMethod3:
                            case OPCODE_IW_CallBuiltinMethod4:
                            case OPCODE_IW_CallBuiltinMethod5: {
                                PreImport& imp{ imports.emplace_back() };
                                imp.flags = ACTSGSCImportFlags::ACTS_CALL_BUILTIN_METHOD_NO_PARAMS;
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                imp.id = bytecodeReader.Read<uint16_t>();
                                asmout << "\n";
                                break;
                            }
                            case OPCODE_CallBuiltinMethod: {
                                PreImport& imp{ imports.emplace_back() };
                                imp.flags = ACTSGSCImportFlags::ACTS_CALL_BUILTIN_METHOD;
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                SkipNBytes(1) << "\n";
                                imp.id = bytecodeReader.Read<uint16_t>();
                                break;
                            }
                            case OPCODE_IW_ScriptFunctionCall2: {
                                PreImport& imp{ imports.emplace_back() };
                                imp.flags = T8GSCImportFlags::FUNCTION;
                                asmout << ReadSourceToken() << " "; // TODO: put token in name
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                SkipNBytes(1) << "\n";
                                imp.id = bytecodeReader.Read<uint16_t>();

                                break;
                            }
                            case OPCODE_IW_GetIString:
                            case OPCODE_GetString: {
                                PreString& ps{ strings.emplace_back() };
                                const char* ds{ acts::decryptutils::DecryptString(utils::CloneString(sourceReader.ReadString())) };

                                ps.string = (uint32_t)utils::WriteString(stringData, ds);
                                ps.address = (uint32_t)bytecodeReader.Loc();
                                utils::PrintFormattedString(asmout << "\"", ds) << "\"";
                                SkipNBytes(4) << "\n";
                                break;
                            }
                            case OPCODE_GSCBIN_SKIP_4BC_1STR: {
                                const char* ds{ acts::decryptutils::DecryptString(utils::CloneString(sourceReader.ReadString())) };
                                utils::PrintFormattedString(asmout << "\"", ds) << "\"";
                                SkipNBytes(4) << "\n";
                                break;
                            }
                            case OPCODE_IW_GetAnimation: {
                                GSC_ANIMTREE_ITEM& item{ animTrees.emplace_back() };
                                const char* an1{ acts::decryptutils::DecryptString(utils::CloneString(sourceReader.ReadString())) };
                                const char* an2{ acts::decryptutils::DecryptString(utils::CloneString(sourceReader.ReadString())) };
                                item.address_str1 = (uint32_t)utils::WriteString(stringData, an1);
                                item.address_str2 = (uint32_t)utils::WriteString(stringData, an2);
                                item.num_address = (uint32_t)bytecodeReader.Loc();

                                utils::PrintFormattedString(asmout, an1);
                                utils::PrintFormattedString(asmout << ",", an2);
                                SkipNBytes(8) << "\n";
                                break;
                            }
                            case OPCODE_IW_GetAnimationTree: {
                                GSC_USEANIMTREE_ITEM& item{ useAnimTrees.emplace_back() };
                                const char* an{ acts::decryptutils::DecryptString(utils::CloneString(sourceReader.ReadString())) };
                                item.address = (uint32_t)utils::WriteString(stringData, an);
                                item.num_address = (uint32_t)bytecodeReader.Loc();

                                utils::PrintFormattedString(asmout, an);
                                SkipNBytes(1) << "\n";
                                break;
                            }
                            case OPCODE_GSCBIN_SKIP_3BC_4SD: {


                                break;
                            }
                            case OPCODE_GSCBIN_SKIP_4BC_4SD:
                            default: {
                                const char* err{ utils::va("Operator not handled 0x%x (%d/%s)", opcode, opcode, nfo->m_name) };
                                asmout << err << std::endl;
                                throw std::runtime_error(err);
                            }
                            }
                        }
                    }
                    catch (std::runtime_error& err) {
                        LOG_ERROR("Error when reading the bytecode: {}", err.what());
                        break;
                    }
                }
            }
            // write cseg
            if (fakeHeader.exports_count = functions.size()) {
                fakeHeader.exports_table = utils::WriteValue(fakeLinked, functions.data(), sizeof(functions[0]) * functions.size());
            }

            // write strings
            size_t strData{ utils::WriteValue(fakeLinked, stringData.data(), stringData.size()) };

            // write strings
            if (fakeHeader.strings_count = strings.size()) {
                fakeHeader.strings_table = fakeLinked.size();
                for (PreString& s : strings) {
                    T8GSCString& item = utils::Allocate<T8GSCString>(fakeLinked);
                    item.string = (uint32_t)(strData + s.string);
                    item.type = 0;
                    item.num_address = 1;
                    utils::WriteValue<uint32_t>(fakeLinked, (uint32_t)(bytecode + s.address));
                }
            }

            // write animtrees
            if (fakeHeader.animtree_count = animTrees.size()) {
                fakeHeader.animtree_table = fakeLinked.size();

                for (GSC_ANIMTREE_ITEM& at : animTrees) {
                    GSC_ANIMTREE_ITEM& item = utils::Allocate<GSC_ANIMTREE_ITEM>(fakeLinked);
                    item.address_str1 = (uint32_t)(strData + at.address_str1);
                    item.address_str2 = (uint32_t)(strData + at.address_str2);
                    item.num_address = 1;
                    utils::WriteValue<uint32_t>(fakeLinked, (uint32_t)(bytecode + at.num_address));
                }
            }

            // write use animtrees
            if (fakeHeader.useanimtree_count = useAnimTrees.size()) {
                fakeHeader.useanimtree_table = fakeLinked.size();

                for (GSC_USEANIMTREE_ITEM& at : useAnimTrees) {
                    GSC_USEANIMTREE_ITEM& item = utils::Allocate<GSC_USEANIMTREE_ITEM>(fakeLinked);
                    item.address = (uint32_t)(strData + at.address);
                    item.num_address = 1;
                    utils::WriteValue<uint32_t>(fakeLinked, (uint32_t)(bytecode + at.num_address));
                }
            }

            // write tokens
            if (fakeHeader.tokens_count = tokens.size()) {
                fakeHeader.tokens_table = fakeLinked.size();
                for (GSCBINToken& token : tokens) {
                    if (token.val == tool::gsc::GBTT_STRING) {
                        // use the string offset as a val
                        token.val = (uint32_t)(strData + token.val);
                    }
                }
                utils::WriteValue(fakeLinked, tokens.data(), sizeof(tokens[0]) * tokens.size());
            }

            // write imports
            if (fakeHeader.imports_count = imports.size()) {
                fakeHeader.imports_table = fakeLinked.size();
                for (PreImport& imp : imports) {
                    IW23GSCImport& item = utils::Allocate<IW23GSCImport>(fakeLinked);
                    item.name = imp.id;
                    item.name_space = 0;
                    item.param_count = 0;
                    item.flags = imp.flags;
                    item.num_address = 1;
                    utils::WriteValue<uint32_t>(fakeLinked, (uint32_t)(bytecode + imp.address));
                }
            }

            if (originalFile) {
                std::string fn = originalFile->string();

                if (originalFile->has_extension()) {
                    size_t len = originalFile->extension().string().length();
                    fn = fn.substr(0, fn.length() - len);
                }

                fn = fn + ".gsc";

                const char* name{ fn.data() };
                hashutils::Add(name);
                fakeHeader.name = hash::Hash64(name);
            }
            fakeHeader.binHeader = header;
            std::memcpy(fakeLinked.data(), &fakeHeader, sizeof(fakeHeader));
            SetFile(fakeLinked.data(), fakeLinked.size());

            return GSCOBJHandler::PreLoadCode(ctx, asmout);
        }

        uint64_t GetName() override {
            return fakeHeader.name;
        }
        uint16_t GetExportsCount() override {
            return (uint16_t)fakeHeader.exports_count;
        }
        uint32_t GetExportsOffset() override {
            return (uint32_t)fakeHeader.exports_table;
        }
        uint16_t GetIncludesCount() override {
            return 0;
        }
        uint32_t GetIncludesOffset() override {
            return 0;
        }
        uint16_t GetImportsCount() override {
            return (uint16_t)fakeHeader.imports_count;
        }
        uint32_t GetImportsOffset() override {
            return (uint32_t)fakeHeader.imports_table;
        }
        uint16_t GetGVarsCount() override {
            return 0;
        }
        uint32_t GetGVarsOffset() override {
            return 0;
        }
        uint16_t GetStringsCount() override {
            return (uint16_t)fakeHeader.strings_count;
        }
        uint32_t GetStringsOffset() override {
            return (uint32_t)fakeHeader.strings_table;
        }
        uint16_t GetDevStringsCount() override {
            return 0;
        }
        uint32_t GetDevStringsOffset() override {
            return 0;
        }
        size_t GetHeaderSize() override {
            return sizeof(compatibility::xensik::gscbin::GscBinHeader);
        }
        char* DecryptString(char* str) override {
            return acts::decryptutils::DecryptString(str);
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(compatibility::xensik::gscbin::GscBinHeader) && Ref<uint32_t>() == compatibility::xensik::gscbin::GSCBIN_MAGIC;
        }
        uint16_t GetAnimTreeSingleCount() override {
            return (uint16_t)fakeHeader.useanimtree_count;
        }
        uint32_t GetAnimTreeSingleOffset() override {
            return (uint32_t)fakeHeader.useanimtree_table;
        }
        uint16_t GetAnimTreeDoubleCount() override {
            return (uint16_t)fakeHeader.animtree_count;
        }
        uint32_t GetAnimTreeDoubleOffset() override {
            return (uint32_t)fakeHeader.animtree_table;
        }
        uint16_t GetTokensCount() override {
            return (uint16_t)fakeHeader.tokens_count;
        }
        uint32_t GetTokensOffset() override {
            return (uint32_t)fakeHeader.tokens_table;
        }

        // no name
        void SetName(uint64_t name) override {}
        void SetNameString(uint32_t name) override {}

        void SetHeader() override {
            Ref<uint32_t>() = compatibility::xensik::gscbin::GSCBIN_MAGIC;
        }
        void SetExportsCount(uint16_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetExportsOffset(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetIncludesCount(uint16_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetIncludesOffset(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetImportsCount(uint16_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetImportsOffset(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetStringsCount(uint16_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetStringsOffset(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetDevStringsCount(uint16_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetDevStringsOffset(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetGVarsCount(uint16_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetGVarsOffset(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetFileSize(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetCSEGOffset(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetCSEGSize(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        uint32_t GetCSEGOffset() override {
            return 0;
        }
        uint32_t GetCSEGSize() override {
            return 0;
        }
        void SetAnimTreeSingleCount(uint16_t val) override {}
        void SetAnimTreeSingleOffset(uint32_t val) override {}
        void SetAnimTreeDoubleCount(uint16_t val) override {
            throw std::runtime_error("not implemented");
        }
        void SetAnimTreeDoubleOffset(uint32_t val) override {
            throw std::runtime_error("not implemented");
        }
        size_t GetImportSize() override {
            return sizeof(tool::gsc::T8GSCImport);
        }
        size_t GetExportSize() override {
            return sizeof(tool::gsc::T7GSCExport);
        }
        size_t GetStringSize() override {
            return sizeof(tool::gsc::T8GSCString);
        }
        size_t GetGVarSize() override {
            return 0;
        }
        size_t GetAnimTreeSingleSize() override {
            return 0;
        }
        size_t GetAnimTreeDoubleSize() override {
            return sizeof(tool::gsc::T7GscAnimTree);
        }
        void WriteExport(byte* data, const tool::gsc::IW23GSCExport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::T7GSCExport*>(data);
            imp.name = (uint32_t)item.name;
            imp.name_space = (uint32_t)item.name_space;
            imp.checksum = (uint32_t)item.checksum;
            imp.flags = item.flags;
            imp.address = item.address;
            imp.param_count = item.param_count;
        }
        void WriteImport(byte* data, const tool::gsc::IW23GSCImport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::T8GSCImport*>(data);
            imp.name = (uint32_t)item.name;
            imp.import_namespace = (uint32_t)item.name_space;
            imp.flags = item.flags;
            imp.num_address = item.num_address;
            imp.param_count = item.param_count;
        }
        void WriteGVar(byte* data, const tool::gsc::T8GSCGlobalVar& item) override {}
        void WriteString(byte* data, const tool::gsc::T8GSCString& item) override {
            *reinterpret_cast<tool::gsc::T8GSCString*>(data) = item;
        }
        void WriteAnimTreeSingle(byte* data, const tool::gsc::GSC_USEANIMTREE_ITEM& item) override { }
        void WriteAnimTreeDouble(byte* data, const tool::gsc::GSC_ANIMTREE_ITEM& item) override { }
        int64_t GetDefaultChecksum(bool client) override {
            return 0x1234567;
        }
        void SetChecksum(uint64_t val) override {

        }
        uint32_t GetChecksum() override {
            return 0;
        }
        const char* GetDefaultName(bool client) override {
            return "";
        }

        bool IsVTableImportFlags(byte flags) override {
            return false;
        }
        byte GetVTableImportFlags() override {
            return 0;
        }
    };
    void OpCode() {

        VmInfo* gscbin = RegisterVM(VMI_IW_GSCBIN, "GSCBIN", "gscbin", "gscbin", VmFlags::VMF_GSCBIN | VmFlags::VMF_NO_VERSION);
        gscbin->RegisterVmName("gscbin");
        gscbin->AddPlatform(PLATFORM_PC);

    }

}
REGISTER_GSC_VM(VMI_IW_GSCBIN, IWGSCOBJHandler);
REGISTER_GSC_VM_OPCODES(gscbin, OpCode);