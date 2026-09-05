#include <dll_includes.hpp>
#include <data/cw_generated.hpp>
#include <hook/library.hpp>
#include <utils/utils.hpp>
#include <core/system.hpp>

namespace {
    byte* FindExport(bocw::scriptInstance_t inst, bocw::CWXHash target_script, uint32_t name_space, uint32_t name) {
        bocw::objFileInfo_t* end{ (*bocw::gObjFileInfo)[inst] + bocw::gObjFileInfoCount[inst] };

        auto its = std::find_if((*bocw::gObjFileInfo)[inst], end, [target_script](const bocw::objFileInfo_t& info) {
            return info.activeVersion->name == target_script;
        });

        if (its == end) {
            return nullptr;
        }

        bocw::T9GSCOBJ* obj = its->activeVersion;

        auto exportEnd = reinterpret_cast<bocw::T8GSCExport*>(obj->magic + obj->exports_tables) + obj->exports_count;
        auto ite = std::find_if(
            reinterpret_cast<bocw::T8GSCExport*>(obj->magic + obj->exports_tables),
            exportEnd,
            [name_space, name](const bocw::T8GSCExport& exp) {
                return exp.name == name && exp.name_space == name_space;
            }
        );

        if (ite == exportEnd) {
            return nullptr;
        }

        return obj->magic + ite->address;
    }

    void VM_OP_LazyLink_Handler(
        bocw::scriptInstance_t inst, void* varInfo, bocw::FunctionStack* fs_0, void* ctx, bool* terminate
    ) {
        struct LazyLinkData {
            uint32_t nameSpace;
            uint32_t name;
            uint64_t script;
        };
        LazyLinkData* data = (LazyLinkData*)utils::Aligned<uint32_t>(fs_0->pos);

        fs_0->pos = (byte*)(data + 1);

        byte* exp = FindExport(inst, data->script, data->nameSpace, data->name);

        fs_0->top++;

        if (exp) {
            fs_0->top->type = bocw::TYPE_SCRIPT_FUNCTION;
            fs_0->top->u.codePosValue = exp;
        } else {
            fs_0->top->type = bocw::TYPE_UNDEFINED;
            fs_0->top->u.intValue = 0;
        }
    }

    hook::library::Detour Scr_GscObjLinkDetour;
    void Scr_GscObjLinkStub(bocw::scriptInstance_t inst, uint64_t scriptname) {
        LOG_INFO("Load {:x}", scriptname);
        Scr_GscObjLinkDetour.Call(inst, scriptname);
    };

    void PostInit(uint64_t id) {
        // add lazylink operator
        bocw::gVmOpJumpTable[0x13] = VM_OP_LazyLink_Handler;
    }

} // namespace
REGISTER_SYSTEM(gsc, nullptr, PostInit);