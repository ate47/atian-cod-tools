#pragma once
#include <utils/hash.hpp>

namespace core::hashes::lookup {
    template<size_t maskSize = 64>
    uint64_t LookupFNV1A64(const char* str, uint64_t key = hash::FNV1A_PRIME, uint64_t iv = hash::IV_DEFAULT) {
        uint64_t mask = 0xFFFF;
        uint64_t found = 0;
        uint64_t disc{};

        while (found < maskSize) {
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
    uint64_t LookupDJB2(const char* str, uint64_t key = 0, uint64_t iv = 33);
} // namespace core::hashes::lookup