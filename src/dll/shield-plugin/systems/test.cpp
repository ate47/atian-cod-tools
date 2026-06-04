#include <dll_includes.hpp>
#include <data/refs.hpp>
#include <core/hashes/hash_store.hpp>
#include <core/config.hpp>
#include <core/system.hpp>
#include <hook/memory.hpp>
#include <hook/library.hpp>
#include <systems/mods.hpp>
#include <deps/idc_builder.hpp>

namespace systems::test {
	namespace {
		struct PlayerRoleTemplate;
		struct CharacterHead;
		struct StoreProducts;
		struct GfxImage;

		struct PlayerRoleLevels {
			bool enabled;
			PlayerRoleTemplate* levels;
		};
		static_assert(sizeof(PlayerRoleLevels) == 0x10);

		struct CustomizationTable {
			XHash name;
			uint32_t numPlayerRoles;
			PlayerRoleLevels** playerRoles;
			uint32_t numHeads;
			CharacterHead* heads;
		};
		static_assert(sizeof(CustomizationTable) == 0x30);

		struct StoreCategory {
			const char* name2;
			XHash name;
			GfxImage* image;
			XHash displayName;
			byte productsCount;
			StoreProducts** products;
			bool visibility;
		};
		static_assert(sizeof(StoreCategory) == 0x48);



		void ModAssetLinkHookCustomizationTable(bo4::XAsset* asset) {
			if (!asset || asset->type != games::bo4::pool::ASSET_TYPE_CUSTOMIZATION_TABLE) {
				return;
			}

			CustomizationTable& table{ *(CustomizationTable*)asset->header };

			if (table.playerRoles) {
				for (size_t i = 0; i < table.numPlayerRoles; i++) {
					if (!table.playerRoles[i] || table.playerRoles[i]->enabled || !table.playerRoles[i]->levels) {
						continue;
					}

					XHash role{ *(XHash*)table.playerRoles[i]->levels };

					LOG_DEBUG("patch CustomizationTable::playerRoles[{}] {}", i, core::hashes::ExtractTmp("hash", role));

					table.playerRoles[i]->enabled = true;
				}
			}
		}

		void ModAssetLinkHookStoreCategory(bo4::XAsset* asset) {
			if (!asset || asset->type != games::bo4::pool::ASSET_TYPE_STORECATEGORY) {
				return;
			}

			StoreCategory& cat{ *(StoreCategory*)asset->header };

			if (!cat.visibility) {
				LOG_DEBUG("patch StoreCategory::visibility {}", core::hashes::ExtractTmp("hash", cat.name));
				cat.visibility = true;
			}
		}

		
		void DumpDVarRefs_f() {
			utils::OutFileCE os{ "dvar.csv" };

			if (!os) {
				LOG_ERROR("Can't open dvar.csv");
				return;
			}

			hook::library::Library base{};
			const bo4::dvar_t** dbase{ *bo4::s_dvarHashTable.ptr };
			const bo4::dvar_t** dend{ &dbase[ACTS_ARRAYSIZE(*bo4::s_dvarHashTable.ptr)] };
			const bo4::dvar_t* dpbase{ *bo4::s_dvarPool.ptr };
			const bo4::dvar_t* dpend{ &dpbase[ACTS_ARRAYSIZE(*bo4::s_dvarPool.ptr)] };

			for (const bo4::dvar_t* node : *bo4::s_dvarHashTable.ptr) {
				while (node) {
					for (hook::library::ScanResult r : base.ScanNumber(node)) {
						if (dbase <= r.GetPtr() && dend > r.GetPtr()) {
							continue; // in pool (buckets)
						}
						if (dpbase <= r.GetPtr() && dpend > r.GetPtr()) {
							continue; // in pool (refs)
						}
						os
							<< hook::library::CodePointer{ r.GetPtr() } << ","
							<< core::hashes::ExtractTmp("dvar", node->name) << "\n"
							;

					}

					node = node->hashnext;
				}
			}
			LOG_ERROR("Dump dvar.csv");

		}

		void TestPostInit(uint64_t uid) {
			if (core::config::GetBool("test.4k", false)) {
				hook::memory::ReturnVal(bo4::DB_Is4KEnabled.ptr, true);
			}
			if (core::config::GetBool("test.playerroole", false)) {
				// enable all the playerroles -> (none + infected zombies in wz/mp)
				systems::mods::GetModAssetLinkHooks().emplace_back(ModAssetLinkHookCustomizationTable);
			}
			if (core::config::GetBool("test.storecat", false)) {
				// enable all the playerroles -> (none + infected zombies in wz/mp)
				systems::mods::GetModAssetLinkHooks().emplace_back(ModAssetLinkHookStoreCategory);
			}

			Cmd_AddCommand("dumpdvars", DumpDVarRefs_f);
		}


		REGISTER_SYSTEM(test, nullptr, TestPostInit);
	}
}