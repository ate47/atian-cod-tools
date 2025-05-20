#include <dll_includes.hpp>
#include <deps/scobalula_wni.hpp>
#include <core/system.hpp>

namespace {
	void HashLoad(uint64_t uid) {
		std::filesystem::path wnidir{ "acts/package_index" };
		std::filesystem::create_directories(wnidir);
		deps::scobalula::wni::LoadHashFile(wnidir);
	}

	REGISTER_SYSTEM(hashes, HashLoad, nullptr);
}