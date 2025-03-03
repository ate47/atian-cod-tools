#pragma once

namespace core::bytebuffer {
	class ByteBuffer {
		byte* buffer;
		size_t len;
		size_t pointer{};

	public:
		ByteBuffer(byte* buffer, size_t len, size_t pointer = 0) : buffer(buffer), len(len), pointer(pointer) {}
		ByteBuffer(std::string& buff) : buffer((byte*)buff.data()), len(buff.size()) {}
		ByteBuffer(std::vector<byte>& buff) : buffer((byte*)buff.data()), len(buff.size()) {}
		
		bool CanRead(size_t size) const {
			return pointer + size <= len;
		}

		constexpr size_t Length() const {
			return len;
		}

		constexpr size_t Remaining() const {
			return len - pointer;
		}

		byte* GetBuffer() {
			return buffer;
		}
		
		template<typename T>
		T Read() {
			if (!CanRead(sizeof(T))) {
				throw std::runtime_error(utils::va("Reading too much at 0x%llx + 0x%llx > 0x%llx", pointer, sizeof(T), len));
			}
			T t = *(T*)&buffer[pointer];
			pointer += sizeof(T);
			return t;
		}

		void Read(void* to, size_t size) {
			if (!CanRead(size)) {
				throw std::runtime_error(utils::va("Reading pointer too much at 0x%llx + 0x%llx > 0x%llx", pointer, size, len));
			}
			
			std::memmove(to, &buffer[pointer], size);
			pointer += size;
		}

		template<typename T>
		T Read(size_t len) {
			if (!CanRead(len)) {
				throw std::runtime_error(utils::va("Reading too much at 0x%llx + 0x%llx > 0x%llx", pointer, len, len));
			}
			T t = *(T*)&buffer[pointer];
			pointer += len;
			return t;
		}

		template<typename T>
		T* ReadPtr(size_t count = 1) {
			if (!CanRead(sizeof(T) * count)) {
				throw std::runtime_error(utils::va("Reading too much at 0x%llx + 0x%llx > 0x%llx", pointer, sizeof(T) * count, len));
			}
			T* t = (T*)&buffer[pointer];
			pointer += sizeof(T) * count;
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

		constexpr size_t Size() const {
			return len;
		}

		char* ReadString(size_t* len = nullptr, char end = 0) {
			char* str{ Ptr<char>() };
			size_t _len{};
			if (end) {
				while (Read<char>() != end) _len++;
			}
			else {
				while (Read<char>()) _len++;
			}
			if (len) *len = _len;
			return str;
		}

		uint64_t ReadVByte() {
			uint64_t v{};
			byte r;
			do {
				r = Read<byte>();

				v = (v << 7) | (r & 0x7F);
			} while (r & 0x80);

			return v;
		}

		uint64_t ReadVByteInv() {
			uint64_t v{};
			byte r;
			int shift{};
			do {
				r = Read<byte>();

				v |= (uint64_t)(r & 0x7F) << shift;
				shift += 7;
			} while ((r & 0x80) == 0);

			return v;
		}

		int64_t ReadSignedVByte() {
			uint64_t v{ ReadVByte() };

			if (v & 1) {
				// -
				return ~(int64_t)(v >> 1);
			}
			else {
				// +
				return (int64_t)(v >> 1);
			}
		}

		template<typename S>
		std::string ReadSizedString(S* len = nullptr) {
			S _len = Read<S>();
			const char* str{ Ptr<const char>() };
			Skip(_len);
			if (len) *len = _len;
			return { str, str + _len };
		}

		std::string ReadVByteSizedString(uint64_t* len = nullptr) {
			uint64_t _len{ ReadVByte() };
			const char* str{ Ptr<const char>() };
			Skip(_len);
			if (len) *len = _len;
			return { str, str + _len };
		}

		std::string ReadVByteInvSizedString(uint64_t* len = nullptr) {
			uint64_t _len{ ReadVByteInv() };
			const char* str{ Ptr<const char>() };
			Skip(_len);
			if (len) *len = _len;
			return { str, str + _len };
		}

		constexpr bool End() {
			return pointer >= len;
		}
		constexpr size_t Loc() {
			return pointer;
		}

		void Goto(size_t loc) {
			if (loc > len) {
				throw std::runtime_error(utils::va("Goto after end 0x%llx + 0x%llx > 0x%llx", loc, len));
			}
			pointer = loc;
		}

		void GotoEnd() {
			pointer = len;
		}

		void Align(size_t val) {
			Goto(utils::Aligned(pointer, val));
		}

		template<typename Type>
		void Align() {
			Align(sizeof(Type));
		}

		size_t Find(byte* ptr, size_t ptrSize, size_t offset = 0) {
			size_t curr{ pointer + offset };
			while (curr + ptrSize <= len) {
				if (memcmp(ptr, Ptr(curr), ptrSize) == 0) {
					return curr;
				}
				curr++;
			}

			return std::string::npos;
		}

		std::vector<size_t> FindAll(void* ptr, size_t ptrSize) {
			std::vector<size_t> r{};

			size_t curr{ pointer };
			while (curr + ptrSize <= len) {
				if (memcmp(ptr, Ptr(curr), ptrSize) == 0) {
					r.push_back(curr);
				}
				curr++;
			}

			return r;
		}

		std::vector<size_t> FindAllMasked(void* ptr, void* mask, size_t ptrSize) {
			std::vector<size_t> r{};

			size_t curr{ pointer };
			while (curr + ptrSize <= len) {
				size_t delta{};

				while (delta < ptrSize) {
					if ((buffer[curr + delta] & ((byte*)mask)[delta]) != ((byte*)ptr)[delta]) break;
					delta++;
				}
				if (delta == ptrSize) {
					r.push_back(curr);
				}
				curr++;
			}

			return r;
		}

		size_t FindMasked(void* ptr, void* mask, size_t ptrSize, size_t offset = 0) {
			size_t curr{ pointer + offset };
			while (curr + ptrSize <= len) {
				size_t delta{};

				while (delta < ptrSize) {
					if ((buffer[curr + delta] & ((byte*)mask)[delta]) != ((byte*)ptr)[delta]) break;
					delta++;
				}
				if (delta == ptrSize) {
					return curr;
				}
				curr++;
			}

			return std::string::npos;
		}
	};


}