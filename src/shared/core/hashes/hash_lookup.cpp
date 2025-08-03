#include <includes_shared.hpp>
#include <core/hashes/hash_lookup.hpp>

namespace core::hashes::lookup {
	uint64_t LookupFNV1A64(const char* str, uint64_t key, uint64_t iv) {
		uint64_t mask = 0xFFFF;
		uint64_t found = 0;
		uint64_t disc{};

		while (found < 64) {
			uint64_t k;
			for (k = 0; k < 0x10000; k++) {
				uint64_t v = (k << found) | disc;
				if ((hash::Hash64A(str, v, iv) & mask) == (key & mask)) {
					break;
				}
			}
			if (k == 0x10000) {
				LOG_ERROR("Invalid key");
				return 0;
			}

			disc |= (k & 0xF) << found;

			found += 4;
			mask = (mask << 4) | 0xF;
		}
		return disc;
	}
}