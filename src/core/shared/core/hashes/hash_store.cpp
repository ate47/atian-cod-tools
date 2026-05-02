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

	bool Add(const char* str, bool ignoreCol, bool iw, bool async, bool clone) {
		// use the same string for all the hashes
		if (clone) {
			str = core::hashes::CloneHashStr(str);
		}
		AddPrecomputed(hash::Hash64(str), str, true);
		if (iw) {
			AddPrecomputed(hash::HashIWAsset(str), str, true);
			AddPrecomputed(hash::HashJupScr(str), str, true);
			AddPrecomputed(hash::Hash64(str, 0x811C9DC5, 0x1000193) & 0xFFFFFFFF, str, true);
			AddPrecomputed(hash::HashIWDVar(str), str, true);
			AddPrecomputed(hash::HashT10Scr(str), str, true);
			AddPrecomputed(hash::HashT10ScrSP(str), str, true);
			AddPrecomputed(hash::HashT10OmnVar(str), str, true);
		}
		bool cand32 = true;

		for (const char* s = str; *s; s++) {
			auto c = *s;
			if (!(
				(c >= 'A' && c <= 'Z')
				|| (c >= 'a' && c <= 'z')
				|| (c >= '0' && c <= '9')
				|| c == '_')) {
				cand32 = false; // a hash32 can only match [a-z0-9A-Z_]* in this context
				break;
			}
		}

		if (cand32) {
			AddPrecomputed(hash::HashT7(str), str, true);

			auto h = hash::HashT89Scr(str);
			if (!ignoreCol) {
				auto find = core::hashes::ExtractPtr(h);
				if (find && _strcmpi(str, find)) {
					LOG_WARNING("Coll '{}'='{}' #{:x}", str, find, h);
					return false;
				}
			}
			AddPrecomputed(h, str, true);
		}
		return true;
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