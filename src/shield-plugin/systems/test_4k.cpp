#include <dll_includes.hpp>
#include <core/config.hpp>
#include <core/system.hpp>
#include <hook/memory.hpp>

namespace mods {
	namespace {
		void Test4KPostInit(uint64_t uid) {
			if (core::config::GetBool("test.4k", false)) {
				hook::memory::ReturnVal(0x35D6EC0_a, true);
			}
		}


		REGISTER_SYSTEM(test4k, nullptr, Test4KPostInit);
	}
}