#pragma once
#include <decryptutils.hpp>
#include <core/bytebuffer.hpp>
#include <compatibility/xensik_gscbin.hpp>
#include <tools/gsc/gsc_vm.hpp>
#include <tools/gsc/gsc.hpp>
#include <tools/gsc/gsc_opcodes.hpp>
#include <tools/gsc/gsc_iw.hpp>


// GSCBIN

namespace tool::gsc {
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
        uint64_t ns;
        uint64_t name;
        byte flags;
        uint32_t address;
        byte params;
    };

    uint64_t ConvertToHash(const char* name) {
        uint64_t h{ hash::HashJupScr(name) };
        hashutils::AddPrecomputed(h, name);
        return h;
    }
    inline uint64_t ConvertToHash(const std::string& name) { return ConvertToHash(name.data()); }

    uint64_t CreateScriptNameSpace(const char* name) {
        std::string_view s{ name };
        if (!s.ends_with(".gsc")) {
            return ConvertToHash(name);
        }
        return ConvertToHash(std::string{ s.begin(), s.end() - 4 });
    }


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
            size_t bytecode{ utils::Allocate(fakeLinked, header.bytecodeLen) };

            std::unordered_map<uint64_t, uint64_t> locals{};

            auto GetLocalId = [&locals](uint64_t loc) -> uint64_t {
                auto it{ locals.find(loc) };
                if (it != locals.end()) {
                    return it->second;
                }
                return ConvertToHash(utils::va("<errlocal:%x>", loc));
            };

            if (header.bytecodeLen) {

                core::bytebuffer::ByteBuffer sourceReader{ decompressedData.get(), sizef };
                core::bytebuffer::ByteBuffer bytecodeReader{ header.GetByteCode(), header.bytecodeLen };

                auto AddTokenID = [&ctx, &sourceReader, &tokens](uint32_t* base, uint32_t id) {
                    *base = (uint32_t)tokens.size();
                    GSCBINToken& token{ tokens.emplace_back() };
                    //token.location = 0;
                    token.type = GBTT_FIELD;
                    token.val = id;
                };

                auto ReadSourceToken = [&ctx, &sourceReader, &tokens, &stringData](uint32_t* oid = nullptr, uint32_t* outtokenid = nullptr) -> const char* {
                    if (outtokenid) *outtokenid = (uint32_t)tokens.size();
                    GSCBINToken& token{ tokens.emplace_back() };
                    
                    token.location = (uint32_t)sourceReader.Loc();
                    uint32_t id{ sourceReader.Read<uint32_t>() };

                    if (oid) *oid = id;

                    if (!id) {
                        char* str{ sourceReader.ReadString() };

                        token.type = GBTT_STRING;
                        token.val = (uint32_t)utils::WriteString(stringData, str);

                        return acts::decryptutils::DecryptString(str);
                    }

                    token.type = GBTT_FIELD;
                    token.val = id;

                    return tool::gsc::iw::GetOpaqueStringForVm(ctx.m_vmInfo->vmMagic, id);
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
                        uint32_t trueLoc{ (uint32_t) bytecodeReader.Loc() };
                        exph.address = (uint32_t)(bytecode + trueLoc);
                        byte* funcStart{ bytecodeReader.Ptr() };
                        exph.size = sourceReader.Read<uint32_t>();
                        byte* funcEnd{ funcStart + exph.size };
                        uint32_t funcId;
                        const char* name{ ReadSourceToken(&funcId) };
                        uint64_t hname{ hash::HashJupScr(name) };
                        hashutils::AddPrecomputed(hname, name);
                        exph.name = hname;
                        locals[trueLoc] = hname;

                        asmout
                            << "\n"
                            << "// name: " << name << " (0x" << std::hex << funcId << ")" << "\n"
                            << "// offset: 0x" << std::hex << exph.address << "\n"
                            << "// size: 0x" << std::hex << exph.size << "/ 0x" << (exph.address + exph.size) << "\n"
                            ;
                        BINGSCExport& exp{ functions.emplace_back(exph) };
                        while (bytecodeReader.Ptr() < funcEnd) {
                            asmout 
                                << "." << std::hex << std::setfill('0') << std::setw(4) << bytecodeReader.Loc()
                                << "." << std::hex << std::setfill('0') << std::setw(4) << sourceReader.Loc() 
                                << " ";
                            byte opcode{ bytecodeReader.Read<byte>() };

                            const OPCodeInfo* nfo{ LookupOpCode(ctx.m_vmInfo->vmMagic, ctx.opt.m_platform, opcode) };

                            asmout
                                << "0x" << std::hex << std::setfill('0') << std::setw(2) << (int)opcode
                                << " " << std::setw(0x24) << std::setfill('.') << nfo->m_name << "(" << std::dec << std::setfill(' ') << std::setw(3) << (int)opcode << ")" << " | ";
                            switch (nfo->m_id) {
                            case OPCODE_Plus:
                            case OPCODE_Minus:
                            case OPCODE_Modulus:
                            case OPCODE_Vector:
                            case OPCODE_Bit_And:
                            case OPCODE_Bit_Or:
                            case OPCODE_Bit_Xor:
                            case OPCODE_ShiftLeft:
                            case OPCODE_ShiftRight:
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
                            case OPCODE_IW_IncRef:
                            case OPCODE_IW_DecRef:
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
                            case OPCODE_WaitTillFrameEnd:
                            case OPCODE_PreScriptCall:
                            case OPCODE_SizeOf:
                            case OPCODE_IW_SetLocalVariableCached0:
                            case OPCODE_IW_ClearLocalVariableCached0:
                            case OPCODE_IW_WaitFrame:
                            case OPCODE_GetUndefined:
                            case OPCODE_IW_Notify:
                            case OPCODE_IW_SingleEndon:
                            case OPCODE_IW_VoidCodePos:
                            case OPCODE_ClearArray:
                            case OPCODE_IW_EvalLocalVariableRefCached0:
                            case OPCODE_ScriptMethodCallPointer:
                            case OPCODE_ScriptFunctionCallPointer:
                            case OPCODE_AddToArray:
                            case OPCODE_IW_AppendToArray:
                            case OPCODE_GSCBIN_SKIP_0:
                                asmout << "\n";
                                break;
                            case OPCODE_GetByte:
                            case OPCODE_GetNegByte:
                            case OPCODE_GetSignedByte:
                            case OPCODE_EvalLocalVariableCached:
                            case OPCODE_SetLocalVariableCached:
                            case OPCODE_IW_EvalLocalArrayCached:
                            case OPCODE_IW_EvalLocalArrayRefCached:
                            case OPCODE_EvalLocalVariableRefCached:
                            case OPCODE_IW_EvalLocalVariableObjectCached:
                            case OPCODE_T9_IncLocalVariableCached:
                            case OPCODE_T9_DecLocalVariableCached:
                            case OPCODE_IW_SetNewLocalVariableFieldCached0:
                            case OPCODE_IW_CreateLocalVar:
                            case OPCODE_IW_ClearFieldVariableRef:
                            case OPCODE_IW_SetWaittillVariableFieldCached:
                            case OPCODE_IW_RemoveVariables:
                            case OPCODE_IW_BuiltinFunctionCallPointer:
                            case OPCODE_IW_BuiltinMethodCallPointer:
                            case OPCODE_ScriptMethodThreadCallPointerEndOn:
                            case OPCODE_ScriptMethodThreadCallPointer:
                            case OPCODE_ScriptThreadCallPointerEndOn:
                            case OPCODE_ScriptThreadCallPointer:
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
                            case OPCODE_IW_JumpBack:
                            case OPCODE_DevblockBegin:
                            case OPCODE_IW_WaitTillMatch:
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
                            case OPCODE_GSCBIN_SKIP_3:
                                SkipNBytes(3) << "\n";
                                break;
                            case OPCODE_GetInteger:
                            case OPCODE_GetUnsignedInteger:
                            case OPCODE_GetNegUnsignedInteger:
                            case OPCODE_GetFloat:
                            case OPCODE_IW_Jump32:
                            case OPCODE_GSCBIN_SKIP_4:
                                SkipNBytes(4) << "\n";
                                break;
                            case OPCODE_IW_Switch: {
                                int32_t delta{ bytecodeReader.Read<int32_t>() };
                                asmout << "delta: ";

                                if (delta < 0) asmout << "-0x" << std::hex << -delta;
                                else asmout << "0x" << std::hex << delta;
                                
                                asmout << " table: ." << bytecodeReader.Loc() + delta << std::endl;
                                break;
                            }
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
                                    asmout
                                        << "." << std::hex << std::setfill('0') << std::setw(4) << bytecodeReader.Loc()
                                        << "." << std::hex << std::setfill('0') << std::setw(4) << sourceReader.Loc()
                                        << std::setfill(' ') << std::setw(50) << " ";

                                    union SwitchCase {
                                        struct {
                                            int32_t scr;
                                            int16_t rloc;
                                        };
                                        byte __pad[7];
                                    };

                                    size_t rloc{ bytecodeReader.Loc() };
                                    SwitchCase* switchCase{bytecodeReader.Ptr<SwitchCase>()};

                                    if (switchCase->scr < 0x100000) { // 0x80000 in pdb
                                        char* valStr{ sourceReader.ReadString() };
                                        valStr = acts::decryptutils::DecryptString(valStr);

                                        if (valStr[0] == 1 && !valStr[1]) {
                                            asmout << "default:";
                                            switchCase->scr = 0xdefdef;
                                        }
                                        else {
                                            PreString& ps{ strings.emplace_back() };
                                            ps.string = (uint32_t)utils::WriteString(stringData, valStr);
                                            ps.address = (uint32_t)rloc;
                                            asmout << "case \"" << utils::FormattedString(valStr) << "\":";
                                        }

                                    }
                                    else {
                                        switchCase->scr &= 0x7FFFFF;
                                        asmout << "case 0x" << std::hex << switchCase->scr << ":";
                                    }
                                    if (switchCase->rloc < 0) {
                                        asmout << " -0x" << std::hex << -switchCase->rloc;
                                    }
                                    else {
                                        asmout << " 0x" << std::hex << switchCase->rloc;
                                    }
                                        
                                    asmout << " (" << std::dec << switchCase->rloc << ")" << std::endl;
                                    
                                    bytecodeReader.Skip(sizeof(SwitchCase::__pad));
                                }
                                break;
                            }
                            case OPCODE_IW_EvalLevelFieldVariableToken:
                            case OPCODE_IW_EvalLevelFieldVariableTokenRef:
                            case OPCODE_IW_EvalAnimFieldVariableToken:
                            case OPCODE_IW_EvalAnimFieldVariableTokenRef:
                            case OPCODE_IW_SetLevelFieldVariableToken:
                            case OPCODE_IW_SetSelfFieldVariableToken:
                            case OPCODE_IW_SetAnimFieldVariableToken:
                            case OPCODE_IW_EvalFieldVariableToken:
                            case OPCODE_IW_EvalFieldVariableTokenRef:
                            case OPCODE_IW_EvalSelfFieldVariableToken:
                            case OPCODE_IW_EvalSelfFieldVariableTokenRef:
                            case OPCODE_IW_ClearFieldVariableToken:
                            case OPCODE_GSCBIN_SKIP_STR_TOKEN: {
                                uint32_t* idLoc{ bytecodeReader.Ptr<uint32_t>() };
                                uint32_t id{ bytecodeReader.Read<uint32_t>() };

                                asmout << "0x" << std::hex << id << " ";
                                if (id > opaqueStringCount) {
                                    const char* valStr{ ReadSourceToken(nullptr, idLoc) };
                                    asmout << ": " << valStr;
                                }
                                else {
                                    AddTokenID(idLoc, id);
                                    asmout << ": " << tool::gsc::iw::GetOpaqueStringForVm(ctx.m_vmInfo->vmMagic, id);
                                }
                                asmout << " -> 0x" << std::hex << *idLoc << "\n";

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
                                uint16_t fid{ bytecodeReader.Read<uint16_t>() };
                                const char* t{ tool::gsc::iw::GetFunctionForVm(ctx.m_vmInfo->vmMagic, fid) };
                                imp.name = ConvertToHash(t);
                                asmout << t << " 0x" << std::hex << fid << "\n";
                                break;
                            }
                            case OPCODE_CallBuiltinFunction: {
                                PreImport& imp{ imports.emplace_back() };
                                imp.flags = ACTSGSCImportFlags::ACTS_CALL_BUILTIN_FUNCTION;
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                imp.params = bytecodeReader.Read<byte>();
                                uint16_t fid{ bytecodeReader.Read<uint16_t>() };
                                const char* t{ tool::gsc::iw::GetFunctionForVm(ctx.m_vmInfo->vmMagic, fid) };
                                imp.name = ConvertToHash(t);
                                asmout << t << " 0x" << std::hex << fid << "\n";
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
                                uint16_t fid{ bytecodeReader.Read<uint16_t>() };
                                const char* t{ tool::gsc::iw::GetMethodForVm(ctx.m_vmInfo->vmMagic, fid) };
                                imp.name = ConvertToHash(t);
                                asmout << t << " 0x" << std::hex << fid << "\n";
                                break;
                            }
                            case OPCODE_CallBuiltinMethod: {
                                PreImport& imp{ imports.emplace_back() };
                                imp.flags = ACTSGSCImportFlags::ACTS_CALL_BUILTIN_METHOD;
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                imp.params = bytecodeReader.Read<byte>();
                                uint16_t fid{ bytecodeReader.Read<uint16_t>() };
                                const char* t{ tool::gsc::iw::GetMethodForVm(ctx.m_vmInfo->vmMagic, fid) };
                                imp.name = ConvertToHash(t);
                                asmout << t << " 0x" << std::hex << fid << "\n";
                                break;
                            }
                            case OPCODE_ScriptMethodThreadCallEndOn:
                            case OPCODE_ScriptMethodThreadCall:
                            case OPCODE_ScriptThreadCall:
                            case OPCODE_ScriptThreadCallEndOn: {
                                uint32_t sid, fid;
                                const char* script{ ReadSourceToken(&sid) };
                                const char* func{ ReadSourceToken(&fid) };
                                asmout << ": " << script << "::" << func << "(0x" << std::hex << sid << "::0x" << fid << ") ";

                                PreImport& imp{ imports.emplace_back() };
                                switch (nfo->m_id) {
                                case OPCODE_ScriptMethodThreadCallEndOn:
                                    imp.flags = T8GSCImportFlags::METHOD_CHILDTHREAD;
                                    break;
                                case OPCODE_ScriptMethodThreadCall:
                                    imp.flags = T8GSCImportFlags::METHOD_THREAD;
                                    break;
                                case OPCODE_ScriptThreadCall:
                                    imp.flags = T8GSCImportFlags::FUNCTION_THREAD;
                                    break;
                                case OPCODE_ScriptThreadCallEndOn:
                                    imp.flags = T8GSCImportFlags::FUNCTION_CHILDTHREAD;
                                    break;
                                default: throw std::runtime_error("INVALID THREADCALL SKIP");
                                }
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                imp.name = ConvertToHash(func);
                                imp.ns = CreateScriptNameSpace(script);
                                SkipNBytes(3);
                                imp.params = bytecodeReader.Read<byte>();
                                asmout << "\n";
                                break;
                            }
                            case OPCODE_ScriptMethodCall: {
                                uint32_t sid, fid;
                                const char* script{ ReadSourceToken(&sid) };
                                const char* func{ ReadSourceToken(&fid) };
                                asmout << ": " << script << "::" << func << "(0x" << std::hex << sid << "::0x" << fid << ") ";

                                PreImport& imp{ imports.emplace_back() };
                                imp.flags = T8GSCImportFlags::METHOD;
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                imp.name = ConvertToHash(func);
                                imp.ns = CreateScriptNameSpace(script);

                                SkipNBytes(3) << "\n";
                                break;
                            }
                            case OPCODE_IW_ScriptFunctionCallFar2:
                            case OPCODE_ScriptFunctionCall: {
                                uint32_t sid, fid;
                                const char* script{ ReadSourceToken(&sid) };
                                const char* func{ ReadSourceToken(&fid) };
                                asmout << ": " << script << "::" << func << "(0x" << std::hex << sid << "::0x" << fid << ") ";

                                PreImport& imp{ imports.emplace_back() };
                                imp.flags = T8GSCImportFlags::FUNCTION;
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                imp.name = ConvertToHash(func);
                                imp.ns = CreateScriptNameSpace(script);

                                SkipNBytes(3) << "\n";
                                break;
                            }
                            case OPCODE_IW_GetBuiltinFunction:
                            case OPCODE_IW_GetBuiltinMethod: {
                                PreImport& imp{ imports.emplace_back() };
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                const char* t;
                                uint16_t fid{ bytecodeReader.Read<uint16_t>() };
                                if (nfo->m_id == OPCODE_IW_GetBuiltinFunction) {
                                    t = tool::gsc::iw::GetFunctionForVm(ctx.m_vmInfo->vmMagic, fid);
                                    imp.flags = ACTSGSCImportFlags::ACTS_GET_BUILTIN_FUNCTION;
                                }
                                else if (nfo->m_id == OPCODE_IW_GetBuiltinMethod) {
                                    t = tool::gsc::iw::GetMethodForVm(ctx.m_vmInfo->vmMagic, fid);
                                    imp.flags = ACTSGSCImportFlags::ACTS_GET_BUILTIN_METHOD;
                                }
                                else {
                                    throw std::runtime_error("INVALID FUNC/METHOD blt opcode");
                                }
                                imp.name = ConvertToHash(t);
                                asmout << "get builtin " << t << std::endl;
                                break;
                            }
                            case OPCODE_GetResolveFunction: {
                                uint32_t sid, fid;
                                const char* script{ ReadSourceToken(&sid) };
                                const char* func{ ReadSourceToken(&fid) };
                                asmout << ": " << script << "::" << func << "(0x" << std::hex << sid << "::0x" << fid << ") ";

                                PreImport& imp{ imports.emplace_back() };
                                imp.flags = T8GSCImportFlags::FUNC_METHOD;
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                imp.name = ConvertToHash(func);
                                imp.ns = CreateScriptNameSpace(script);

                                SkipNBytes(3) << "\n";
                                break;
                            }
                            case OPCODE_IW_GetLocal: {
                                if (!bytecodeReader.CanRead(3)) {
                                    throw std::runtime_error("Can't read 3 byte for getlocal");
                                }
                                int32_t localDelta{ (*bytecodeReader.Ptr<int32_t>() << 8) >> 8 };

                                PreImport& imp{ imports.emplace_back() };
                                imp.flags = T8GSCImportFlags::FUNC_METHOD | T8GSCImportFlags::LOCAL_CALL;
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                imp.name = imp.address + localDelta;
                                bytecodeReader.Skip(3);
                                asmout << "(0x" << std::hex << localDelta << ")\n";
                                break;
                            }
                            case OPCODE_IW_LocalMethodCall:
                            case OPCODE_IW_LocalMethodCall2: {
                                if (!bytecodeReader.CanRead(3)) {
                                    throw std::runtime_error("Can't read 3 byte for localcall method");
                                }
                                int32_t localDelta{ (*bytecodeReader.Ptr<int32_t>() << 8) >> 8 };

                                PreImport& imp{ imports.emplace_back() };
                                imp.flags = T8GSCImportFlags::METHOD | T8GSCImportFlags::LOCAL_CALL;
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                imp.name = imp.address + localDelta;
                                bytecodeReader.Skip(3);
                                asmout << "(0x" << std::hex << localDelta << ")\n";
                                break;
                            }
                            case OPCODE_IW_LocalCall2:
                            case OPCODE_IW_LocalCall: {
                                if (!bytecodeReader.CanRead(3)) {
                                    throw std::runtime_error("Can't read 3 byte for localcall");
                                }
                                int32_t localDelta{ (*bytecodeReader.Ptr<int32_t>() << 8) >> 8 };

                                PreImport& imp{ imports.emplace_back() };
                                imp.flags = T8GSCImportFlags::FUNCTION | T8GSCImportFlags::LOCAL_CALL;
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                imp.name = imp.address + localDelta;
                                bytecodeReader.Skip(3);
                                asmout << "(0x" << std::hex << localDelta << ")\n";
                                break;
                            }
                            case OPCODE_IW_LocalThreadCall: {
                                if (!bytecodeReader.CanRead(3)) {
                                    throw std::runtime_error("Can't read 3 byte for localcall");
                                }
                                int32_t localDelta{ (*bytecodeReader.Ptr<int32_t>() << 8) >> 8 };

                                PreImport& imp{ imports.emplace_back() };
                                imp.flags = T8GSCImportFlags::FUNCTION_THREAD | T8GSCImportFlags::LOCAL_CALL;
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                imp.name = imp.address + localDelta;
                                bytecodeReader.Skip(3);
                                imp.params = bytecodeReader.Read<byte>();
                                asmout << "(0x" << std::hex << localDelta << ")\n";
                                break;
                            }
                            case OPCODE_IW_LocalMethodThreadCall: {
                                if (!bytecodeReader.CanRead(3)) {
                                    throw std::runtime_error("Can't read 3 byte for localcall");
                                }
                                int32_t localDelta{ (*bytecodeReader.Ptr<int32_t>() << 8) >> 8 };

                                PreImport& imp{ imports.emplace_back() };
                                imp.flags = T8GSCImportFlags::METHOD_THREAD | T8GSCImportFlags::LOCAL_CALL;
                                imp.address = (uint32_t)bytecodeReader.Loc();
                                imp.name = imp.address + localDelta;
                                bytecodeReader.Skip(3);
                                imp.params = bytecodeReader.Read<byte>();
                                asmout << "(0x" << std::hex << localDelta << ")\n";
                                break;
                            }
                            case OPCODE_IW_GetIString:
                            case OPCODE_GetString: {
                                PreString& ps{ strings.emplace_back() };
                                const char* ds{ acts::decryptutils::DecryptString(utils::CloneString(sourceReader.ReadString())) };

                                ps.string = (uint32_t)utils::WriteString(stringData, ds);
                                ps.address = (uint32_t)bytecodeReader.Loc();
                                if (nfo->m_id == OPCODE_IW_GetIString) {
                                    asmout << "&" << utils::FormattedString(ds);
                                }
                                else {
                                    asmout << "\"" << utils::FormattedString(ds) << "\"";
                                }
                                SkipNBytes(4) << "\n";
                                break;
                            }
                            case OPCODE_GSCBIN_SKIP_4BC_1STR: {
                                const char* ds{ acts::decryptutils::DecryptString(utils::CloneString(sourceReader.ReadString())) };
                                asmout << "\"" << utils::FormattedString(ds) << "\"";
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

                                asmout << utils::FormattedString(an1) << "," << utils::FormattedString(an2);
                                SkipNBytes(8) << "\n";
                                break;
                            }
                            case OPCODE_IW_GetAnimationTree: {
                                GSC_USEANIMTREE_ITEM& item{ useAnimTrees.emplace_back() };
                                const char* an{ acts::decryptutils::DecryptString(utils::CloneString(sourceReader.ReadString())) };
                                item.address = (uint32_t)utils::WriteString(stringData, an);
                                item.num_address = (uint32_t)bytecodeReader.Loc();

                                asmout << utils::FormattedString(an);
                                SkipNBytes(1) << "\n";
                                break;
                            }
                            case OPCODE_GSCBIN_SKIP_3BC_4SD: {


                                break;
                            }
                            case OPCODE_GSCBIN_SKIP_4BC_4SD:
                            default: {
                                const char* err{ utils::va("Operator not handled 0x%x (%d/%s) for vm %s", opcode, opcode, nfo->m_name, ctx.m_vmInfo->name) };
                                asmout << err << std::endl;
                                throw std::runtime_error(err);
                            }
                            }
                        }
                        if (bytecodeReader.Ptr() != funcEnd) {
                            throw std::runtime_error(std::format("Reading too much for function"));
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
            std::memcpy(fakeLinked.data() + bytecode, header.GetByteCode(), header.bytecodeLen);

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
                    if (token.type == tool::gsc::GBTT_STRING) {
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
                    if (imp.flags & T8GSCImportFlags::LOCAL_CALL) {
                        item.name = GetLocalId(imp.name);
                    }
                    else {
                        item.name = imp.name;
                    }
                    item.name_space = imp.ns;
                    item.param_count = imp.params;
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
                fakeHeader.name = hash::HashJupScr(name);
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

}