#include "actslib.hpp"


namespace actslib {

	template<size_t Buffers, size_t Size>
	struct StrBuff {
		char buffer[Buffers][Size]{};
		size_t loc{};

		constexpr size_t NextId() {
			return loc = (loc + 1) % Buffers;
		}

	};

	const char* va(const char* fmt, ...) {
		static thread_local StrBuff<0x10, 0x200> buf{};

		va_list va;
		
        va_start(va, fmt);

		auto& buff = buf.buffer[buf.NextId()];
		sprintf_s(buff, fmt);

		va_end(va);

		return buff;
	}
}