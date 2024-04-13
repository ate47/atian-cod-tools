#include <dll_includes.hpp>
#include <core/system.hpp>
#include <hook/library.hpp>
#include <imgui.h>
#include "systems/core.hpp"
#include "data/cw.hpp"
#include "callbacks.hpp"

namespace {
    const char* EnableEE() {
        auto& spt = core::xassetpools[cw::ASSET_TYPE_SCRIPTPARSETREE];

        auto* pool = spt.pool.spt;

        size_t count{};
        for (size_t i = 0; i < spt.itemAllocCount; i++) {
            auto& entry = pool[i];

            if ((entry.name != hash::Hash64("scripts/zm_common/zm_utility.gsc")
                && entry.name != hash::Hash64("scripts/zm_common/zm_utility.csc"))
                || !entry.len || !entry.buffer) {
                continue;
            }

            cw::T9GSCOBJ* obj = entry.buffer;

            cw::T8GSCExport* exports = reinterpret_cast<cw::T8GSCExport*>(obj->magic + obj->exports_tables);

            for (size_t j = 0; j < obj->exports_count; j++) {
                auto& exp = exports[j];

                if (exp.name != hash::Hash32("is_ee_enabled")) {
                    continue;
                }


                static byte data[] = { 0x0d, 0x00, 0x13, 0x00, 0xca, 0x00, 0x01, 0x00, 0x1a, 0x00 };

                byte* bytecode = obj->magic + exp.address;

                memcpy(bytecode, data, sizeof(data));
                count++;
                break;
            }
            if (count == 2) {
                break;
            }
        }

        if (!count) {
            return "Can't find scripts. Are you in zombies?";
        }

        return "EE injected";
    }

	void PostInit(uint64_t id) {
        callbacks::RegisterRenderImGui([](void* ev) {
			const char** notif = reinterpret_cast<const char**>(ev);

			if (ImGui::Button("Enable EE")) {
				*notif = EnableEE();
			}
		});
	}
}
REGISTER_SYSTEM(menu_ee, nullptr, PostInit);