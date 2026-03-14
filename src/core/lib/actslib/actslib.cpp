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
		static std::mutex lock;
		static StrBuff<MAX_VA_ALLOC, MAX_VA_LEN> buf{};

		va_list va;
		
        va_start(va, fmt);

		{
			std::lock_guard lg{ lock };

			auto& buff = buf.buffer[buf.NextId()];
			vsprintf_s(buff, fmt, va);

			va_end(va);

			return buff;
		}
	}
}