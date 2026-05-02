#include <includes_shared.hpp>
#include <core/memory_allocator.hpp>
#include <core/hashes/hash_store.hpp>
#include <core/shared_cfg.hpp>

namespace core::hashes {
	const std::unordered_map<uint64_t, const char*>& GetMap() {
		return core::shared_cfg::GetSharedConfig().hashes.defstrings;
	}
	
	void* AllocHashMemory(size_t len) {
		return core::shared_cfg::GetSharedConfig().hashes.alloc.Alloc<byte>(len);
	}

	const char* CloneHashStr(const char* str) {
		return core::shared_cfg::GetSharedConfig().hashes.alloc.CloneStr(str);
	}

	void Clean() {
		auto& hashes{ core::shared_cfg::GetSharedConfig().hashes };
		hashes.defstrings.clear();
		hashes.alloc.FreeAll();
	}

	const char* AddPrecomputed(uint64_t value, const char* str, bool clone) {
		if (clone) {
			str = core::hashes::CloneHashStr(str);
		}
		core::shared_cfg::GetSharedConfig().hashes.defstrings.emplace(value & hash::MASK60, str);
		return str;
	}
	
	bool Extract(const char* type, uint64_t hash, char* out, size_t outSize) {
		if (!hash) {
			if (outSize) {
				*out = 0;
			}
			return true;
		}
		auto& hashes{ core::shared_cfg::GetSharedConfig().hashes };
		const auto res = hashes.defstrings.find(hash & hash::MASK60);
		if (res == hashes.defstrings.end()) {
			snprintf(out, outSize, "%s_%llx", type, hash);
			return false;
		}
		snprintf(out, outSize, "%s", res->second);
		return true;
	}

	std::pair<bool, char*> ExtractTmpPair(const char* type, uint64_t hash) {
		static thread_local char buffer[10][0x600];
		static thread_local size_t bufferIndex = 0;
		bufferIndex = (bufferIndex + 1) % ACTS_ARRAYSIZE(buffer);
		auto& buff = buffer[bufferIndex];
		return { Extract(type, hash, buff, sizeof(buff)), buff };
	}

	char* ExtractTmp(const char* type, uint64_t hash) {
		auto [ok, buff] = ExtractTmpPair(type, hash);
		return buff;
	}

	const char* ExtractPtr(uint64_t hash) {
		auto& hashes{ core::shared_cfg::GetSharedConfig().hashes };
		const auto res = hashes.defstrings.find(hash & hash::MASK60);
		if (res == hashes.defstrings.end()) {
			return NULL;
		}
		return res->second;
	}
}