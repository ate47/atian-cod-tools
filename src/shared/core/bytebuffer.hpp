#pragma once

namespace core::bytebuffer {
	class ByteBuffer {
		byte* buffer;
		size_t len;
		size_t pointer{};

	public:
		ByteBuffer(byte* buffer, size_t len) : buffer(buffer), len(len) {}

		template<typename T>
		T Read() {
			if (pointer + sizeof(T) > len) {
				throw std::runtime_error(utils::va("Reading too much at 0x%llx + 0x%llx > 0x%llx", pointer, sizeof(T), len));
			}
			T t = *(T*)&buffer[pointer];
			pointer += sizeof(T);
			return t;
		}

		void Skip(size_t len) {
			while (len--) {
				Read<byte>();
			}
		}
		template<typename T>
		void Skip() {
			Skip(sizeof(T));
		}

		template<typename T = byte>
		T* Ptr(size_t offset = 0) {
			return (T*)&buffer[pointer + offset];
		}

		const char* ReadString(size_t* len = nullptr) {
			const char* str{ Ptr<const char>() };
			size_t _len{};
			while (Read<char>()) _len++;
			if (len) *len = _len;
			return str;
		}

		constexpr bool End() {
			return pointer >= len;
		}
		constexpr size_t Loc() {
			return pointer;
		}

		void GotoEnd() {
			pointer = len;
		}
	};


}