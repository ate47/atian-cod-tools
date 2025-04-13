#pragma once
#include "bytebuffer_abstract.hpp"

namespace core::bytebuffer {
	class FileReader : public AbstractByteBuffer {
		std::istream& is;
		size_t fileend;
		size_t filestart;
		size_t location;

	public:
		FileReader(std::istream& is, size_t start = 0, size_t fileend = std::string::npos) : is(is), filestart(start), location(start) {
			if (fileend == std::string::npos) {
				is.seekg(0, std::ios::end);
				this->fileend = is.tellg();
				is.seekg(start, std::ios::beg);
			}
			else {
				this->fileend = fileend;
			}
			ValidateState();
		}

		void ValidateState() {
			if (!is) throw std::runtime_error("stream error");
		}

		bool CanRead(size_t size) const override {
			return location + size <= fileend;
		}

		size_t Length() const override {
			return fileend;
		}

		size_t Remaining() const override {
			return fileend - location;
		}
		size_t Loc() const override {
			return location;
		}

		void ReadImpl(void* to, size_t size) override {
			if (!CanRead(size)) {
				throw std::runtime_error(utils::va("Reading pointer too much at 0x%llx + 0x%llx > 0x%llx", location, size, fileend));
			}

			is.read((char*)to, size);

			ValidateState();

			location += size;
		}

		void Goto(size_t loc) override {
			size_t trueloc{ filestart + loc };
			if (trueloc > fileend) {
				throw std::runtime_error(utils::va("Goto after end 0x%llx + 0x%llx > 0x%llx", loc, fileend));
			}
			if (trueloc == fileend) {
				is.seekg(0, std::ios::end);
			}
			else {
				is.seekg(filestart + loc, std::ios::beg);
			}

			ValidateState();

			location = loc;
		}
	
	};

}