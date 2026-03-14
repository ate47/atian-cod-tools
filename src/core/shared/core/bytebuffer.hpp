#pragma once
#include "bytebuffer_abstract.hpp"

namespace core::bytebuffer {

	class ByteBuffer : public AbstractByteBuffer {
		byte* buffer;
		size_t len;
		size_t pointer{};

	public:
		ByteBuffer(byte* buffer, size_t len, size_t pointer = 0) : buffer(buffer), len(len), pointer(pointer) {}
		ByteBuffer(std::string& buff) : buffer((byte*)buff.data()), len(buff.size()) {}
		ByteBuffer(std::vector<byte>& buff) : buffer((byte*)buff.data()), len(buff.size()) {}

		bool CanRead(size_t size) const override {
			return pointer + size <= len;
		}

		size_t Length() const override {
			return len;
		}

		size_t Remaining() const override {
			return len - pointer;
		}
		size_t Loc() const override {
			return pointer;
		}

		byte* GetBuffer() {
			return buffer;
		}

		void ReadImpl(void* to, size_t size) override {
			if (!CanRead(size)) {
				throw std::runtime_error(utils::va("Reading pointer too much at 0x%llx + 0x%llx > 0x%llx", Loc(), size, len));
			}
			
			std::memmove(to, &buffer[pointer], size);
			pointer += size;
		}

		void Goto(size_t loc) override {
			if (loc > len) {
				throw std::runtime_error(utils::va("Goto after end 0x%llx > 0x%llx", loc, len));
			}
			pointer = loc;
		}

		template<typename T = byte>
		T* Ptr(size_t offset = 0) {
			return (T*)&buffer[pointer + offset];
		}

		template<typename T>
		T* ReadPtr(size_t count = 1) {
			if (!CanRead(sizeof(T) * count)) {
				throw std::runtime_error(utils::va("Reading too much at 0x%llx + 0x%llx > 0x%llx", Loc(), sizeof(T) * count, End()));
			}
			T* t = (T*)&buffer[pointer];
			pointer += sizeof(T) * count;
			return t;
		}

		char* ReadString(size_t* len = nullptr, char end = 0) {
			char* str{ Ptr<char>() };
			size_t _len{};
			if (end) {
				while (AbstractByteBuffer::Read<char>() != end) _len++;
			}
			else {
				while (AbstractByteBuffer::Read<char>()) _len++;
			}
			if (len) *len = _len;
			return str;
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