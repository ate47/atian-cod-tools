#include <dll_includes.hpp>

struct LinkingInfo {
    bool method = false;
    bool function = false;
    bool get = false;
    bool resolve = false;
    std::vector<UINT32> locations{};
};

LPCCH FindFunc(LinkingInfo& info, scriptinstance::ScriptInstance inst, UINT32 name) {
    static CHAR messageBuff[0x400] = { 0 };

    int use;

    bo4::BuiltinType tcf1;
    bo4::BuiltinType tcm2;
    bo4::BuiltinType tsf3;
    bo4::BuiltinType tcm4;

    // yolo
    void* cf = bo4::CScr_GetFunction(name, &tcf1, &use, &use);
    void* cm = bo4::CScr_GetMethod(name, &tcm2, &use, &use);
    void* sf = bo4::Scr_GetFunction(name, &tsf3, &use, &use);
    void* sm = bo4::Scr_GetMethod(name, &tcm4, &use, &use);

    // all the cases

    if (info.function) {
        if (inst) {
            if (!cf) {
                if (cm) {
                    sprintf_s(messageBuff, "Can't find %s CSC func, but a CSC method exists", hash_lookup::ExtractTmp(inst, name, "function"));
                    return messageBuff;
                }
                else if (sf) {
                    sprintf_s(messageBuff, "Can't find %s CSC func, but a GSC func exists", hash_lookup::ExtractTmp(inst, name, "function"));
                    return messageBuff;
                }
                else {
                    sprintf_s(messageBuff, "Can't find %s function", hash_lookup::ExtractTmp(inst, name, "function"));
                    return messageBuff;
                }
            }
            else if (tcf1 == bo4::BUILTIN_DEV) {
                sprintf_s(messageBuff, "Call of dev function %s", hash_lookup::ExtractTmp(inst, name, "function"));
                return messageBuff;
            }
        }
        else {
            if (!sf) {
                if (sm) {
                    sprintf_s(messageBuff, "Can't find %s GSC func, but a GSC method exists", hash_lookup::ExtractTmp(inst, name, "function"));
                    return messageBuff;
                }
                else if (cf) {
                    sprintf_s(messageBuff, "Can't find %s GSC func, but a CSC func exists", hash_lookup::ExtractTmp(inst, name, "function"));
                    return messageBuff;
                }
                else {
                    sprintf_s(messageBuff, "Can't find %s function", hash_lookup::ExtractTmp(inst, name, "function"));
                    return messageBuff;
                }
            }
            else if (tsf3 == bo4::BUILTIN_DEV) {
                sprintf_s(messageBuff, "Call of dev function %s", hash_lookup::ExtractTmp(inst, name, "function"));
                return messageBuff;
            }
        }
    }

    if (info.method) {
        if (inst) {
            if (!cm) {
                if (cf) {
                    sprintf_s(messageBuff, "Can't find %s CSC method, but a CSC func exists", hash_lookup::ExtractTmp(inst, name, "function"));
                    return messageBuff;
                }
                else if (sm) {
                    sprintf_s(messageBuff, "Can't find %s CSC method, but a GSC method exists", hash_lookup::ExtractTmp(inst, name, "function"));
                    return messageBuff;
                }
                else {
                    sprintf_s(messageBuff, "Can't find %s method", hash_lookup::ExtractTmp(inst, name, "function"));
                    return messageBuff;
                }
            }
            else if (tcm2 == bo4::BUILTIN_DEV) {
                sprintf_s(messageBuff, "Call of dev method %s", hash_lookup::ExtractTmp(inst, name, "function"));
                return messageBuff;
            }
        }
        else {
            if (!sm) {
                if (cm) {
                    sprintf_s(messageBuff, "Can't find %s GSC method, but a GSC func exists", hash_lookup::ExtractTmp(inst, name, "function"));
                    return messageBuff;
                }
                else if (sf) {
                    sprintf_s(messageBuff, "Can't find %s GSC method, but a CSC method exists", hash_lookup::ExtractTmp(inst, name, "function"));
                    return messageBuff;
                }
                else {
                    sprintf_s(messageBuff, "Can't find %s method", hash_lookup::ExtractTmp(inst, name, "function"));
                    return messageBuff;
                }
            }
            else if (tcm4 == bo4::BUILTIN_DEV) {
                sprintf_s(messageBuff, "Call of dev method %s", hash_lookup::ExtractTmp(inst, name, "function"));
                return messageBuff;
            }
        }
    }

    if (info.get) {
        if (inst) {
            if (!cf && !cm) {
                if (sf || sm) {
                    sprintf_s(messageBuff, "Can't find %s CSC function/method, but a GSC function/method", hash_lookup::ExtractTmp(inst, name, "function"));
                    return messageBuff;
                }
                sprintf_s(messageBuff, "Can't find %s function/method", hash_lookup::ExtractTmp(inst, name, "function"));
                return messageBuff;
            }
            else if ((cf && tcf1 == bo4::BUILTIN_DEV) || (cm && tcm2 == bo4::BUILTIN_DEV)) {
                sprintf_s(messageBuff, "Call of dev function/method %s", hash_lookup::ExtractTmp(inst, name, "function"));
                return messageBuff;
            }
        }
        else {
            if (!sf && !sm) {
                if (cf || cm) {
                    sprintf_s(messageBuff, "Can't find %s GSC function/method, but a CSC function/method", hash_lookup::ExtractTmp(inst, name, "function"));
                    return messageBuff;
                }
                sprintf_s(messageBuff, "Can't find %s function/method", hash_lookup::ExtractTmp(inst, name, "function"));
                return messageBuff;
            }
            else if ((sf && tsf3 == bo4::BUILTIN_DEV) || (sm && tcm4 == bo4::BUILTIN_DEV)) {
                sprintf_s(messageBuff, "Call of dev function/method %s", hash_lookup::ExtractTmp(inst, name, "function"));
                return messageBuff;
            }
        }
    }

    return NULL;
}

int find_linking::CheckLinkingError(scriptinstance::ScriptInstance inst) {
    int issues = 0;
	LOG_INFO("Check linking issues in {}", scriptinstance::Name(inst));

	auto c = bo4::objFileInfoCount[inst];

    CHAR nspBuff[0x100] = { 0 };
    CHAR nameBuff[0x100] = { 0 };

    for (size_t scriptId = 0; scriptId < c; scriptId++) {
        auto* script = bo4::objFileInfo[inst][scriptId].activeVersion;

        // LPCCH name = hash_lookup::ExtractTmp(inst, script->name);

        auto imploc = script->magic + script->imports_offset;

        std::unordered_map<UINT64, LinkingInfo> imports{};

        for (size_t j = 0; j < script->imports_count; j++) {
            const auto* imp = reinterpret_cast<bo4::T8GSCImport*>(imploc);

            if (imp->flags & bo4::GSCImportFlags::DEV_CALL) {
                continue; // unsuported
            }

            // "sys" or "" -> 0
            UINT32 nsp = imp->import_namespace == 0x222276a9 || imp->import_namespace == 0xc1243180 ? 0 : imp->import_namespace;

            auto id = utils::CatLocated(nsp, imp->name);

            auto& info = imports[id];

            const auto* importLocations = reinterpret_cast<const UINT32*>(&imp[1]);
            // insert locations
            info.locations.insert(info.locations.end(), importLocations, importLocations + imp->num_address);

            if (imp->flags & bo4::GSCImportFlags::GET_CALL) {
                info.resolve = true;
            }

            switch (imp->flags & 0xF) {
            case 1:
                // get
                info.get = true;
                break;
            case 2:
            case 3:
            case 4:
                // function call (_,thread,childthread)
                info.function = true;
                break;
            case 5:
            case 6:
            case 7:
                // method call (_,thread,childthread)
                info.method = true;
                break;
            default:
                LOG_ERROR("Detected BAD import flags {}", imp->flags & 0xF);
                issues++;
                break;
            }

            // shady pass
            imploc += (size_t) (sizeof(bo4::T8GSCImport) + sizeof(UINT32) * imp->num_address);
        }

        auto* exports = reinterpret_cast<bo4::GSCExport*>(script->magic + script->export_table_offset);

        for (size_t j = 0; j < script->exports_count; j++) {
            auto exp = exports[j];
            
            auto find = imports.find(utils::CatLocated(exp.name_space, exp.name));

            // clear things inside the script
            if (find != imports.end()) {
                imports.erase(find);
            }
        }

        auto* usings = reinterpret_cast<UINT64*>(script->magic + script->include_offset);

        for (size_t i = 0; i < script->include_count; i++) {
            auto usingScript = usings[i];
            auto end = bo4::objFileInfo[inst] + bo4::objFileInfoCount[inst];
            auto find = std::find_if(bo4::objFileInfo[inst], end, [usingScript](const auto& info) { return info.activeVersion->name == usingScript; });
            if (find == end) {
                LOG_ERROR("Can't find #using in {}", hash_lookup::ExtractTmp(inst, script->name));
                LOG_ERROR("#using {};", hash_lookup::ExtractTmp(inst, usingScript));
                issues++;
                continue;
            }

            auto* otherScript = find->activeVersion;


            auto* otherExports = reinterpret_cast<bo4::GSCExport*>(otherScript->magic + otherScript->export_table_offset);

            for (size_t j = 0; j < otherScript->exports_count; j++) {
                auto exp = otherExports[j];

                auto find = imports.find(utils::CatLocated(exp.name_space, exp.name));

                // clear things inside the other script
                if (find != imports.end()) {
                    imports.erase(find);
                }
            }
        }

        // we have all our included elements searching for them inside the VM

        for (auto& [key, info] : imports) {
            auto [nsp, name] = utils::UnCatLocated(key);

            bool dump_loc = false;
            if (nsp && !info.resolve) {
                // This call should have been handled by a previous part
                auto nsp_lookup = hash_lookup::Extract(nsp);
                auto name_lookup = hash_lookup::Extract(name);

                if (!nsp_lookup) {
                    snprintf(nspBuff, sizeof(nspBuff), "namespace_%x", nsp);
                    nsp_lookup = nspBuff;
                }
                if (!name_lookup) {
                    snprintf(nspBuff, sizeof(nspBuff), "function_%x", name);
                    name_lookup = nameBuff;
                }

                LOG_ERROR("Can't find import {}::{}, locations:", nsp_lookup, name_lookup);
                dump_loc = true;
            }
            else {
                // api call
                auto* err = FindFunc(info, inst, name);
                if (err) {
                    LOG_ERROR("{}", err);
                    dump_loc = true;
                }
            }
            if (dump_loc) {
                issues++;
                auto scriptName = hash_lookup::ExtractTmp(inst, script->name);

                for (auto loc : info.locations) {
                    LOG_ERROR("{} at {:x}", scriptName, loc);
                }
            }
        }
	}
    return issues;
}
