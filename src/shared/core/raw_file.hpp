#pragma once
#include <utils/utils.hpp>
#include "bytebuffer.hpp"

namespace core::raw_file {
	constexpr uint64_t MAGIC = 0x667273746361;
	enum DataType : byte {
		RFDT_END = 0,
		RFDT_STRING,
		RFDT_STRING_ENCRYPTED,
		RFDT_HASH,
		RFDT_COUNT
	};

	struct RFHash {
		uint64_t v;
	};

	class RawFileWriter {
		std::vector<byte> data{};
		size_t lastData{};

	public:
		RawFileWriter() {
			utils::WriteValue(data, MAGIC);
			lastData = utils::WriteValue(data, RFDT_END);
		}

		RawFileWriter& WriteDT(DataType dt) {
			if (!data[lastData]) {
				data[lastData] = dt;
			}
			else {
				if (data[lastData] == RFDT_STRING) {
					data.push_back(0); // end string
				}
				lastData = utils::WriteValue(data, dt);
			}
			return *this;
		}

		RawFileWriter& WriteString(const char* str, size_t len = std::string::npos) {
			if (len == std::string::npos) len = std::strlen(str);

			if (data[lastData] == RFDT_STRING) {
				*reinterpret_cast<uint32_t*>(&data[lastData + 1]) += (uint32_t)len;
				data.insert(data.end(), str, str + len);
			}
			else {
				WriteDT(RFDT_STRING);
				utils::WriteValue<uint32_t>(data, (uint32_t)len + 1);
				data.insert(data.end(), str, str + len);
			}
			return *this;
		}

		inline RawFileWriter& WriteString(const std::string& str) {
			return WriteString(str.data(), str.size());
		}

		RawFileWriter& WriteEncryptedString(const char* str) {
			size_t len{ std::strlen(str) + 0x10 }; // add to compute data


			WriteDT(RFDT_STRING_ENCRYPTED);
			utils::WriteValue<uint32_t>(data, (uint32_t)len);
			data.insert(data.end(), str, str + len);
			return *this;
		}

		RawFileWriter& WriteHash(uint64_t hash) {
			WriteDT(RFDT_HASH);
			utils::WriteValue(data, hash);
			return *this;
		}

		template<typename T>
		RawFileWriter& operator<<(const T& t) {
			std::stringstream ss{};
			ss << t;
			std::string sss{ ss.str() };
			return WriteString(sss.data());
		}

		template<>
		RawFileWriter& operator<<(const RFHash& t) {
			return WriteHash(t.v);
		}
		
		const std::vector<byte>& Build() {
			WriteDT(RFDT_END);
			return data;
		}

	};

	class RawFileReader {
		bytebuffer::ByteBuffer& buffer;
		DataType curr{};
		// optional functions
		char* (*DecryptString)(char* str);
		const char* (*FindHash)(uint64_t hash);
	public:
		RawFileReader(bytebuffer::ByteBuffer& buffer, char* (*DecryptString)(char* str) = nullptr, const char* (*FindHash)(uint64_t hash) = nullptr) 
			: buffer(buffer), DecryptString(DecryptString), FindHash(FindHash) {
			if (buffer.Read<uint64_t>() != MAGIC) {
				throw std::runtime_error("Can't read RawFile: bad magic");
			}
		}

		DataType GetType() {
			return curr;
		}

		bool GotoNextField() {
			switch (curr) {
			case RFDT_END: 
				break;
			case RFDT_HASH:
				buffer.Skip<uint64_t>();
				break;
			case RFDT_STRING:
			case RFDT_STRING_ENCRYPTED: {
				uint32_t len{ buffer.Read<uint32_t>() };
				buffer.Skip(len);
				break;
			}
			default:
				throw std::runtime_error("Can't read RawFile: invalid field type");
			}
			curr = buffer.Read<DataType>();
			return curr;
		}

		uint32_t GetStringSize() {
			return *buffer.Ptr<uint32_t>();
		}

		uint64_t GetHash() {
			return *buffer.Ptr<uint64_t>();
		}

		char* GetStringRef() {
			return buffer.Ptr<char>(sizeof(uint32_t));
		}

		const char* GetString() {
			switch (curr) {
			case RFDT_HASH:
				if (FindHash) {
					return FindHash(GetHash());
				}
				return utils::va("hash_%llx", GetHash());
			case RFDT_STRING:
				return GetStringRef();
			case RFDT_STRING_ENCRYPTED:
				if (DecryptString) {
					return DecryptString(GetStringRef());
				}
				return GetStringRef();
			case RFDT_END:
				return "";
			default:
				return "unktype";
			}
		}

		void ParseAllFields() {
			while (GotoNextField()) {
				GetString(); // decrypt str
			}
		}

		void ReadAll(std::ostream& os) {
			while (GotoNextField()) {
				os << GetString();
			}
		}

		std::string ReadAll() {
			std::stringstream ss{};
			ReadAll(ss);
			return ss.str();
		}
	};

}