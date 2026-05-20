#pragma once
#include <utils/hash.hpp>

namespace core::hashes::lookup {
	uint64_t LookupFNV1A64(const char* str, uint64_t key = hash::FNV1A_PRIME, uint64_t iv = hash::IV_DEFAULT);
	uint64_t LookupDJB2(const char* str, uint64_t key = 0, uint64_t iv = 33);
}