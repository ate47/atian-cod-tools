#include <dll_includes.hpp>
#include <core/system.hpp>
#include <hook/library.hpp>
#include <imgui.h>
#include "systems/core.hpp"
#include "data/cw.hpp"
#include "callbacks.hpp"

namespace {
	int DecryptGSCFile(cw::T9GSCOBJ* script) {
		uintptr_t str_location = reinterpret_cast<uintptr_t>(&script->magic[script->string_offset]);

		auto decrypted = 0;
		for (size_t i = 0; i < script->string_count; i++) {

			const auto* str = reinterpret_cast<cw::T8GSCString*>(str_location);

			byte* encryptedString = &script->magic[str->string];

			auto* sd = reinterpret_cast<const char*>(core::DecryptString(encryptedString));

			decrypted++;

			const auto* strings = reinterpret_cast<const uint32_t*>(&str[1]);

			str_location += sizeof(*str) + sizeof(*strings) * str->num_address;
		}

		return decrypted;
	}

	void PostInit(uint64_t id) {
		callbacks::RegisterRenderImGui([](void* ev) {
			const char** notif = reinterpret_cast<const char**>(ev);

			if (ImGui::Button("Dump gsc")) {

				auto& pool = core::xassetpools[cw::ASSET_TYPE_SCRIPTPARSETREE];

				auto* spts = pool.pool.spt;

				std::filesystem::path dumpDir = "scriptparsetree_cw_decrypt";
				std::filesystem::create_directories(dumpDir);

				for (size_t i = 0; i < pool.itemAllocCount; i++) {
					auto& spt = spts[i];

					if (!spt.buffer || !spt.len) {
						continue;
					}

					// decrypt strings
					DecryptGSCFile(spt.buffer);

					auto out = dumpDir / utils::va("script_%llx.gscc", spt.buffer->name);

					if (!utils::WriteFile(out, spt.buffer, spt.len)) {
						LOG_ERROR("Can't dump buffer {}", out.string());
					}
				}

				*notif = "gsc dumped";
				return;
			}
		});
	}
}
REGISTER_SYSTEM(menu_dump, nullptr, PostInit);