#include <dll_includes.hpp>
#include <data/refs.hpp>
#include <core/hashes/hash_store.hpp>
#include <core/config.hpp>
#include <core/system.hpp>
#include <hook/memory.hpp>
#include <systems/mods.hpp>

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
		}


		REGISTER_SYSTEM(test, nullptr, TestPostInit);
	}
}