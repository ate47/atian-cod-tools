#include <includes.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_object.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_asm.hpp>
#include <tools/gsc/gsc_iw.hpp>
#include <tools/gsc/gsc_vm.hpp>

namespace tool::gsc {
    using namespace tool::gsc::opcode;

    T8GSCOBJContext::T8GSCOBJContext(GscDecompilerGlobalContext& gdctx) : opt(gdctx.opt), gdctx(gdctx) {}

    T8GSCOBJContext::~T8GSCOBJContext() {
        for (auto& [k, string] : m_allocatedStrings) {
            delete[] string;
        }
    }

    const char* T8GSCOBJContext::GetFLocName(uint32_t floc) const {
        // check the exports to find the right floc name
        uint32_t off = scriptfile->GetExportsOffset();

        byte* exportTable = scriptfile->Ptr(off);

        uint32_t max{};
        void* maxPtr{};
        for (size_t i = 0; i < scriptfile->GetExportsCount(); i++) {
            void* ptr = exportTable + exp->SizeOf() * i;
            if (scriptfile->GetExportsOffset() + exp->SizeOf() * (i + 1) > scriptfile->GetFileSize()) {
                throw std::runtime_error("Invalid export size");
            }
            exp->SetHandle(ptr);

            uint32_t addr = exp->GetAddress();

            if (addr <= floc && addr > max) {
                maxPtr = ptr;
                max = addr;
            }
        }

        if (maxPtr) {
            exp->SetHandle(maxPtr);
            return utils::va(
                "%s::%s@%x",
                hashutils::ExtractTmpPath("namespace", exp->GetNamespace()),
                hashutils::ExtractTmp("function", exp->GetName()),
                floc - exp->GetAddress()
            );
        }

        return utils::va("unk:%lx", floc);
    }


    // apply ~ to ref to avoid using 0, 1, 2 which might already be used

    uint64_t T8GSCOBJContext::GetGlobalVarName(uint16_t gvarRef) {
        auto f = m_gvars.find(gvarRef);
        if (f == m_gvars.end()) {
            return 0;
        }
        return f->second;
    }

    const char* T8GSCOBJContext::GetStringValue(uint32_t stringRef) {
        auto f = m_stringRefs.find(stringRef);
        if (f == m_stringRefs.end()) {
            return nullptr;
        }
        return f->second;
    }
    void T8GSCOBJContext::AddStringRef(uint32_t floc, uint32_t str) {
        m_stringRefsLoc[floc] = str;
    }

    const char* T8GSCOBJContext::GetStringValueByLoc(uint32_t floc) {
        auto f = m_stringRefsLoc.find(floc);
        if (f == m_stringRefsLoc.end()) {
            return nullptr;
        }
        return GetStringValue(f->second);
    }

    const char* T8GSCOBJContext::GetTokenValue(uint32_t tokenRef) {
        if (tokenRef >= m_tokens.size()) {
            return CloneString(utils::va("<badtoken:%x>", tokenRef));
        }
        GSCOBJTokenData& td{ m_tokens[tokenRef] };
        if (td.isString) return td.val.str;
        const char* extracted{ tool::gsc::iw::GetOpaqueStringForVm(m_vmInfo->vmMagic, td.val.id, false) };
        if (extracted) return extracted;
        return CloneString(utils::va("ref_%x", td.val.id));
    }

    const char* T8GSCOBJContext::GetStringValueOrError(uint32_t stringRef, uint32_t floc, const char* errorValue) {
        const char* v = GetStringValue(stringRef);

        if (v) {
            return v;
        }
        return GetStringError(floc, errorValue);
    }

    const char* T8GSCOBJContext::GetStringError(uint32_t floc, const char* errorValue) {
        if (errorValue) {
            m_unkstrings[errorValue].insert(floc);
        }

        return errorValue;
    }

    uint16_t T8GSCOBJContext::AddGlobalVarName(uint64_t value) {
        uint16_t id = ((uint16_t)m_gvars.size());
        m_gvars[id] = value;
        return id;
    }

    uint32_t T8GSCOBJContext::AddStringValue(const char* value) {
        uint32_t id = ((uint32_t)m_stringRefs.size());
        m_stringRefs[id] = value;
        return id;
    }
    char* T8GSCOBJContext::CloneString(const char* str) {
        char*& r{ m_allocatedStrings[hash::Hash64(str)] };
        if (r) {
            return r;
        }
        size_t len = std::strlen(str);
        char* ptr = new char[len + 1];

        memcpy(ptr, str, len + 1);

        r = ptr;

        return ptr;
    }

    int DumpAsm(GSCExportReader& exp, std::ostream& out, GSCOBJHandler& gscFile, T8GSCOBJContext& objctx, ASMContext& ctx) {
        uint32_t baseloc{ exp.GetAddress() };
        uint64_t filename{ ctx.m_gscReader.GetName() };
        // main reading loop
        while (ctx.FindNextLocation()) {
            while (true) {
                if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN) && objctx.m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16)) {
                    ctx.Aligned<uint16_t>();
                }
                uint32_t floc = ctx.ScriptAbsoluteLocation();
                if (ctx.m_bcl < gscFile.Ptr() || floc >= gscFile.GetFileSize()) {
                    throw std::runtime_error(std::format("FIND location after file 0x{:x}", floc));
                }
                byte*& base = ctx.m_bcl;

                // mark the current location as handled
                auto& loc = ctx.PushLocation();
                loc.handled = true;

                if (ctx.m_lastOpCodeBase == -1) {
                    ctx.m_lastOpCodeBase = loc.rloc;
                }
                // print the stack and the fields
                auto printStack = [&ctx, &out, &loc](const char* type) {
                    if (ctx.m_opt.m_dcomp && ctx.m_opt.m_display_stack) {
                        out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << loc.rloc << ":"
                            << std::setfill(' ') << std::setw(6) << std::left << " " << std::right
                            << std::setfill(' ') << std::setw(26) << std::left << type << std::right
                            << "stack(" << std::dec << ctx.m_stack.size() << "): "
                            << std::flush;

                        for (const auto* node : ctx.m_stack) {
                            out << "<" << *node << "> ";
                        }
                        out << std::endl;
                        out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << loc.rloc << ":"
                            << std::setfill(' ') << std::setw(32) << std::left << " " << std::right
                            << "fieldid: <";
                        if (ctx.m_fieldId) {
                            out << *ctx.m_fieldId;
                        }
                        else {
                            out << "none";
                        }
                        out << "> objectid: <";
                        if (ctx.m_objectId) {
                            out << *ctx.m_objectId;
                        }
                        else {
                            out << "none";
                        }
                        out << ">"
                            << std::flush;

                        out << std::endl;
                    }

                    };

                // compute the late operations (OR|AND)
                for (const auto& lateop : loc.m_lateop) {
                    lateop->Run(ctx, objctx);
                    printStack(lateop->type);
                }

                uint16_t opCode;

                if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16)) {
                    opCode = ctx.Read<uint16_t>(base);
                }
                else {
                    opCode = (uint16_t)ctx.Read<byte>(base);
                }

                //if (objctx.m_vmInfo->modToolFlag && (opCode & objctx.m_vmInfo->modToolFlag)) {
                //    // modtool
                //}

                const auto* handler = ctx.LookupOpCode(opCode);

                if (ctx.m_opt.m_func_floc) {
                    out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << (baseloc + loc.rloc);
                }

                if (ctx.m_opt.m_show_opcode_values) {
                    out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << loc.rloc << ": " << std::flush;
                }

                const char* opcodeName{};
                if (ctx.m_opt.m_use_internal_names) {
                    opcodeName = opcode::OpCodeName(handler->m_id);
                }
                else {
                    opcodeName = handler->m_name;
                }
                if (!opcodeName) {
                    opcodeName = utils::va("NO_NAME_%d", handler->m_id);
                }

                if (opCode > objctx.m_vmInfo->maxOpCode) {
                    throw std::runtime_error(std::format("FIND errec {} (0x{:x} > 0x{:x})", opcodeName, opCode, objctx.m_vmInfo->maxOpCode));
                    opCode &= objctx.m_vmInfo->maxOpCode;
                    break;
                }

                if (ctx.m_opt.m_show_opcode_values) {
                    out << std::hex << std::setfill('0') << std::setw(sizeof(int16_t) << 1) << opCode
                        << " ";
                }

                out << std::setfill(' ') << std::setw(25) << std::left
                    << opcodeName
                    << std::right << " " << std::flush;

                // dump translation data
                uint32_t opcodeRloc{ (uint32_t)(reinterpret_cast<uint64_t>(base) - reinterpret_cast<uint64_t>(gscFile.Ptr())) };
                if (ctx.m_objctx.gdctx.opcodesLocs) {
                    (*ctx.m_objctx.gdctx.opcodesLocs)[filename].insert(opcodeRloc);
                }

                // pass the opcode

                byte* opBase{ ctx.m_bcl };

                if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16)) {
                    base += 2;
                }
                else {
                    base++;
                }

                // update ASMContext::WritePadding if you change the format

                int ret = handler->Dump(out, opCode, ctx, objctx);

                if (ctx.m_opt.m_dataDump) {
                    if (ctx.m_bcl >= opBase) {
                        size_t l{ (size_t)(ctx.m_bcl - opBase) };
                        out << "// data (0x" << std::hex << l << ")";
                        for (size_t i = 0; i < l; i++) {
                            out << " " << std::setfill('0') << std::setw(2) << (int)opBase[i];
                        }

                        out << std::endl;
                    }
                    else {
                        out << "// ERROR: NEW BASE AFTER OLD BASE" << std::endl;
                    }
                }

                if (ret) {
                    break;
                }

                printStack("endop");
            }
        }
        // no more location, we can assume the final size
        // maybe we don't end on a return/end, to check?
        out << "// final size: 0x" << std::hex << ctx.FinalSize() << std::endl;

        return 0;
    }

    DumpVTableAnswer DumpVTable(GSCExportReader& exp, std::ostream& out, GSCOBJHandler& gscFile, T8GSCOBJContext& objctx, ASMContext& ctx, DecompContext& dctxt) {
        using namespace tool::gsc::opcode;
        auto FetchCode = [&ctx, &dctxt, &out]() -> const OPCodeInfo* {
            dctxt.rloc = ctx.FunctionRelativeLocation();
            uint16_t code;
            if (ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16)) {
                if (ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
                    ctx.Aligned<uint16_t>();
                }
                code = *(uint16_t*)ctx.m_bcl;
                ctx.m_bcl += 2;
            }
            else {
                code = (uint16_t)*ctx.m_bcl;
                ctx.m_bcl += 1;
            }
            const OPCodeInfo* ret{ ctx.LookupOpCode(code) };

            if (!ret || ret->m_id == OPCODE_Undefined) {
                dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << code << std::endl;
            }

            return ret;
            };

        // main reading loop
        const OPCodeInfo* ccp = FetchCode();

        /*
         * Start
         .00000000: 000d CheckClearParams
         .00000002: 000e PreScriptCall
         .00000004: 0eca ScriptFunctionCall        params: 0 spawnstruct
         .00000010: 0b16 GetZero
         .00000012: 0eef GetGlobalObject           classes
         .00000016: 05c0 EvalFieldVariableRef      cct_shared_warning
         .0000001c: 04ce EvalArrayRef
         .0000001e: 0c18 SetVariableField
        */

        if (ccp->m_id != OPCODE_CheckClearParams) {
            // dctxt.WritePadding(out) << "Bad vtable opcode: expected CheckClearParams\n";
            return DVA_NOT;
        }

        const OPCodeInfo* preScriptCall = FetchCode();


        if (preScriptCall->m_id != OPCODE_PreScriptCall) {
            // dctxt.WritePadding(out) << "Bad vtable opcode: expected PreScriptCall\n";
            return DVA_NOT;
        }

        const OPCodeInfo* spawnStruct = FetchCode();

        if (spawnStruct->m_id != OPCODE_ScriptFunctionCall && spawnStruct->m_id != OPCODE_CallBuiltinFunction) {
            if (ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_CRC_DUMP)) {
                return DVA_OK; // crc dump
            }
            // dctxt.WritePadding(out) << "Bad vtable opcode, expected ScriptFunctionCall vm" << std::hex << (int)ctx.m_vm << std::endl;
            return DVA_NOT;
        }

        ctx.m_bcl += 1; // call params
        ctx.Aligned<uint64_t>() += 8; // assume that we have a spawnstruct

        auto AssertOpCode = [&FetchCode, &dctxt, &out](OPCode opc) -> bool {
            const OPCodeInfo* nfo{ FetchCode() };
            if (nfo->m_id != opc) {
                dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << nfo->m_name << ", expected " << OpCodeName(opc) << std::endl;
                out << "}" << std::endl;
                return false;
            }
            return true;
            };

        if (!AssertOpCode(OPCODE_GetZero)) return DVA_BAD;

        if (gscFile.GetMagic() > VMI_T8_36) {
            if (!AssertOpCode(OPCODE_T9_EvalFieldVariableFromGlobalObject)) return DVA_BAD;
            ctx.Aligned<uint16_t>() += 2; // - classes
        }
        else if (gscFile.GetMagic() < VMI_T8_34) {
            ctx.Aligned<uint16_t>() += 2; // GetClassesObject

            ctx.Aligned<uint16_t>() += 2; // EvalFieldVariableRef className
        }
        else {
            ctx.Aligned<uint16_t>() += 2; // GetGlobalObject
            ctx.Aligned<uint16_t>() += 2; // - classes

            ctx.Aligned<uint16_t>() += 2; // EvalFieldVariableRef
        }

        auto& clsName = ctx.Aligned<uint32_t>();

        uint32_t name = *(uint32_t*)clsName; // __vtable

        auto& cls = objctx.m_classes[name];
        cls.name_space = exp.GetNamespace();

        clsName += 4;
        if (ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START)) {
            out << std::endl;
        }
        else {
            out << " ";
        }
        out << "{" << std::endl;

        dctxt.WritePadding(out) << "// " << hashutils::ExtractTmp("class", name) << std::endl;

        if (gscFile.GetMagic() > VMI_T8_36) {
            if (!AssertOpCode(OPCODE_T9_SetVariableFieldFromEvalArrayRef)) return DVA_BAD;
        }
        else {
            ctx.Aligned<uint16_t>() += 2; // EvalArrayRef
            if (!AssertOpCode(OPCODE_SetVariableField)) return DVA_BAD;
        }


        while (true) {
            const OPCodeInfo* funcOpCode = FetchCode();

            if (!funcOpCode) {
                dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << funcOpCode->m_name << std::endl;
                out << "}" << std::endl;
                return DVA_BAD;
            }
            if (funcOpCode->m_id == OPCODE_End) {
                break; // end
            }
            if (funcOpCode->m_id != OPCODE_GetResolveFunction) {
                dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << funcOpCode->m_name << ", excepted GetResolveFunction or End" << std::endl;
                out << "}" << std::endl;
                return DVA_BAD;
            }

            auto& getFuncBase = ctx.Aligned<uint64_t>();
            uint32_t methodName = ((uint32_t*)getFuncBase)[0];
            uint32_t methodClsName = ((uint32_t*)getFuncBase)[1];
            getFuncBase += 8;

            const OPCodeInfo* uidCodeOpCode = FetchCode();

            if (!uidCodeOpCode) {
                dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << uidCodeOpCode->m_name << ", excepted Getter" << std::endl;
                out << "}" << std::endl;
                return DVA_BAD;
            }

            uint64_t uid;

            switch (uidCodeOpCode->m_id) {
            case OPCODE_GetZero: // int32_t
                uid = 0;
                break;
            case OPCODE_GetNegUnsignedInteger: // int32_t
                uid = -*(int32_t*)ctx.Aligned<int32_t>();
                ctx.m_bcl += 4;
                break;
            case OPCODE_GetNegUnsignedShort: // uint16_t
                uid = -*(uint16_t*)ctx.Aligned<uint16_t>();
                ctx.m_bcl += 2;
                break;
            case OPCODE_GetNegByte: // uint8_t
                uid = -*(uint8_t*)ctx.Aligned<uint8_t>();
                ctx.m_bcl++;
                break;
            case OPCODE_GetByte: // byte
                uid = *(byte*)ctx.Aligned<byte>();
                ctx.m_bcl++;
                break;
            case OPCODE_GetInteger: // int32_t
                uid = *(int32_t*)ctx.Aligned<int32_t>();
                ctx.m_bcl += 4;
                break;
            case OPCODE_GetLongInteger: // int64_t
                uid = *(int64_t*)ctx.Aligned<int64_t>();
                ctx.m_bcl += 8;
                break;
            case OPCODE_GetUnsignedInteger: // uint32_t
                uid = *(uint32_t*)ctx.Aligned<uint32_t>();
                ctx.m_bcl += 4;
                break;
            case OPCODE_GetUnsignedShort: // uint16_t
                uid = *(uint16_t*)ctx.Aligned<uint16_t>();
                ctx.m_bcl += 2;
                break;
            default:
                dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << uidCodeOpCode->m_name << ", excepted Getter" << std::endl;
                out << "}" << std::endl;
                return DVA_BAD;
            }

            if (methodClsName == name) {
                cls.m_methods.push_back(methodName);
            }
            else {
                cls.m_superClass.emplace(methodClsName);
            }
            cls.m_vtableMethods.emplace(NameLocated{ methodClsName, methodName });
            auto& mtd = cls.m_vtable[uid];
            mtd.name = methodName;
            mtd.nsp = methodClsName;
            dctxt.WritePadding(out) << "0x" << std::hex << std::setfill('0') << std::setw(sizeof(uid)) << uid
                << " -> &" << hashutils::ExtractTmp("class", methodClsName) << std::flush
                << "::" << hashutils::ExtractTmp("function", methodName) << ";" << std::endl;

            if (!AssertOpCode(OPCODE_GetZero)) return DVA_BAD;

            if (gscFile.GetMagic() >= VMI_T8_34) {
                ctx.Aligned<uint16_t>() += 2; // EvalGlobalObjectFieldVariable
                ctx.Aligned<uint16_t>() += 2; // - gvar
            }
            else {
                ctx.Aligned<uint16_t>() += 2; // GetClassesObject
                ctx.Aligned<uint16_t>() += 2; // EvalFieldVariable
            }
            ctx.Aligned<uint32_t>() += 4; // - ref
            ctx.Aligned<uint16_t>() += 2; // EvalArray
            ctx.Aligned<uint16_t>() += 2; // CastFieldObject
            ctx.Aligned<uint16_t>() += 2; // EvalFieldVariableRef
            ctx.Aligned<uint32_t>() += 4; // - ref

            if (gscFile.GetMagic() > VMI_T8_36) {
                ctx.Aligned<uint16_t>() += 2; // SetVariableFieldFromEvalArrayRef
            }
            else {
                ctx.Aligned<uint16_t>() += 2; // EvalArrayRef
                ctx.Aligned<uint16_t>() += 2; // SetVariableField
            }
        }
        /*
        * Field
        .000002f4: 086c GetResolveFunction        &cct_shared_warning::__constructor
        .00000300: 0a9a GetInteger                674154906
        .00000308: 056d GetZero
        .0000030a: 07f7 EvalGlobalObjectFieldVariable classes.cct_shared_warning
        .00000314: 09e4 EvalArray
        .00000316: 05e3 CastFieldObject
        .00000318: 0af5 EvalFieldVariableRef      __vtable
        .00000320: 0887 EvalArrayRef
        .00000322: 00f3 SetVariableField
        * End
        End
        */
        dctxt.WritePadding(out) << "// class " << hashutils::ExtractTmp("class", name) << std::flush;
        if (cls.m_superClass.size()) {
            out << " : ";
            for (auto it = cls.m_superClass.begin(); it != cls.m_superClass.end(); it++) {
                if (it != cls.m_superClass.begin()) {
                    out << ", ";
                }
                out << hashutils::ExtractTmp("class", *it) << std::flush;
            }
        }
        out << std::endl;
        out << "}" << std::endl;
        return DVA_OK;
    }

    void DumpFunctionHeader(GSCExportReader& exp, std::ostream& asmout, GSCOBJHandler& gscFile, T8GSCOBJContext& objctx, ASMContext& ctx, int padding, const char* forceName, const char** currentAnimTree) {
        auto remapedFlags = gscFile.RemapFlagsExport(exp.GetFlags());
        bool classMember = remapedFlags & (T8GSCExportFlags::CLASS_MEMBER | T8GSCExportFlags::CLASS_DESTRUCTOR);

        auto detourVal = objctx.m_gsicInfo.detours.find(exp.GetAddress());
        bool isDetour = detourVal != objctx.m_gsicInfo.detours.end();

        tool::gsc::formatter::FormatterFlags headerFormat{ ctx.m_opt.m_formatter->GetHeaderFormat() };

        if (ctx.useAnimTree && (objctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_ANIM_REAL))) {
            if (!currentAnimTree || !*currentAnimTree || std::strcmp(*currentAnimTree, ctx.useAnimTree)) {
                if (currentAnimTree) *currentAnimTree = ctx.useAnimTree; // new name
                // write animtree

                utils::Padding(asmout, padding) << "#using_animtree(";

                if (objctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS)) {
                    asmout << " ";
                }

                asmout << '"' << utils::FormattedString(ctx.useAnimTree) << '"';

                if (objctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS)) {
                    asmout << " ";
                }

                asmout << ");" << std::endl << std::endl;
            }
        }

        if (ctx.m_opt.m_func_header && headerFormat != tool::gsc::formatter::FFL_FUNC_HEADER_FORMAT_NONE) {
            const char* prefix;
            if (ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_ONE_LINE_HEADER_COMMENTS)) {
                utils::Padding(asmout, padding) << "/*" << std::endl;
                padding++;
                prefix = "";
            }
            else {
                prefix = "// ";
            }

            switch (headerFormat) {
            case tool::gsc::formatter::FFL_FUNC_HEADER_FORMAT_SERIOUS: {
                utils::Padding(asmout, padding) << prefix << "Name: " << hashutils::ExtractTmp("function", exp.GetName()) << std::endl;
                if (exp.GetNamespace()) {
                    utils::Padding(asmout, padding) << prefix << "Namespace: " << hashutils::ExtractTmp(classMember ? "class" : "namespace", exp.GetNamespace()) << std::endl;
                }
                // no file namespace in this format, maybe later?
                if (!ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_EXPORT_NOCHECKSUM)) {
                    utils::Padding(asmout, padding) << prefix << "Checksum: 0x" << std::hex << std::uppercase << exp.GetChecksum() << std::endl;
                }
                if (!objctx.m_vmInfo->HasFlag(VmFlags::VMF_GSCBIN)) {
                    utils::Padding(asmout, padding) << prefix << "Offset: 0x" << std::hex << std::uppercase << exp.GetAddress() << std::endl;
                }

                uint32_t knownSize{ exp.GetSize() };
                if (knownSize) {
                    utils::Padding(asmout, padding) << prefix << std::hex << "Size: 0x" << std::hex << knownSize << std::endl;
                }
                else {
                    UINT size = ctx.FinalSize();
                    if (size > 1) { // at least one opcode
                        utils::Padding(asmout, padding) << prefix << std::hex << "Size: 0x" << std::hex << size << std::endl;
                    }
                }


                utils::Padding(asmout, padding) << prefix << "Parameters: " << std::dec << (int)exp.GetParamCount() << std::endl;

                utils::Padding(asmout, padding) << prefix << "Flags: ";

                if (!remapedFlags) {
                    asmout << "None";
                }
                else if (gscFile.IsVTableImportFlags(exp.GetFlags())) {
                    asmout << "VTable";
                }
                else {
                    const struct {
                        T8GSCExportFlags flag;
                        const char* name;
                    } knownFlags[]{
                        { T8GSCExportFlags::LINKED, "Linked" },
                        { T8GSCExportFlags::AUTOEXEC, "AutoExec" },
                        { T8GSCExportFlags::PRIVATE, "Private" },
                        { T8GSCExportFlags::EVENT, "Event" },
                        { T8GSCExportFlags::CLASS_LINKED, "Class Linked" },
                        { T8GSCExportFlags::VE, "Variadic" },
                    };
                    bool def{};
                    for (const auto& kf : knownFlags) {
                        if ((remapedFlags & kf.flag) != kf.flag) {
                            continue;
                        }
                        if (def) {
                            asmout << ", ";
                        }
                        else {
                            def = true;
                        }
                        asmout << kf.name;
                    }
                }
                asmout << std::nouppercase << std::endl;
                break;
            }
            case tool::gsc::formatter::FFL_FUNC_HEADER_FORMAT_NONE:
                break;
            case tool::gsc::formatter::FFL_FUNC_HEADER_FORMAT_TYPE_4:
                utils::Padding(asmout, padding) << prefix << "Header format type 4 not implemented!";
                break;
            default: { // ACTS DEFAULT FORMAT
                if (exp.GetNamespace()) {
                    utils::Padding(asmout, padding) << prefix << "Namespace "
                        << hashutils::ExtractTmp(classMember ? "class" : "namespace", exp.GetNamespace()) << std::flush;

                    uint64_t fileNamespace = exp.GetFileNamespace();

                    if (fileNamespace && !objctx.m_vmInfo->HasFlag(VmFlags::VMF_NO_FILE_NAMESPACE)) {
                        // some VMs are only using the filename in the second namespace field, the others are using the full name (without .gsc?)
                        // so it's better to use spaces. A flag was added to keep the same format.
                        if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_FULL_FILE_NAMESPACE)) {
                            asmout << " / ";
                        }
                        else {
                            asmout << "/";
                        }

                        asmout
                            << ((remapedFlags & T8GSCExportFlags::EVENT)
                                ? hashutils::ExtractTmp("event", fileNamespace)
                                : hashutils::ExtractTmpPath("namespace", fileNamespace));
                    }
                    asmout << std::endl;
                }

                if (isDetour) {
                    auto det = detourVal->second;
                    utils::Padding(asmout, padding) << prefix
                        << "Detour " << hashutils::ExtractTmp("function", exp.GetName()) << " "
                        << "Offset 0x" << std::hex << det.fixupOffset << "/0x" << det.fixupSize
                        << std::endl
                        ;
                }

                utils::Padding(asmout, padding) << prefix << "Params " << std::dec << (int)exp.GetParamCount();

                if (remapedFlags) {
                    asmout << ", eflags: 0x" << std::hex << (int)exp.GetFlags();

                    if (remapedFlags & T8GSCExportFlags::LINKED) {
                        asmout << " linked";
                    }
                    if (remapedFlags & T8GSCExportFlags::CLASS_LINKED) {
                        asmout << " class_linked";
                    }
                    if (remapedFlags & T8GSCExportFlags::VE) {
                        asmout << " variadic";
                    }
                    if (gscFile.IsVTableImportFlags(exp.GetFlags())) {
                        asmout << " vtable";
                    }
                }

                asmout << std::endl;
                if (ctx.m_opt.m_rawhash) {
                    utils::Padding(asmout, padding) << prefix
                        << std::hex
                        << "namespace_" << exp.GetNamespace() << "<file_" << exp.GetFileNamespace() << ">::function_" << exp.GetName() << std::endl;
                }

                if (!objctx.m_vmInfo->HasFlag(VmFlags::VMF_GSCBIN)) {
                    utils::Padding(asmout, padding) << prefix;
                    if (!ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_EXPORT_NOCHECKSUM)) {
                        asmout << "Checksum 0x" << std::hex << exp.GetChecksum() << ", ";
                    }
                    asmout << "Offset: 0x" << std::hex << exp.GetAddress() << std::endl;
                }

                uint32_t knownSize{ exp.GetSize() };
                bool hasSize{ (bool)knownSize };
                if (knownSize) {
                    utils::Padding(asmout, padding) << prefix << std::hex << "Size: 0x" << std::hex << knownSize;
                }
                else {
                    auto size = ctx.FinalSize();
                    if (size) { // at least one opcode
                        utils::Padding(asmout, padding) << prefix << std::hex << "Size: 0x" << size;
                        hasSize = true;
                    }
                }

                if (ctx.m_devFuncCandidate || ctx.m_boolFuncCandidate) {

                    if (hasSize) {
                        asmout << "," << " Type:";
                    }
                    else {
                        utils::Padding(asmout, padding) << prefix << "Type:";
                    }

                    if (ctx.m_devFuncCandidate) {
                        asmout << " dev";
                    }
                    if (ctx.m_boolFuncCandidate) {
                        asmout << " bool";
                    }
                    asmout << std::nouppercase;
                }
                if (hasSize) {
                    asmout << std::endl;
                }
                break;
            }
            }
            if (ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_ONE_LINE_HEADER_COMMENTS)) {
                padding--;
                utils::Padding(asmout, padding) << "*/" << std::endl;
            }
        }

        bool specialClassMember = !ctx.m_opt.m_dasm && classMember &&
            ((remapedFlags & T8GSCExportFlags::CLASS_DESTRUCTOR) || ctx.m_objctx.m_vmInfo->HashField("__constructor") == exp.GetName());

        utils::Padding(asmout, padding);

        if (!specialClassMember && !ctx.noFunctionPrefix && !(ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NO_FUNCTION_TITLE))) {
            asmout << "function ";
        }
        if (remapedFlags & T8GSCExportFlags::PRIVATE) {
            asmout << "private ";
        }
        if (remapedFlags & T8GSCExportFlags::AUTOEXEC) {
            asmout << "autoexec ";
        }
        if (remapedFlags & T8GSCExportFlags::EVENT) {
            asmout << "event_handler[" << hashutils::ExtractTmp("event", exp.GetFileNamespace()) << "] " << std::flush;
        }

        if (ctx.m_opt.m_dasm && (classMember || (remapedFlags & T8GSCExportFlags::CLASS_DESTRUCTOR))) {
            asmout << hashutils::ExtractTmp("class", exp.GetNamespace())
                << std::flush << "::";

            if (exp.GetFlags() & T8GSCExportFlags::CLASS_DESTRUCTOR) {
                asmout << "~";
            }
        }

        if (isDetour) {
            auto& detour = detourVal->second;

            asmout << "detour ";
            if (detour.replaceNamespace) {
                asmout << hashutils::ExtractTmpPath("namespace", detour.replaceNamespace) << std::flush;
            }

            if (detour.replaceScript) {
                asmout << "<" << hashutils::ExtractTmpScript(detour.replaceScript) << ">" << std::flush;
            }

            if (detour.replaceNamespace) {
                asmout << "::";
            }

            asmout
                << hashutils::ExtractTmp("function", detour.replaceFunction) << std::flush;
        }
        else {
            asmout << (forceName ? forceName : hashutils::ExtractTmp("function", exp.GetName()));
        }


        asmout << std::flush << "(";

        // local var size = <empty>, <params>, <localvars> so we need to check that we have at least param_count + 1
        if (ctx.m_localvars.size() > exp.GetParamCount()) {
            if (exp.GetParamCount() && (ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS))) {
                asmout << " ";
            }

            for (size_t i = 0; i < exp.GetParamCount(); i++) {
                // -1 to avoid the <empty> object, -1 because we are in reverse order
                const auto& lvar = ctx.m_localvars[ctx.m_localvars.size() - i - 2];

                if ((lvar.flags & T8GSCLocalVarFlag::IW_VARIADIC_COUNT) && !lvar.defaultValueNode) {
                    continue; // ignore if the varargcount is used
                }
                if (i) {
                    asmout << ", ";
                }

                if (lvar.flags & T8GSCLocalVarFlag::VARIADIC) {
                    asmout << "...";
                }
                else {
                    if (lvar.flags & T8GSCLocalVarFlag::ARRAY_REF) {
                        asmout << "&";
                    }
                    else if (gscFile.GetMagic() != VMI_T8_36 && (lvar.flags & T8GSCLocalVarFlag::T9_VAR_REF)) {
                        asmout << "*";
                    }

                    asmout << hashutils::ExtractTmp("var", lvar.name) << std::flush;
                }

                byte mask = ~(T8GSCLocalVarFlag::VARIADIC | T8GSCLocalVarFlag::ARRAY_REF);

                if (ctx.m_vm != VMI_T8_36) {
                    mask &= ~T8GSCLocalVarFlag::T9_VAR_REF;
                }

                if (lvar.flags & mask) {
                    asmout << " (unk flags: " << std::hex << (int)lvar.flags << ")";
                }
                if (lvar.defaultValueNode) {
                    asmout << " = ";
                    DecompContext dctx = { 0, 0, ctx.m_opt, 0 };
                    lvar.defaultValueNode->Dump(asmout, dctx);
                }
            }

            if (exp.GetParamCount() && (ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS))) {
                asmout << " ";
            }
        }
        asmout << ")";
    }


    GSCOBJHandler::GSCOBJHandler(byte* file, uint64_t fileSize, size_t buildFlags) : file(file), buildFlags(buildFlags), fileSize(fileSize) {}

    void GSCOBJHandler::SetFile(byte* file, size_t fileSize) {
        this->file = file;
        this->fileSize = fileSize;
    }

    // by default no remapping
    byte GSCOBJHandler::RemapFlagsImport(byte flags) {
        return flags;
    }
    byte GSCOBJHandler::RemapFlagsExport(byte flags) {
        return flags;
    }

    byte GSCOBJHandler::MapFlagsImportToInt(byte flags) {
        return flags;
    }
    byte GSCOBJHandler::MapFlagsExportToInt(byte flags) {
        return flags;
    }

    void GSCOBJHandler::SetNameString(uint32_t name) {
        throw std::runtime_error("can't set string name for this vm");
    }
    void GSCOBJHandler::DumpHeaderInternal(std::ostream& asmout, const GscInfoOption& opt) {};
    void GSCOBJHandler::DumpHeader(std::ostream& asmout, const GscInfoOption& opt) {
        uint32_t checksum{ GetChecksum() };

        if (checksum) {
            asmout << "// crc: 0x" << std::hex << checksum << " (" << std::dec << checksum << ")" << std::endl;
        }

        asmout
            << std::left << std::setfill(' ')
            << "// size ...... " << std::dec << std::setw(3) << GetFileSize() << " (0x" << std::hex << GetFileSize() << ")" << std::endl;

        uint32_t includesOffset{ GetIncludesOffset() };
        uint32_t includesCount{ GetIncludesCount() };
        if (includesOffset) {
            asmout << "// includes .. " << std::dec << std::setw(3) << includesCount << " (offset: 0x" << std::hex << includesOffset << ")" << std::endl;
        }

        uint32_t stringsOffset{ GetStringsOffset() };
        uint32_t stringsCount{ GetStringsCount() };
        if (stringsOffset) {
            asmout << "// strings ... " << std::dec << std::setw(3) << stringsCount << " (offset: 0x" << std::hex << stringsOffset << ")" << std::endl;
        }

        uint32_t devStringsOffset{ GetDevStringsOffset() };
        uint32_t devStringsCount{ GetDevStringsCount() };
        if (devStringsOffset) {
            asmout << "// dev strs .. " << std::dec << std::setw(3) << devStringsCount << " (offset: 0x" << std::hex << devStringsOffset << ")" << std::endl;
        }

        uint32_t exportsOffset{ GetExportsOffset() };
        uint32_t exportsCount{ GetExportsCount() };
        if (exportsOffset) {
            asmout << "// exports ... " << std::dec << std::setw(3) << exportsCount << " (offset: 0x" << std::hex << exportsOffset << ")" << std::endl;
        }

        uint32_t csegOffset{ GetCSEGOffset() };
        uint32_t csegSize{ GetCSEGSize() };
        if (csegOffset + csegSize) {
            asmout << "// cseg ...... 0x" << std::hex << csegOffset << " + 0x" << std::hex << csegSize << " (0x" << (csegOffset + csegSize) << ")" << std::endl;
        }

        uint32_t importsOffset{ GetImportsOffset() };
        uint32_t importsCount{ GetImportsCount() };
        if (importsOffset) {
            asmout << "// imports ... " << std::dec << std::setw(3) << importsCount << " (offset: 0x" << std::hex << importsOffset << ")" << std::endl;
        }

        uint32_t animSingleOffset{ GetAnimTreeSingleOffset() };
        uint32_t animSingleCount{ GetAnimTreeSingleCount() };
        if (animSingleOffset) {
            asmout << "// animtree1 . " << std::dec << std::setw(3) << animSingleCount << " (offset: 0x" << std::hex << animSingleOffset << ")" << std::endl;
        }

        uint32_t animDoubleOffset{ GetAnimTreeDoubleOffset() };
        uint32_t animDoubleCount{ GetAnimTreeDoubleCount() };
        if (animDoubleOffset) {
            asmout << "// animtree2 . " << std::dec << std::setw(3) << animDoubleCount << " (offset: 0x" << std::hex << animDoubleOffset << ")" << std::endl;
        }

        uint32_t globalsOffset{ GetGVarsOffset() };
        uint32_t globalsCount{ GetGVarsCount() };
        if (globalsOffset) {
            asmout << "// globals .. " << std::dec << std::setw(3) << globalsCount << " (offset: 0x" << std::hex << globalsOffset << ")" << std::endl;
        }

        DumpHeaderInternal(asmout, opt);
        asmout << std::right << std::flush;
    }

    void GSCOBJHandler::SwitchHeaderEndian() {
        throw std::runtime_error("SwitchHeaderEndian not implemented for this vm");
    }

    uint16_t GSCOBJHandler::GetTokensCount() {
        return 0;
    }
    uint32_t GSCOBJHandler::GetTokensOffset() {
        return 0;
    }

    int GSCOBJHandler::PreLoadCode(T8GSCOBJContext& ctx, std::ostream& asmout) {
        return tool::OK;
    }

    opcode::Platform GSCOBJHandler::ComputePlatform(T8GSCOBJContext& ctx) {
        return PLATFORM_UNKNOWN; // can't compute platform for that
    }

    std::pair<const char*, size_t> GSCOBJHandler::GetStringHeader(size_t len) {
        return { "", 0 }; // no encryption header by default
    }
    int GSCOBJHandler::PatchCode(T8GSCOBJContext& ctx) {
        size_t opcodeSize = ctx.m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16) ? 2 : 1;
        if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_IW_LIKE)) {
            if (GetAnimTreeSingleOffset()) {
                // HAS TO BE DONE FIRST BECAUSE THEY ARE STORED USING 1 byte
                uintptr_t unk2c_location = reinterpret_cast<uintptr_t>(file) + GetAnimTreeSingleOffset();
                auto anims_count = (int)GetAnimTreeSingleCount();
                for (size_t i = 0; i < anims_count; i++) {
                    const auto* unk2c = reinterpret_cast<GSC_USEANIMTREE_ITEM*>(unk2c_location);

                    char* s = Ptr<char>(unk2c->address);

                    if (!ctx.opt.m_noStrDecrypt) {
                        s = DecryptString(s);
                    }

                    if (ctx.gdctx.dumpStringsStore) {
                        ctx.gdctx.dumpStringsStore->insert(s);
                    }
                    if (ctx.gdctx.stringsLoc) {
                        ctx.gdctx.stringsLoc->push_back(s);
                    }
                    uint32_t ref = ctx.AddStringValue(s);
                    const auto* vars = reinterpret_cast<const uint32_t*>(&unk2c[1]);

                    if (ref > 256) {
                        LOG_ERROR("Too many animtrees single usage");
                        return tool::BASIC_ERROR;
                    }
                    else {
                        for (size_t j = 0; j < unk2c->num_address; j++) {
                            Ref(vars[j]) = (byte)ref;
                            ctx.AddStringRef(vars[j], ref);
                        }
                    }
                    unk2c_location += sizeof(*unk2c) + sizeof(*vars) * unk2c->num_address;
                }
            }

            if (GetDevStringsOffset()) {
                T8GSCString* val = Ptr<T8GSCString>(GetDevStringsOffset());
                for (size_t i = 0; i < GetDevStringsCount(); i++) {
                    const char* str;
                    if (val->string) {
                        if (ctx.gdbData) {
                            // add this location as a dev string
                            ctx.gdbData->devStringsLocation.insert(val->string);
                        }
                        if (ctx.dbgData && ctx.dbgSize) {
                            if (val->string >= ctx.dbgSize) {
                                LOG_ERROR("Invalid dev string: location outside of debug file: 0x{:x} >= 0x{:x}", val->string, ctx.dbgSize);
                                str = ctx.CloneString(utils::va("<dev string:x%x>", val->string));
                            }
                            else {
                                str = (const char*)&ctx.dbgData[val->string];
                            }
                        }
                        else {
                            // no gdb
                            if (ctx.m_formatter && ctx.m_formatter->HasFlag(tool::gsc::formatter::FFL_NOERROR_STR)) {
                                break; // nothing
                            }
                            str = ctx.CloneString(utils::va("<dev string:x%x>", val->string));
                        }
                    }
                    else {
                        str = "<dev string>";
                    }

                    uint32_t* loc = reinterpret_cast<uint32_t*>(val + 1);
                    for (size_t j = 0; j < val->num_address; j++) {

                        if (ctx.gdbctx) {
                            // use gdb string
                            auto it = ctx.gdbctx->strings.find(loc[j]);
                            if (it != ctx.gdbctx->strings.end()) {
                                std::string& str = it->second;

                                uint32_t strref = ctx.AddStringValue(str.c_str());
                                Ref<uint32_t>(loc[j]) = strref;
                                ctx.AddStringRef(loc[j], strref);
                                continue;
                            }
                        }
                        ctx.m_unkstrings[str].insert(loc[j]);
                        uint32_t strref = ctx.AddStringValue(str);
                        Ref<uint32_t>(loc[j]) = strref;
                        ctx.AddStringRef(loc[j], strref);
                    }
                    val = reinterpret_cast<T8GSCString*>(loc + val->num_address);
                }
            }

            uintptr_t str_location = reinterpret_cast<uintptr_t>(file) + GetStringsOffset();
            int string_count = (int)GetStringsCount();
            for (size_t i = 0; i < string_count; i++) {

                const auto* str = reinterpret_cast<T8GSCString*>(str_location);
                const char* rcstr;
                if (str->string) {
                    char* cstr = Ptr<char>(str->string);

                    if (!ctx.opt.m_noStrDecrypt) {
                        cstr = DecryptString(cstr);
                    }
                    rcstr = cstr;
                }
                else {
                    rcstr = "<invalid>";
                }
                if (ctx.gdctx.stringsLoc) {
                    ctx.gdctx.stringsLoc->push_back(rcstr);
                }

                if (ctx.gdctx.dumpStringsStore) {
                    ctx.gdctx.dumpStringsStore->insert(rcstr);
                }
                uint32_t ref = ctx.AddStringValue(rcstr);

                const auto* strings = reinterpret_cast<const uint32_t*>(&str[1]);
                for (size_t j = 0; j < str->num_address; j++) {
                    if (strings[j] >= fileSize) {
                        LOG_ERROR("Invalid string ref 0x{:x} for {}", strings[j], rcstr);
                        return tool::BASIC_ERROR;
                    }
                    Ref<uint32_t>(strings[j]) = ref;
                    ctx.AddStringRef(strings[j], ref);
                }
                str_location += sizeof(*str) + sizeof(*strings) * str->num_address;
            }

            auto imports_count = (int)GetImportsCount();
            uintptr_t import_location = reinterpret_cast<uintptr_t>(file) + GetImportsOffset();
            for (size_t i = 0; i < imports_count; i++) {

                const auto* imp = reinterpret_cast<IW23GSCImport*>(import_location);

                uint8_t mappedImportFlags = RemapFlagsImport(imp->flags);

                size_t delta;
                if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_IW_CALLS)) {
                    switch (mappedImportFlags & 0xF) {
                    case FUNCTION_THREAD:
                    case FUNCTION_CHILDTHREAD:
                    case ACTS_CALL_BUILTIN_FUNCTION:
                    case METHOD_THREAD:
                    case METHOD_CHILDTHREAD:
                    case ACTS_CALL_BUILTIN_METHOD:
                        delta = 1;
                        break;
                    default:
                        delta = 0;
                        break;
                    }
                }
                else {
                    delta = 0;
                }

                const auto* imports = reinterpret_cast<const uint32_t*>(&imp[1]);
                for (size_t j = 0; j < imp->num_address; j++) {
                    if (delta) {
                        Ref<byte>(imports[j]) = imp->param_count;
                    }
                    uint16_t* loc = Ptr<uint16_t>(imports[j] + delta);
                    auto idx = ctx.m_linkedImports.size();
                    ctx.m_linkedImports.push_back(*imp);
                    *loc = (uint16_t)idx;
                }
                import_location += sizeof(*imp) + sizeof(*imports) * imp->num_address;
            }

            if (GetAnimTreeDoubleOffset()) {
                uintptr_t animt_location = reinterpret_cast<uintptr_t>(file) + GetAnimTreeDoubleOffset();
                auto anims_count = (int)GetAnimTreeDoubleCount();
                for (size_t i = 0; i < anims_count; i++) {
                    const auto* animt = reinterpret_cast<GSC_ANIMTREE_ITEM*>(animt_location);

                    auto* s1 = Ptr<char>(animt->address_str1);
                    auto* s2 = Ptr<char>(animt->address_str2);

                    if (!ctx.opt.m_noStrDecrypt) {
                        s1 = DecryptString(s1);
                        s2 = DecryptString(s2);
                    }
                    if (ctx.gdctx.stringsLoc) {
                        ctx.gdctx.stringsLoc->push_back(s1);
                        ctx.gdctx.stringsLoc->push_back(s2);
                    }
                    if (ctx.gdctx.dumpStringsStore) {
                        ctx.gdctx.dumpStringsStore->insert(s1);
                        ctx.gdctx.dumpStringsStore->insert(s2);
                    }

                    uint32_t ref1 = ctx.AddStringValue(s1);
                    uint32_t ref2 = ctx.AddStringValue(s2);

                    const auto* vars = reinterpret_cast<const uint32_t*>(&animt[1]);
                    for (size_t j = 0; j < animt->num_address; j++) {
                        auto* loc = Ptr<uint32_t>(vars[j]);
                        // use strings to link them
                        loc[0] = ref1;
                        loc[1] = ref2;

                        ctx.AddStringRef(vars[j], ref1);
                        ctx.AddStringRef(vars[j] + sizeof(*loc), ref2);
                    }
                    animt_location += sizeof(*animt) + sizeof(*vars) * animt->num_address;
                }
            }

            if (GetTokensOffset()) {
                auto tokens_count = GetTokensCount();
                GSCBINToken* tokens{ Ptr<GSCBINToken>(GetTokensOffset()) };
                for (size_t i = 0; i < tokens_count; i++) {
                    GSCOBJTokenData& tdt{ ctx.m_tokens.emplace_back() };
                    tdt.loc = tokens[i].location;
                    if (tokens[i].type == GBTT_STRING) {
                        tdt.isString = true;
                        tdt.val.str = Ptr<const char>(tokens[i].val);
                    }
                    else {
                        tdt.isString = false;
                        tdt.val.id = tokens[i].val;
                    }
                }
            }

            return tool::OK;// mwiii
        }
        // patching imports unlink the script refs to write namespace::import_name instead of the address
        auto imports_count = (int)GetImportsCount();
        uint32_t import_offset = GetImportsOffset();
        if (import_offset > fileSize) {
            LOG_ERROR("Invalid import table 0x{:x} ", import_offset);
            return tool::BASIC_ERROR;
        }
        uintptr_t import_location = reinterpret_cast<uintptr_t>(file) + import_offset;
        for (size_t i = 0; i < imports_count; i++) {
            const auto* imp = reinterpret_cast<T8GSCImport*>(import_location);
            if (import_location - reinterpret_cast<uintptr_t>(file) + sizeof(uint32_t) * imp->num_address > fileSize) {
                LOG_ERROR("Invalid import 0x{:x} with {} addresses", import_location - reinterpret_cast<uintptr_t>(file), imp->num_address);
                return tool::BASIC_ERROR;
            }

            const auto* imports = reinterpret_cast<const uint32_t*>(&imp[1]);
            for (size_t j = 0; j < imp->num_address; j++) {
                uint32_t* loc;
                auto remapedFlags = RemapFlagsImport(imp->flags);

                if (imports[j] > fileSize) {
                    LOG_ERROR("Invalid import {}::{} address 0x{:x} > 0x{:x} for i{}#{}",
                        hashutils::ExtractTmp("namespace", imp->import_namespace), hashutils::ExtractTmp("function", imp->name),
                        imports[j], fileSize, i, j);
                    break;
                }

                switch (remapedFlags & CALLTYPE_MASK) {
                case FUNC_METHOD:
                    loc = PtrAlign<uint64_t, uint32_t>(imports[j] + opcodeSize);
                    break;
                case FUNCTION:
                case FUNCTION_THREAD:
                case FUNCTION_CHILDTHREAD:
                case METHOD:
                case METHOD_THREAD:
                case METHOD_CHILDTHREAD: {
                    // here the game fix function calls with a bad number of params,
                    // but for the decomp/dasm we don't care because we only mind about
                    // what we'll find on the stack.
                    if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_CALL_NO_PARAMS)) {
                        loc = PtrAlign<uint64_t, uint32_t>(imports[j] + opcodeSize);
                    }
                    else {
                        Ref<byte>(imports[j] + opcodeSize) = imp->param_count;
                        loc = PtrAlign<uint64_t, uint32_t>(imports[j] + opcodeSize + 1);
                    }

                }
                                       break;
                default:
                    loc = nullptr;
                    break;
                }
                if (loc) {
                    loc[0] = imp->name;

                    if (remapedFlags & T8GSCImportFlags::GET_CALL) {
                        // no need for namespace if we are getting the call dynamically (api or inside-code script)
                        loc[1] = 0xc1243180; // ""
                    }
                    else {
                        loc[1] = imp->import_namespace;
                    }

                }
            }
            import_location += sizeof(*imp) + sizeof(*imports) * imp->num_address;
        }

        uintptr_t gvars_location = reinterpret_cast<uintptr_t>(file) + GetGVarsOffset();
        auto globalvar_count = (int)GetGVarsCount();
        for (size_t i = 0; i < globalvar_count; i++) {
            const auto* globalvar = reinterpret_cast<T8GSCGlobalVar*>(gvars_location);
            uint16_t ref = ctx.AddGlobalVarName(globalvar->name);

            const auto* vars = reinterpret_cast<const uint32_t*>(&globalvar[1]);
            for (size_t j = 0; j < globalvar->num_address; j++) {
                // no align, no opcode to pass, directly the fucking location, cool.
                if (vars[j] >= GetFileSize() - sizeof(uint16_t)) {
                    LOG_ERROR("Invalid global variable: 0x{:x}", vars[j]);
                    return tool::BASIC_ERROR;
                }
                else {
                    Ref<uint16_t>(vars[j]) = ref;
                }
            }
            gvars_location += sizeof(*globalvar) + sizeof(*vars) * globalvar->num_address;
        }

        if (GetDevStringsOffset() && !(ctx.m_formatter && ctx.m_formatter->HasFlag(tool::gsc::formatter::FFL_NOERROR_STR))) {
            T8GSCString* val = Ptr<T8GSCString>(GetDevStringsOffset());
            for (size_t i = 0; i < GetDevStringsCount(); i++) {
                const char* str;
                if (val->string) {
                    if (ctx.gdbData) {
                        // add this location as a dev string
                        ctx.gdbData->devStringsLocation.insert(val->string);
                    }
                    // the acts compiler uses empty strings location when they're not compiled in the gdb
                    if (ctx.dbgData && ctx.dbgSize) {
                        if (val->string >= ctx.dbgSize) {
                            LOG_ERROR("Invalid dev string: location outside of debug file: 0x{:x} >= 0x{:x}", val->string, ctx.dbgSize);
                            str = ctx.CloneString(utils::va("<dev string:x%x>", val->string));
                        }
                        else {
                            str = (const char*)&ctx.dbgData[val->string];
                        }
                    }
                    else {
                        // no gdb
                        if (ctx.m_formatter && ctx.m_formatter->HasFlag(tool::gsc::formatter::FFL_NOERROR_STR)) {
                            break; // nothing
                        }
                        str = ctx.CloneString(utils::va("<dev string:x%x>", val->string));
                    }
                }
                else {
                    str = "<dev string>";
                }

                uint32_t* loc = reinterpret_cast<uint32_t*>(val + 1);
                for (size_t j = 0; j < val->num_address; j++) {

                    if (loc[j] + sizeof(uint32_t) >= GetFileSize()) {
                        LOG_ERROR("Invalid devstring location: 0x{:x}", loc[j]);
                        return tool::BASIC_ERROR;
                    }

                    if (ctx.gdbctx) {
                        // use gdb string
                        auto it = ctx.gdbctx->strings.find(loc[j]);
                        if (it != ctx.gdbctx->strings.end()) {
                            std::string& str = it->second;

                            uint32_t strref = ctx.AddStringValue(str.c_str());
                            //Ref<uint32_t>(loc[j]) = strref;
                            ctx.AddStringRef(loc[j], strref);
                            continue;
                        }
                    }
                    ctx.m_unkstrings[str].insert(loc[j]);
                    uint32_t strref = ctx.AddStringValue(str);
                    //Ref<uint32_t>(loc[j]) = strref;
                    ctx.AddStringRef(loc[j], strref);
                }
                val = reinterpret_cast<T8GSCString*>(loc + val->num_address);
            }
        }

        uintptr_t str_location = reinterpret_cast<uintptr_t>(file) + GetStringsOffset();
        int string_count = (int)GetStringsCount();
        for (size_t i = 0; i < string_count; i++) {

            const auto* str = reinterpret_cast<T8GSCString*>(str_location);
            const char* rcstr;
            if (str->string) {
                char* cstr = Ptr<char>(str->string);
                if (!ctx.opt.m_noStrDecrypt) {
                    cstr = DecryptString(cstr);
                }
                rcstr = cstr;
            }
            else {
                rcstr = "<invalid>";
            }
            if (ctx.gdctx.stringsLoc) {
                ctx.gdctx.stringsLoc->push_back(rcstr);
            }
            if (ctx.gdctx.dumpStringsStore) {
                ctx.gdctx.dumpStringsStore->insert(rcstr);
            }
            uint32_t ref = ctx.AddStringValue(rcstr);

            const auto* strings = reinterpret_cast<const uint32_t*>(&str[1]);
            for (size_t j = 0; j < str->num_address; j++) {
                // no align too....
                if (strings[j] + sizeof(uint32_t) >= GetFileSize()) {
                    LOG_ERROR("Invalid string location: 0x{:x}", strings[j]);
                    break;
                }

                //Ref<uint32_t>(strings[j]) = ref;
                if (str->string || !ctx.GetStringValueByLoc(strings[j])) {
                    ctx.AddStringRef(strings[j], ref);
                }
            }
            str_location += sizeof(*str) + sizeof(*strings) * str->num_address;
        }

        if (GetAnimTreeDoubleOffset()) {
            uintptr_t animt_location = reinterpret_cast<uintptr_t>(file) + GetAnimTreeDoubleOffset();
            auto anims_count = (int)GetAnimTreeDoubleCount();
            for (size_t i = 0; i < anims_count; i++) {
                const auto* animt = reinterpret_cast<T7GscAnimTree*>(animt_location);

                auto* s1 = Ptr<char>(animt->name);

                uint32_t ref1 = ctx.AddStringValue(s1);
                if (ctx.gdctx.dumpStringsStore) {
                    ctx.gdctx.dumpStringsStore->insert(s1);
                }
                if (ctx.gdctx.stringsLoc) {
                    ctx.gdctx.stringsLoc->push_back(s1);
                }

                const uint32_t* vars = reinterpret_cast<const uint32_t*>(&animt[1]);
                for (size_t j = 0; j < animt->num_tree_address; j++) {
                    uint32_t rloc = *(vars++);
                    uint32_t* loc = Ptr<uint32_t>(rloc);
                    // use strings to link them
                    ctx.m_animTreeLocations[rloc] = ref1;
                    *loc = 666;
                }
                const uint64_t* vars2 = reinterpret_cast<const uint64_t*>(vars);
                for (size_t j = 0; j < animt->num_node_address; j++) {
                    if (vars2[0] < fileSize) {
                        const char* v = Ptr<const char>(vars2[0]);
                        uint32_t ref2 = ctx.AddStringValue(v);
                        if (ctx.gdctx.dumpStringsStore) {
                            ctx.gdctx.dumpStringsStore->insert(v);
                        }
                        if (ctx.gdctx.stringsLoc) {
                            ctx.gdctx.stringsLoc->push_back(v);
                        }

                        uint32_t rloc = (uint32_t)vars2[1];
                        uint32_t* loc = Ptr<uint32_t>(rloc);
                        loc[0] = ref1;
                        loc[1] = ref2;
                        ctx.AddStringRef(rloc, ref1);
                        ctx.AddStringRef(rloc + sizeof(*loc), ref2);

                    }
                    vars2 += 2;
                }
                animt_location += sizeof(*animt) + sizeof(*vars) * (animt->num_tree_address + (size_t)animt->num_node_address * 4);
            }
        }
        return tool::OK;
    }

    void tool::gsc::GSCOBJHandler::DumpExperimental(std::ostream& asmout, const GscInfoOption& opt, T8GSCOBJContext& ctx) {
    }

}