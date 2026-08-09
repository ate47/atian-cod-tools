#include <dll_includes.hpp>
#include <games/bo4/t8_errors.hpp>
#include <data/bo4.hpp>
#include <data/refs.hpp>
#include <core/config.hpp>
#include <core/system.hpp>
#include <core/eventhandler.hpp>
#include <hook/library.hpp>
#include <gsc/gsc_gdb.hpp>
#include <gsc/gsc_acts_addons.hpp>
#include <core/hashes/hash_store.hpp>
#include <systems/gsc_link.hpp>
#include <systems/gsc_op.hpp>
#include <systems/errors.hpp>
#include <systems/events.hpp>
#include <utils/data_utils.hpp>

namespace systems::gsc::link {
    constexpr bool traceLinking = false;

    using namespace shared::gsc;

    using acts_addons::GSC_ACTS_ADDONS;
    using acts_debug::GSC_ACTS_DEBUG;
    using acts_debug::GSC_ACTS_DETOUR;

    struct objGDBFileInfo_t {
        XHash name;
        bo4::GSC_OBJ* obj;
        GSC_ACTS_DEBUG* gdb;
        GSC_ACTS_ADDONS* addons;
    };

    namespace {
        bool enableDevBlocks{};
        bool traceDetours;
        objGDBFileInfo_t gObjGDBFileInfo[bo4::scriptInstance_t::SI_COUNT][650];
        int gObjGDBFileInfoCount[bo4::scriptInstance_t::SI_COUNT];
        size_t gObjCountLinked[bo4::scriptInstance_t::SI_COUNT];
    } // namespace

    inline byte* Scr_FindDetourArray(
        GSC_ACTS_DETOUR* detours, GSC_ACTS_DETOUR* end, bo4::GSC_OBJ* obj, byte* origin, uint64_t script, uint32_t nsp,
        uint32_t func
    ) {
        for (; detours != end; detours++) {
            if (detours->script != script || detours->name_space != nsp || detours->name != func) {
                continue; // not it
            }

            byte* funcPtr{ obj->magic + detours->location };
            byte* funcPtrEnd{ funcPtr + detours->size };

            if (funcPtr <= origin && funcPtrEnd > origin) {
                continue; // inside the detour
            }
            return funcPtr;
        }
        return nullptr;
    }

    byte* Scr_FindDetour(bo4::scriptInstance_t inst, byte* origin, uint64_t script, uint32_t nsp, uint32_t func) {
        for (size_t i = 0; i < gObjGDBFileInfoCount[inst]; i++) {
            objGDBFileInfo_t* odbg{ &gObjGDBFileInfo[inst][i] };
            if (!odbg->obj) {
                continue; // is this mandatory?
            }
            if (odbg->addons && odbg->addons->HasFeature(acts_addons::AAF_DETOUR)) {
                // addons
                byte* d{ Scr_FindDetourArray(
                    odbg->addons->GetDetours(odbg->obj->magic),
                    odbg->addons->GetDetoursEnd(odbg->obj->magic),
                    odbg->obj,
                    origin,
                    script,
                    nsp,
                    func
                ) };
                if (d) {
                    return d;
                }
            }
            if (odbg->gdb && odbg->gdb->HasFeature(acts_debug::ADF_DETOUR)) {
                // debug (backward compatibility with old scripts)
                byte* d{ Scr_FindDetourArray(
                    odbg->gdb->GetDetours(),
                    odbg->gdb->GetDetoursEnd(),
                    odbg->obj,
                    origin,
                    script,
                    nsp,
                    func
                ) };
                if (d) {
                    return d;
                }
            }
        }
        return nullptr;
    }

    namespace {
        hook::library::Detour Scr_GscLink_Detour;
        hook::library::Detour Scr_ResetLinkInfo_Detour;

        inline void
        Scr_CheckDetour(objGDBFileInfo_t* dbgobj, bo4::GSC_OBJ* obj, GSC_ACTS_DETOUR* detour, GSC_ACTS_DETOUR* end) {
            for (; detour != end; detour++) {
                uint64_t* usings{ (uint64_t*)(obj->magic + obj->include_offset) };
                uint64_t* usingsend{ &usings[obj->include_count] };

                if (!detour->script && detour->script != obj->name &&
                    std::find(usings, usingsend, detour->script) == usingsend) {
                    continue; // not included by this script
                }

                bo4::GSC_IMPORT_ITEM* next{ (bo4::GSC_IMPORT_ITEM*)&obj->magic[obj->imports_offset] };
                for (size_t i = 0; i < obj->imports_count; i++) {
                    bo4::GSC_IMPORT_ITEM* imp{ next };
                    int32_t* locs{ (int32_t*)&imp[1] };
                    next = (bo4::GSC_IMPORT_ITEM*)&locs[imp->num_address];
                    if (imp->name_space != detour->name_space || imp->name != detour->name) {
                        continue; // bad export
                    }

                    size_t off;
                    switch (imp->flags & bo4::GIF_CALLTYPE_MASK) {
                    case bo4::GIF_FUNC_METHOD: // &ns::fc
                        off = 0;               // no count
                        break;
                    case bo4::GIF_FUNCTION:
                    case bo4::GIF_FUNCTION_CHILDTHREAD:
                    case bo4::GIF_FUNCTION_THREAD:
                    case bo4::GIF_METHOD:
                    case bo4::GIF_METHOD_CHILDTHREAD:
                    case bo4::GIF_METHOD_THREAD:
                        off = 1;
                        break;
                    default:
                        LOG_ERROR("INVALID LINK ITEM, BAD IMPORT TYPE {}", (int)(imp->flags & bo4::GIF_CALLTYPE_MASK));
                        return;
                    }

                    byte* detourFunc{ dbgobj->obj->magic + detour->location };
                    byte* detourFuncEnd{ detourFunc + detour->size };

                    size_t match{};
                    for (size_t j = 0; j < imp->num_address; j++) {
                        byte* loc{ utils::Aligned<uint16_t>(obj->magic + locs[j]) };

                        if (loc >= detourFunc && loc < detourFuncEnd) {
                            continue; // inside the def, we do not replace it
                        }

                        // PATCHME: patch opcode for API
                        // *(uint16_t*)loc = 0;

                        byte** ref{ (byte**)utils::Aligned<byte*>(loc + 2 + off) };

                        *ref = detourFunc;
                        match++;
                    }

                    if (traceDetours) {
                        LOG_TRACE(
                            "[{}] Loading detour {}<{}>::{} with {}/{} patch(es)",
                            core::hashes::ExtractTmp("script", obj->name),
                            core::hashes::ExtractTmp("namespace", detour->name_space),
                            core::hashes::ExtractTmp("script", detour->script),
                            core::hashes::ExtractTmp("function", detour->name),
                            match,
                            (int)imp->num_address
                        );
                    }
                }
            }
        }

        void Scr_CheckDetour(objGDBFileInfo_t* dbgobj, bo4::GSC_OBJ* obj) {
            if (!dbgobj || !obj->imports_count) {
                return;
            }

            if (dbgobj->addons && dbgobj->addons->HasFeature(acts_addons::AAF_DETOUR)) {
                Scr_CheckDetour(
                    dbgobj,
                    obj,
                    dbgobj->addons->GetDetours(dbgobj->obj->magic),
                    dbgobj->addons->GetDetoursEnd(dbgobj->obj->magic)
                );
            }

            if (dbgobj->gdb && dbgobj->gdb->HasFeature(acts_debug::ADF_DETOUR)) {
                Scr_CheckDetour(dbgobj, obj, dbgobj->gdb->GetDetours(), dbgobj->gdb->GetDetoursEnd());
            }
        }

        // sync the registered detours for this entry
        void Scr_SyncDetours(bo4::scriptInstance_t inst, bo4::GSC_OBJ* obj) {
            size_t count{ (size_t)gObjGDBFileInfoCount[inst] };
            for (size_t i = 0; i < count; i++) {
                Scr_CheckDetour(&gObjGDBFileInfo[inst][i], obj);
            }
        }

        // sync the previously linked scripts with a new debug object
        void Scr_AddDetours(bo4::scriptInstance_t inst, objGDBFileInfo_t* dbgobj) {
            if (!dbgobj) {
                return;
            }

            size_t count{ (size_t)bo4::gObjFileInfoCount[inst] };
            for (size_t i = 0; i < count; i++) {
                bo4::GSC_OBJ* obj{ (*bo4::gObjFileInfo)[inst][i].activeVersion };
                if (dbgobj->obj == obj) {
                    continue; // avoid double match
                }
                Scr_CheckDetour(dbgobj, obj);
            }
        }

        void Scr_GdbLink(bo4::scriptInstance_t inst, bo4::GSC_OBJ* prime_obj, objGDBFileInfo_t** obj) {

            XHash name{ prime_obj->name };
            bo4::ScriptParseTreeDBG* dbg{
                bo4::DB_FindXAssetHeader(bo4::XAssetType::ASSET_TYPE_SCRIPTPARSETREEDBG, &name, false, -1).sptdbg
            };
            if (!dbg || !dbg->gdb) {
                return; // no debug data
            }

            if (dbg->gdbLen >= sizeof(uint64_t) && dbg->gdb->GetMagic() == acts_debug::MAGIC) {
                acts_debug::GSC_ACTS_DEBUG* gdb{ dbg->gdb };

                if (!gdb->HasFeature(acts_debug::ADF_CHECKSUM)) {
                    LOG_WARNING(
                        "Found ADF file without checksum feature: {}",
                        core::hashes::ExtractTmp("script", dbg->name)
                    );
                } else {
                    if (gdb->checksum != prime_obj->crc) {
                        LOG_ERROR(
                            "Found ADF file with bad checksum for file {}",
                            core::hashes::ExtractTmp("script", dbg->name)
                        );
                        return;
                    }
                }

                if (gdb->HasFeature(acts_debug::ADF_STRING)) {
                    uint32_t* strings{ gdb->GetStrings() };
                    uint32_t* stringsend{ gdb->GetStringsEnd() };

                    // load strings
                    for (; strings != stringsend; strings++) {
                        const char* str{ gdb->GetString(*strings) };
                        core::hashes::AddPrecomputed(hash::Hash64(str), str);
                        core::hashes::AddPrecomputed(hash::HashT89Scr(str), str);
                    }
                }

                if (enableDevBlocks && gdb->HasFeature(acts_debug::ADF_DEVBLOCK_BEGIN)) {
                    // link the dev blocks op
                    uint32_t* devBlocks{ gdb->GetDevBlocks() };
                    uint32_t* devBlocksEnd{ gdb->GetDevBlocksEnd() };
                    while (devBlocks != devBlocksEnd) {
                        uint16_t* opbase{ (uint16_t*)&prime_obj->magic[*(devBlocks++)] };
                        *opbase = systems::gsc::op::OPMT_DevblockBeginLinked | systems::gsc::op::MOD_TOOL_FLAG;
                    }
                }

                if (!*obj) {
                    *obj = &gObjGDBFileInfo[inst][gObjGDBFileInfoCount[inst]++];
                    (*obj)->name = dbg->name;
                    (*obj)->obj = prime_obj;
                }
                (*obj)->gdb = gdb;
                LOG_TRACE("load acts debug data {}", core::hashes::ExtractTmp("script", dbg->name));
            }

            if (enableDevBlocks) {
                // link the dev blocks data

                // link strings
                bo4::GSC_STRINGTABLE_ITEM* string{
                    (bo4::GSC_STRINGTABLE_ITEM*)&prime_obj->magic[prime_obj->devblock_string_offset]
                };
                const char* gdbBase{ (char*)dbg->gdb };

                for (size_t i = 0; i < prime_obj->devblock_string_count; i++) {
                    const char* str{ &gdbBase[string->string] };
                    uint32_t* refs{ (uint32_t*)&string[1] };

                    bo4::ScrString_t sstr{ bo4::SL_GetStringOfSize(str, 0, bo4::MT_TYPE_SCRIPT_PARSE, true) };
                    bo4::SL_TransferRefToUser(sstr, 0);

                    for (size_t j = 0; j < string->num_address; j++) {
                        ScrVm_Assert(
                            inst,
                            (refs[j] >= prime_obj->cseg_offset &&
                             refs[j] < prime_obj->cseg_offset + prime_obj->cseg_size)
                        );

                        *(uint32_t*)&prime_obj->magic[refs[j]] = sstr;
                    }

                    string = (bo4::GSC_STRINGTABLE_ITEM*)&refs[string->num_address];
                }
            }
        }

        void Scr_GscLink_Stub(bo4::scriptInstance_t inst, XHash* filename, bool runScript) {
            bo4::ScopedCriticalSection scs{ bo4::CRITSECT_VM, bo4::SCOPED_CRITSECT_NORMAL };

            Scr_GscLink_Detour.Call(inst, filename, runScript);

            // we now have to link the newly linked scripts
            size_t count{ (size_t)bo4::gObjFileInfoCount[inst] };
            for (; gObjCountLinked[inst] < count; gObjCountLinked[inst]++) {
                bo4::GSC_OBJ* prime_obj{ (*bo4::gObjFileInfo)[inst][gObjCountLinked[inst]].activeVersion };
                XHash hash{ prime_obj->name };

                // search addon data, we don't have the size, so we assume that the cseg is not 0 (acts/treyarch valid
                // assumption)
                objGDBFileInfo_t* obj{};
                if (prime_obj->cseg_offset > sizeof(bo4::GSC_OBJ) + sizeof(uint64_t)) {
                    GSC_ACTS_ADDONS* addons{ (GSC_ACTS_ADDONS*)&prime_obj[1] };

                    if (addons->GetMagic() == acts_addons::MAGIC) {
                        obj = &gObjGDBFileInfo[inst][gObjGDBFileInfoCount[inst]++];
                        obj->name = hash;
                        obj->obj = prime_obj;
                        obj->addons = addons;
                        LOG_TRACE(
                            "load acts addons data {} detours={} {}",
                            core::hashes::ExtractTmp("script", filename->name),
                            addons->detour_count,
                            (void*)obj
                        );
                    }
                }

                // load dbg data
                Scr_GdbLink(inst, prime_obj, &obj);
                Scr_AddDetours(inst, obj);
                Scr_SyncDetours(inst, prime_obj);
            }
        }

        // find an import in a script
        inline void*
        Scr_GetExport(bo4::GSC_OBJ* prime_obj, bo4::GSC_IMPORT_ITEM* imp, bool allowPrivate, bool checkParams) {
            for (size_t i = 0; i < prime_obj->exports_count; i++) {
                bo4::GSC_EXPORT_ITEM* item{
                    &((bo4::GSC_EXPORT_ITEM*)&prime_obj->magic[prime_obj->export_table_offset])[i]
                };

                // validate param namespace::name
                if (item->name != imp->name || item->name_space != imp->name_space) {
                    continue;
                }
                // validate params count (too many or not a VE)
                if (checkParams && item->param_count < imp->param_count &&
                    !(item->flags & bo4::GSC_EXPORT_FLAGS::GEF_VE)) {
                    LOG_WARNING(
                        "call {} {}::{} can't match item: too many params",
                        core::hashes::ExtractTmp("script", imp->name_space),
                        core::hashes::ExtractTmp("namespace", imp->name_space),
                        core::hashes::ExtractTmp("function", imp->name)
                    );
                    continue;
                }
                // ignore private items
                if (!allowPrivate && (item->flags & bo4::GSC_EXPORT_FLAGS::GEF_PRIVATE)) {
                    LOG_WARNING(
                        "call {} {}::{} can't match item: can't import private members",
                        core::hashes::ExtractTmp("script", prime_obj->name),
                        core::hashes::ExtractTmp("namespace", imp->name_space),
                        core::hashes::ExtractTmp("function", imp->name)
                    );
                    continue;
                }

                item->flags |= bo4::GSC_EXPORT_FLAGS::GEF_LINKED;
                return &prime_obj->magic[item->address];
            }
            return nullptr;
        }

        // find an export referenced in a script
        inline void* Scr_GetScriptExport(
            bo4::scriptInstance_t inst, bo4::GSC_OBJ* prime_obj, bo4::GSC_IMPORT_ITEM* imp, bool errorMissing,
            bool checkParams
        ) {
            void* p;

            if ((p = Scr_GetExport(prime_obj, imp, true, checkParams)) != nullptr) {
                return p;
            }
            uint64_t* includes{ (uint64_t*)&prime_obj->magic[prime_obj->include_offset] };

            for (size_t i = 0; i < prime_obj->include_count; i++) {
                XHash incl{ includes[i] };

                bo4::ScriptParseTree* spt{
                    bo4::DB_FindXAssetHeader(bo4::XAssetType::ASSET_TYPE_SCRIPTPARSETREE, &incl, false, -1).spt
                };

                if (!spt) {
                    systems::errors::ScrVm_Error(
                        inst,
                        "Could not load %s: can't find include %s",
                        true,
                        core::hashes::ExtractTmp("script", prime_obj->name),
                        core::hashes::ExtractTmp("script", incl)
                    );
                    continue;
                }

                if ((p = Scr_GetExport((bo4::GSC_OBJ*)spt->buffer, imp, false, checkParams)) != nullptr) {
                    return p;
                }
            }

            if (errorMissing) {
                systems::errors::ScrVm_Error(
                    inst,
                    "Could not load %s: can't find script import %s::%s",
                    true,
                    core::hashes::ExtractTmp("script", prime_obj->name),
                    core::hashes::ExtractTmp("namespace", imp->name_space),
                    core::hashes::ExtractTmp("function", imp->name)
                );
            }
            return nullptr;
        }

        constexpr uint32_t namespaceSys = hash::HashT89Scr("sys");
        constexpr uint32_t namespaceEmpty = hash::HashT89Scr("");

        // see gsc_opcodes_t8_36.hpp
        static std::array getFunctionOps{ 0x432, 0x5d8, 0x6a1, 0x718, 0xb64, 0xb9c, 0xc91, 0xcb1, 0xedb, 0xf16 };
        static std::array getResFunctionOps{ 0x181, 0x222, 0x230, 0x232, 0x2aa, 0x2e0, 0x34d, 0x402, 0x434, 0x4db };
        static std::array callBuiltinFunctionOps{ 0xf };
        static std::array callBuiltinMethodOps{ 0x10 };
        static std::array scriptFunctionCallOps{ 0x5c,  0x68,  0xa7,  0x17e, 0x24c, 0x327, 0x37c,
                                                 0x75c, 0x814, 0x834, 0x880, 0x97f, 0xac8, 0xbb6,
                                                 0xc0e, 0xc9b, 0xcb5, 0xcf1, 0xde4, 0xeca, 0xfd3 };
        static std::array scriptThreadCallOps{ 0x45,  0x9d,  0xa4,  0x1aa, 0x223, 0x356, 0x392, 0x442, 0x8cd, 0x966,
                                               0xa63, 0xabb, 0xb34, 0xb98, 0xd48, 0xd5a, 0xe10, 0xef4, 0xf53, 0xfdc };
        static std::array scriptThreadCallEndOnOps{ 0x26,  0xfe,  0x23f, 0x2a2, 0x3c8, 0x4d8, 0x4eb, 0x526, 0x629,
                                                    0x688, 0x70b, 0x788, 0x868, 0xa1f, 0xa64, 0xaeb, 0xbb8, 0xbd3,
                                                    0xc4d, 0xd1f, 0xd35, 0xdb3, 0xe42, 0xeb3, 0xeb8, 0xf7f, 0xfe2 };
        static std::array scriptMethodCallOps{ 0x1e6, 0x36f, 0x425, 0x6c4, 0x705, 0x7f2, 0x8b2,
                                               0x8b6, 0x983, 0xa4c, 0xb78, 0xcad, 0xd52 };
        static std::array scriptMethodThreadCallOps{ 0x248, 0x53d, 0x590, 0x78e, 0x83d, 0xa34,
                                                     0xbe4, 0xc93, 0xcc5, 0xd97, 0xe75 };
        static std::array scriptMethodThreadCallEndOnOps{ 0x36,  0x172, 0x38a, 0x621, 0x775,
                                                          0xa2d, 0xa5c, 0xdbe, 0xf4c, 0xfcb };

        template<size_t len>
        inline uint16_t RandomOp(const std::array<int, len>& arr) {
            static_assert(len, "empty len");
            if constexpr (len == 1) {
                return arr[0];
            } else {
                return (uint16_t)arr[utils::data::Rand(arr.size())];
            }
        }

        inline uint16_t Scr_FindOpCode(size_t type, bool isBuiltin) {
            switch (type) {
            case bo4::GIF_FUNC_METHOD:
                return isBuiltin ? RandomOp(getFunctionOps) : RandomOp(getResFunctionOps);
            case bo4::GIF_FUNCTION:
                return isBuiltin ? RandomOp(callBuiltinFunctionOps) : RandomOp(scriptFunctionCallOps);
            case bo4::GIF_METHOD:
                return isBuiltin ? RandomOp(callBuiltinMethodOps) : RandomOp(scriptMethodCallOps);
            case bo4::GIF_FUNCTION_THREAD:
                return RandomOp(scriptThreadCallOps);
            case bo4::GIF_FUNCTION_CHILDTHREAD:
                return RandomOp(scriptThreadCallEndOnOps);
            case bo4::GIF_METHOD_THREAD:
                return RandomOp(scriptMethodThreadCallOps);
            case bo4::GIF_METHOD_CHILDTHREAD:
                return RandomOp(scriptMethodThreadCallEndOnOps);
            default:
                LOG_ERROR("Scr_FindOpCode: Missing import flags op {}", (int)type);
                return 0;
            }
        }

        int GscObjResolve(bo4::scriptInstance_t inst, bo4::GSC_OBJ* prime_obj) {
            // it's faster to reimplement everything than only adding the dev calls

            bo4::GSC_IMPORT_ITEM* it{ (bo4::GSC_IMPORT_ITEM*)&prime_obj->magic[prime_obj->imports_offset] };

            for (size_t i = 0; i < prime_obj->imports_count; i++) {
                bo4::GSC_IMPORT_ITEM* imp{ it };
                uint32_t* refs{ (uint32_t*)&imp[1] };
                it = (bo4::GSC_IMPORT_ITEM*)&refs[imp->num_address];

                if ((imp->flags & bo4::GIF_DEV_CALL) && !enableDevBlocks) {
                    continue; // ignore dev calls in this run
                }

                size_t importType{ (size_t)(imp->flags & bo4::GSC_IMPORT_FLAGS::GIF_CALLTYPE_MASK) };
                if constexpr (traceLinking) {
                    LOG_TRACE(
                        "linking {} {}::{} (type={},flags=0x{:x},dev={},params={})",
                        core::hashes::ExtractTmp("script", prime_obj->name),
                        core::hashes::ExtractTmp("namespace", imp->name_space),
                        core::hashes::ExtractTmp("function", imp->name),
                        importType,
                        (int)imp->flags,
                        (imp->flags & bo4::GIF_DEV_CALL ? "true" : "false"),
                        (int)imp->param_count
                    );
                }
                bool linkParams{ importType != bo4::GIF_FUNC_METHOD };
                void* func;
                uint16_t op;

                switch (importType) {
                case bo4::GIF_FUNCTION_THREAD:
                case bo4::GIF_FUNCTION_CHILDTHREAD:
                case bo4::GIF_METHOD_THREAD:
                case bo4::GIF_METHOD_CHILDTHREAD:
                    op = Scr_FindOpCode(importType, false);
                    func = Scr_GetScriptExport(inst, prime_obj, imp, true, true);
                    break;
                case bo4::GIF_FUNC_METHOD:
                case bo4::GIF_FUNCTION:
                case bo4::GIF_METHOD: {
                    bool scriptCallOnly{ imp->name_space != namespaceSys && imp->name_space != namespaceEmpty &&
                                         !(imp->flags & bo4::GIF_GET_CALL) };
                    func = Scr_GetScriptExport(inst, prime_obj, imp, scriptCallOnly, linkParams);

                    if (func) {
                        op = Scr_FindOpCode(importType, false);
                    } else {
                        int type{};
                        int min_args{};
                        int max_args{};

                        switch (importType) {
                        case bo4::GIF_FUNC_METHOD:
                            if (inst == bo4::scriptInstance_t::SI_CLIENT) {
                                func = bo4::CScr_GetFunction(imp->name, &type, &min_args, &max_args);
                                if (!func) {
                                    func = bo4::CScr_GetMethod(imp->name, &type, &min_args, &max_args);
                                }
                            } else {
                                func = bo4::Scr_GetFunction(imp->name, &type, &min_args, &max_args);
                                if (!func) {
                                    func = bo4::Scr_GetMethod(imp->name, &type, &min_args, &max_args);
                                }
                            }
                            break;
                        case bo4::GIF_FUNCTION:
                            if (inst == bo4::scriptInstance_t::SI_CLIENT) {
                                func = bo4::CScr_GetFunction(imp->name, &type, &min_args, &max_args);
                            } else {
                                func = bo4::Scr_GetFunction(imp->name, &type, &min_args, &max_args);
                            }
                            break;
                        case bo4::GIF_METHOD:
                            if (inst == bo4::scriptInstance_t::SI_CLIENT) {
                                func = bo4::CScr_GetMethod(imp->name, &type, &min_args, &max_args);
                            } else {
                                func = bo4::Scr_GetMethod(imp->name, &type, &min_args, &max_args);
                            }
                            break;
                        }

                        if (!func) {
                            systems::errors::ScrVm_Error(
                                inst,
                                "Could not load %s: can't find import %s::%s%s",
                                true,
                                core::hashes::ExtractTmp("script", prime_obj->name),
                                core::hashes::ExtractTmp("namespace", imp->name_space),
                                core::hashes::ExtractTmp("function", imp->name),
                                (imp->flags & bo4::GIF_DEV_CALL) ? " (dev call)" : ""
                            );
                            return -3;
                        }

                        // validate that we can call this API function

                        // dev call
                        if (type && !(imp->flags & bo4::GIF_DEV_CALL)) {
                            systems::errors::ScrVm_Error(
                                inst,
                                "Could not load %s: Dev only calls must be wrapped in a devblock /#...#/ %s",
                                true,
                                core::hashes::ExtractTmp("script", prime_obj->name),
                                core::hashes::ExtractTmp("function", imp->name)
                            );
                            return -3;
                        }
                        if (linkParams) {
                            // max params
                            if (imp->param_count > max_args) {
                                systems::errors::ScrVm_Error(
                                    inst,
                                    "Could not load %s: too many parameters for call %s",
                                    true,
                                    core::hashes::ExtractTmp("script", prime_obj->name),
                                    core::hashes::ExtractTmp("function", imp->name)
                                );
                                return -3;
                            }
                            // min params
                            if (imp->param_count < min_args) {
                                systems::errors::ScrVm_Error(
                                    inst,
                                    "Could not load %s: not enough parameters for call %s",
                                    true,
                                    core::hashes::ExtractTmp("script", prime_obj->name),
                                    core::hashes::ExtractTmp("function", imp->name)
                                );
                                return -3;
                            }
                        }
                        op = Scr_FindOpCode(importType, true);
                    }

                    break;
                }
                default:
                    LOG_ERROR("Invalid import flag type 0x{:x}: not implemented", importType);
                    return -3;
                }

                if (!op) {
                    return -3;
                }

                if constexpr (traceLinking) {
                    LOG_TRACE("matching {} with {} address(es)", func, (int)imp->num_address);
                }

                // link the bytecode
                for (size_t j = 0; j < imp->num_address; j++) {
                    ScrVm_Assert(
                        inst,
                        (refs[j] >= prime_obj->cseg_offset && refs[j] < prime_obj->cseg_offset + prime_obj->cseg_size)
                    );
                    byte* bytecode{ &prime_obj->magic[refs[j]] };

                    // we link the opcode (we do it so the compiler doesn't have to care about a api funcs list)
                    *(uint16_t*)bytecode = op;
                    bytecode += sizeof(uint16_t);

                    // add the params if required
                    if (linkParams) {
                        *(bytecode++) = imp->param_count;
                    }
                    // put the func offset
                    void** ref{ (void**)utils::Aligned<void*>(bytecode) };
                    *ref = func;
                }
            }

            return 0;
        }

        void Scr_GetGscExportInfo_Stub(
            bo4::scriptInstance_t inst, byte* codepos, const char** scriptname, int32_t* sloc, int32_t* crc, int32_t* vm
        ) {
            static char scriptnamebuffer[bo4::scriptInstance_t::SI_COUNT][0x200];
            if (codepos >= bo4::scrVmInvalidFuncs[inst] && codepos < (byte*)bo4::scrVmInvalidFuncs[inst] + 32) {
                // invalid link
                if (crc)
                    *crc = 0;
                if (vm)
                    *vm = 0x36;
                if (sloc)
                    *sloc = (int32_t)(codepos - (byte*)bo4::scrVmInvalidFuncs[inst]);
                if (scriptname)
                    *scriptname = "<error>";

                return;
            }
            bo4::GSC_OBJ* script_obj{};
            acts_debug::GSC_ACTS_DEBUG* gdb{};
            {
                bo4::ScopedCriticalSection scs{ bo4::CRITSECT_VM, bo4::SCOPED_CRITSECT_NORMAL };

                // find the right script
                uint32_t count{ bo4::gObjFileInfoCount[inst] };

                for (size_t i = 0; i < count; i++) {
                    bo4::objFileInfo_t& info{ (*bo4::gObjFileInfo)[inst][i] };

                    bo4::GSC_OBJ* obj{ info.activeVersion };

                    if (codepos >= obj->magic + obj->cseg_offset &&
                        codepos < obj->magic + obj->cseg_offset + obj->cseg_size) {
                        script_obj = obj;

                        int dbgcount{ gObjGDBFileInfoCount[inst] };
                        // search dbg file
                        for (size_t i = 0; i < dbgcount; i++) {
                            if (gObjGDBFileInfo[inst][i].obj == obj) {
                                gdb = gObjGDBFileInfo[inst][i].gdb;
                                break;
                            }
                        }
                        break;
                    }
                }
            }

            if (!script_obj) {
                return; // can't find obj
            }

            uint32_t rloc{ (uint32_t)(codepos - script_obj->magic) };

            if (sloc) {
                *sloc = rloc;
            }

            if (crc) {
                *crc = script_obj->crc;
            }

            if (vm) {
                *vm = script_obj->magic[7];
            }

            bo4::GSC_EXPORT_ITEM* expit{};

            for (size_t i = 0; i < script_obj->exports_count; i++) {
                bo4::GSC_EXPORT_ITEM* exp{
                    reinterpret_cast<bo4::GSC_EXPORT_ITEM*>(script_obj->magic + script_obj->export_table_offset) + i
                };

                if (rloc < exp->address) {
                    continue; // our code is after
                }

                if (expit && expit->address > exp->address) {
                    continue; // we already have a better candidate
                }

                expit = exp;
            }

            if (gdb) {
                const char* filename{};
                uint32_t line{};
                GetGDBInfo(rloc, gdb, &filename, &line);
                if (filename) {
                    // use debug data
                    sprintf_s(
                        scriptnamebuffer[inst],
                        "%s::%s:%d@%x",
                        filename,
                        core::hashes::ExtractTmp("function", expit->name),
                        line + 1,
                        rloc - expit->address
                    );
                    *scriptname = scriptnamebuffer[inst];
                    return;
                }
            };

            if (scriptname) {
                if (expit) {
                    sprintf_s(
                        scriptnamebuffer[inst],
                        "%s::%s@%x",
                        core::hashes::ExtractTmp("script", script_obj->name),
                        core::hashes::ExtractTmp("function", expit->name),
                        rloc - expit->address
                    );
                } else {
                    sprintf_s(scriptnamebuffer[inst], "%s", core::hashes::ExtractTmp("script", script_obj->name));
                }

                *scriptname = scriptnamebuffer[inst];
            }
        }

        void Scr_ResetLinkInfo_Stub(bo4::scriptInstance_t inst) {
            // cleanup debug data
            std::memset(gObjGDBFileInfo[inst], 0, sizeof(gObjGDBFileInfo[inst]));
            gObjGDBFileInfoCount[inst] = 0;
            gObjCountLinked[inst] = 0;
        }

        void PostInit(uint64_t uid) {
            enableDevBlocks = core::config::GetBool("gsc.enableDevBlocks");
            traceDetours = core::config::GetBool("test.tracedetours");

            Scr_GscLink_Detour.Create(0x2748BB0_a, Scr_GscLink_Stub);
            hook::memory::RedirectJmp(0x2748550_a, Scr_GetGscExportInfo_Stub);
            hook::memory::RedirectJmp(0x2746A30_a, GscObjResolve);
            events::EVENT_RESET_LINKS.Callback(Scr_ResetLinkInfo_Stub);
        }

        REGISTER_SYSTEM(gsc_link, nullptr, PostInit);
    } // namespace
} // namespace systems::gsc::link