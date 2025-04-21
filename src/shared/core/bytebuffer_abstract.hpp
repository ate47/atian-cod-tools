#pragma once

namespace core::bytebuffer {

	class AbstractByteBuffer {
	public:
		virtual bool CanRead(size_t size) const = 0;
		virtual void ReadImpl(void* to, size_t size) = 0;
		virtual void Goto(size_t loc) = 0;
		virtual size_t Length() const = 0;
		virtual size_t Remaining() const = 0;
		virtual size_t Loc() const = 0;

		bool End() const {
			return Loc() >= Length();
		}

		size_t Size() const {
			return Length();
		}

		void GotoEnd() {
			Goto(Length());
		}

		inline void Read(void* to, size_t size) {
			ReadImpl(to, size);
		}

		template<typename Type>
		void Align() {
			Align(sizeof(Type));
		}

		template<typename T = byte>
		void Skip(size_t len = 1) {
			byte tmp[0x400];
			size_t tor{ sizeof(T) * len };
			while (tor) {
				size_t r{ std::min<size_t>(sizeof(tmp), tor) };
				Read(&tmp, r);
				tor -= r;
			}
		}

		void Align(size_t val) {
			size_t l{ Loc() };
			size_t al{ utils::Aligned(l, val) };
			if (l != al) {
				Skip(al - l);
			}
		}

		template<typename T>
		T Read() {
			if (!CanRead(sizeof(T))) {
				throw std::runtime_error(utils::va("Reading too much at 0x%llx + 0x%llx > 0x%llx", Loc(), sizeof(T), End()));
			}
			T t;
			Read(&t, sizeof(T));
			return t;
		}

		template<typename T>
		T Read(size_t len) {
			if (!CanRead(len)) {
				throw std::runtime_error(utils::va("Reading too much at 0x%llx + 0x%llx > 0x%llx", Loc(), len, End()));
			}
			T t;
			size_t toread{ std::min<size_t>(sizeof(len), len) };
			Read(&t, toread);
			if (toread < len) {
				Skip(toread - len);
			}
			return t;
		}

		template<typename T>
		std::unique_ptr<T> ReadObject() {
			if (!CanRead(sizeof(T))) {
				throw std::runtime_error(utils::va("Reading too much at 0x%llx + 0x%llx > 0x%llx", Loc(), sizeof(T), End()));
			}
			std::unique_ptr<T> t{ std::make_unique<T>() };
			Read(t.get(), sizeof(T));
			return t;
		}

		template<typename T>
		std::unique_ptr<T[]> ReadArray(size_t count) {
			if (!CanRead(sizeof(T) * count)) {
				throw std::runtime_error(utils::va("Reading too much at 0x%llx + 0x%llx > 0x%llx", Loc(), sizeof(T) * count, End()));
			}
			std::unique_ptr<T[]> t{std::make_unique<T[]>(count)};
			Read(t.get(), sizeof(T) * count);
			return t;
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
			std::string ret{};
			ret.resize((size_t)_len);
			Read(ret.data(), _len);
			if (len) *len = _len;
			return ret;
		}

		std::string ReadVByteSizedString(uint64_t* len = nullptr) {
			uint64_t _len{ ReadVByte() };
			std::string ret{};
			ret.resize(_len);
			Read(ret.data(), _len);
			if (len) *len = _len;
			return ret;
		}

		std::string ReadVByteInvSizedString(uint64_t* len = nullptr) {
			uint64_t _len{ ReadVByteInv() };
			std::string ret{};
			ret.resize(_len);
			Read(ret.data(), _len);
			if (len) *len = _len;
			return ret;
		}

		std::string ReadUnkString(uint64_t* len = nullptr) {
			std::string ret{};
			ret.resize(16);
			size_t _len{};

			char c;

			while (c = Read<char>()) {
				ret[_len++] = c;
				if (_len == ret.size()) {
					ret.resize(ret.size() * 2);
				}
			}
			ret[_len] = 0;

			ret.resize(_len);
			if (len) *len = _len;
			return ret;
		}

	};
}