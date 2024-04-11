#include <dll_includes.hpp>
#include <core/system.hpp>


namespace {

	void Init(uint64_t id) {
		LOG_INFO("Init");
	}

	void PostInit(uint64_t id) {
		LOG_INFO("Post init");
	}

}
REGISTER_SYSTEM(core, Init, PostInit);