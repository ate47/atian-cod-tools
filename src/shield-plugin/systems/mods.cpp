#include <dll_includes.hpp>
#include <data/bo4.hpp>
#include <data/refs.hpp>
#include <core/system.hpp>
#include <hook/library.hpp>
#include "mods.hpp"

namespace mods {
	ModContainer commonMods{};
	ModContainer engineMods{};

	void ModContainer::ApplyRedirects(bo4::XAssetType type, bo4::XHash* name) {
		if (!name || !name->name) return;
		std::unordered_map<uint64_t, uint64_t>& map{ redirects[type] };

		// use for to avoid inf redirects
		for (size_t i = 0; i < 10; i++) {
			auto it{ map.find(name->name) };

			if (it != map.end()) {
				name->name = it->second;
			}
		}
	}

	void ModContainer::LoadContainer(const std::filesystem::path& directory) {
		std::vector<std::filesystem::path> xmods{};
		std::filesystem::create_directories(directory);
		utils::GetFileRecurse(directory, xmods, [](const std::filesystem::path& p) {
			std::string s = p.string();
			return s.ends_with(".xmod");
		});

		std::string tmpBuff{};
		for (const std::filesystem::path& xmod : xmods) {

			if (!utils::ReadFile(xmod, tmpBuff)) {
				LOG_ERROR("Can't read xmod {}", xmod.string());
				continue;
			}

			Mod* mod{ AllocMod() };

			mod->data = commonAlloc.Alloc(tmpBuff);
			mod->dataLen = tmpBuff.length();

			core::bytebuffer::ByteBuffer reader{ mod->CreateReader() };
			// TODO: read xmod data
		}
	}

	void ModContainer::SyncContainer() {
		// TODO: sync container
	}

	void ModContainer::ClearContainer() {
		// TODO: clear container
	}

	Mod* ModContainer::AllocMod() {
		return commonAlloc.Alloc<Mod>();
	}

	void ModContainer::FreeMod(Mod* mod) {
		commonAlloc.Free(mod);
	}

	void* ModContainer::FindAsset(bo4::XAssetType type, bo4::XHash* name) {
		if (!name || !name->name) return nullptr;
		auto it = assets[type].find(name->name);
		if (it == assets[type].end()) return nullptr;
		return it->second;
	}

	bool ModContainer::DoesAssetExist(bo4::XAssetType type, bo4::XHash* name) {
		return name && name->name && assets[type].contains(name->name);
	}

	namespace {
		hook::library::Detour DB_FindXAssetHeaderDetour;
		hook::library::Detour DB_DoesXAssetExistDetour;

		void* DB_FindXAssetHeaderStub(bo4::XAssetType type, bo4::XHash* name, bool errorIfMissing, int waitTime) {
			engineMods.ApplyRedirects(type, name);
			commonMods.ApplyRedirects(type, name);

			void* val;

			if (val = engineMods.FindAsset(type, name)) return val;
			if (val = commonMods.FindAsset(type, name)) return val;

			return DB_FindXAssetHeaderDetour.Call<void*>(type, name, errorIfMissing, waitTime);
		}

		bool DB_DoesXAssetExistStub(bo4::XAssetType type, bo4::XHash* name) {
			engineMods.ApplyRedirects(type, name);
			commonMods.ApplyRedirects(type, name);

			return engineMods.DoesAssetExist(type, name) || commonMods.DoesAssetExist(type, name) || DB_DoesXAssetExistDetour.Call<bool>(type, name);
		}

		void ModsInit(uint64_t uid) {
			engineMods.LoadContainer("acts/engine");
			commonMods.LoadContainer("acts/mods");
		}

		void ModsPostInit(uint64_t uid) {

			DB_FindXAssetHeaderDetour.Create(0x2EB75B0_a, DB_FindXAssetHeaderStub);
			DB_DoesXAssetExistDetour.Create(0x2EB6C90_a, DB_DoesXAssetExistStub);
		}


		REGISTER_SYSTEM(mods, ModsInit, ModsPostInit);
	}
}