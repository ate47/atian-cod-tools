#include <includes_shared.hpp>
#include <core/raw_file_json.hpp>
#include <core/hashes/hash_store.hpp>
#include <hook/memory.hpp>
#include <hook/library.hpp>


namespace core::raw_file::json {
	void RawFileJsonWriter::WriteUnknownData(const void* ptr, size_t l) {
		const byte* p{ (byte*)ptr };

		bool empty{ true };
		for (size_t i = 0; i < l; i++) {
			if (p[i]) {
				empty = false;
				break;
			}
		}

		if (empty) {
			this->WriteValueString(utils::va("[null:x%llx]", l)); // nothing
			return;
		}

		std::ostringstream ss{};

		if (l == sizeof(void*)) {
			// maybe ptr

			void* l{ *(void**)ptr };

			byte b;
			if (hook::memory::ReadMemorySafe(l, &b, sizeof(b))) {
				ss << hook::library::CodePointer{ l };
			}
		}
		if (l % 4 == 0) {
			// maybe hash list 32
			uint32_t* v{ (uint32_t*)ptr };
			size_t hc{ l / 4 };
			for (size_t i = 0; i < hc; i++) {
				uint32_t hash{ v[i] };
				const char* res{ core::hashes::ExtractPtr(hash) };
				if (res) {
					ss << "[64h0x" << std::hex << (i * 4) << "=" << res << "]";
				}
			}
		}
		if (l % 8 == 0) {
			// maybe hash list 64
			uint64_t* v{ (uint64_t*)ptr };
			size_t hc{ l / 8 };
			for (size_t i = 0; i < hc; i++) {
				uint64_t hash{ v[i] };
				const char* res{ core::hashes::ExtractPtr(hash) };
				if (res) {
					ss << "[64h0x" << std::hex << (i * 8) << "=" << res << "]";
				}
				else {
					ss << "[ll0x" << std::hex << (i * 8) << "=" << hash << "]";
				}
			}
		}
		if (l % 4 == 0) {
			float* v{ (float*)ptr };
			size_t hc{ l / 4 };
			for (size_t i = 0; i < hc; i++) {
				ss << "[f0x" << std::hex << (i * 4) << "=" << v[i] << "]";
			}
		}

		for (size_t i = 0; i < l; i++) {
			ss << " " << std::setfill('0') << std::setw(2) << (int)p[i];
		}

		this->WriteValueString(ss.str());
	}
}