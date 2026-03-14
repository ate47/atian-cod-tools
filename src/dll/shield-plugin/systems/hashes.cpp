#include <dll_includes.hpp>
#include <deps/scobalula_wni.hpp>
#include <core/system.hpp>
#include <core/hashes/hash_store.hpp>

namespace {
	void HashLoad(uint64_t uid) {
		std::filesystem::path wnidir{ "project-bo4/acts/package_index" };
		std::filesystem::create_directories(wnidir);
		deps::scobalula::wni::LoadHashFile(wnidir);
		LOG_INFO("Loaded {} hash(es) from {}", core::hashes::GetMap().size(), wnidir.string());
	}

	REGISTER_SYSTEM(hashes, HashLoad, nullptr);
}