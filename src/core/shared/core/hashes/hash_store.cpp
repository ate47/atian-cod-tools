#include <includes_shared.hpp>
#include <core/memory_allocator.hpp>
#include <core/hashes/hash_store.hpp>

namespace core::hashes {
	namespace {
		core::memory_allocator::MemoryAllocator alloc{};
		std::unordered_map<uint64_t, const char*> defstrings{};
	}
	const std::unordered_map<uint64_t, const char*>& GetMap() {
		return defstrings;
	}
	
	void* AllocHashMemory(size_t len) {
		return alloc.Alloc<byte>(len);
	}

	const char* CloneHashStr(const char* str) {
		return alloc.CloneStr(str);
	}

	void Clean() {
		defstrings.clear();
		alloc.FreeAll();
	}

	const char* AddPrecomputed(uint64_t value, const char* str, bool clone) {
		if (clone) {
			str = core::hashes::CloneHashStr(str);
		}
		defstrings.emplace(value & hash::MASK60, str);
		return str;
	}
	
	bool Extract(const char* type, uint64_t hash, char* out, size_t outSize) {
		if (!hash) {
			if (outSize) {
				*out = 0;
			}
			return true;
		}
		const auto res = defstrings.find(hash & hash::MASK60);
		if (res == defstrings.end()) {
			snprintf(out, outSize, "%s_%llx", type, hash);
			return false;
		}
		snprintf(out, outSize, "%s", res->second);
		return true;
	}

	std::pair<bool, char*> ExtractTmpPair(const char* type, uint64_t hash) {
		static thread_local char buffer[10][0x600];
		static thread_local size_t bufferIndex = 0;
		bufferIndex = (bufferIndex + 1) % ARRAYSIZE(buffer);
		auto& buff = buffer[bufferIndex];
		return { Extract(type, hash, buff, sizeof(buff)), buff };
	}

	char* ExtractTmp(const char* type, uint64_t hash) {
		auto [ok, buff] = ExtractTmpPair(type, hash);
		return buff;
	}

	const char* ExtractPtr(uint64_t hash) {
		const auto res = defstrings.find(hash & hash::MASK60);
		if (res == defstrings.end()) {
			return NULL;
		}
		return res->second;
	}
}