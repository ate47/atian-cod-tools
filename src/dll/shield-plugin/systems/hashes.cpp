#include <dll_includes.hpp>
#include <deps/scobalula_wni.hpp>
#include <core/system.hpp>
#include <core/hashes/hash_store.hpp>
#include <hook/memory.hpp>

namespace {
	void HashLoad(uint64_t uid) {
		std::filesystem::path wnidir{ "project-bo4/acts/package_index" };
		std::filesystem::create_directories(wnidir);
		deps::scobalula::wni::LoadHashFile(wnidir);
		LOG_INFO("Loaded {} hash(es) from {}", core::hashes::GetMap().size(), wnidir.string());
	}

	const char* Com_ExtractHashTmp(uint64_t value) {
		const char* v{ core::hashes::ExtractPtr(value) };
		if (!v) v = utils::va("%016llx", value);
		//LOG_TRACE("unhash({:x}) -> {}", value, v);
		return v;
	}

	void HashPostInit(uint64_t uid) {
		hook::memory::RedirectJmp(0x2265960_a, Com_ExtractHashTmp);
	}

	REGISTER_SYSTEM(hashes, HashLoad, HashPostInit);
}
