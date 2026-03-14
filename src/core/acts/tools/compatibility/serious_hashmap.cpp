#include <includes.hpp>
#include <core/bytebuffer.hpp>

namespace {
	enum SeriousHashMapType {
		SHMT_UNKNOWN = 0,
		SHMT_T7,
		SHMT_FNV,
		SHMT_T8,
	};
	const char* SeriousHashMapTypeNames[]{
		"unknown",
		"t7",
		"fnv",
		"t8",
	};
	SeriousHashMapType SeriousHashMapTypeOf(const char* name) {
		for (size_t i = 1; i < ARRAYSIZE(SeriousHashMapTypeNames); i++) {
			if (!_strcmpi(SeriousHashMapTypeNames[i], name)) {
				return (SeriousHashMapType)i;
			}
		}
		return SHMT_UNKNOWN;
	}
	const char* SeriousHashMapTypeName(SeriousHashMapType type) {
		if (type < 0 || type >= ARRAYSIZE(SeriousHashMapTypeNames)) {
			return SeriousHashMapTypeNames[0];
		}
		return SeriousHashMapTypeNames[type];
	}



	int serious_hashmap_read(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 3)) return tool::BAD_USAGE;

		SeriousHashMapType type{ SeriousHashMapTypeOf(argv[2]) };
		std::filesystem::path input{ argv[3] };
		std::filesystem::path output{ argv[4] };

		std::string buff{};
		if (!utils::ReadFile(input, buff)) {
			LOG_ERROR("Can't dump read {}", input.string());
			return tool::BASIC_ERROR;
		}

		core::bytebuffer::ByteBuffer reader{ buff };

		std::map<uint64_t, std::string> hashmap{};

		switch (type) {
		case SHMT_T7: {
			while (!reader.End()) {
				uint32_t hash{ reader.Read<uint32_t>() };
				std::string val{ reader.ReadSizedString<byte>() };

				hashmap[hash] = val;
			}
			break;
		}
		case SHMT_FNV: {
			while (!reader.End()) {
				uint64_t hash{ reader.Read<uint64_t>() };
				std::string val{ reader.ReadSizedString<byte>() };

				hashmap[hash] = val;
			}
			break;
		}
		case SHMT_T8: {
			while (!reader.End()) {
				uint32_t hash32{ reader.Read<uint32_t>() };
				uint64_t hash64{ reader.Read<uint64_t>() };
				std::string val{ reader.ReadSizedString<byte>() };

				hashmap[hash32] = val;
				hashmap[hash64] = val;
			}
			break;
		}
		default:
			LOG_ERROR("Invalid type {}", argv[2]);
			break;
		}

		LOG_INFO("{} hash(es) loaded", hashmap.size());
		{
			std::ofstream os{ output };
			if (!os) {
				LOG_ERROR("Can't open {}", output.string());
				return tool::BASIC_ERROR;
			}
			utils::CloseEnd osce{ os };

			for (auto& [key, val] : hashmap) {
				os << std::hex << key << "," << val << "\n";
			}
		}
		LOG_INFO("Dump into {}", output.string());

		return tool::OK;
	}

	int serious_hashmap_write(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 3)) return tool::BAD_USAGE;

		SeriousHashMapType type{ SeriousHashMapTypeOf(argv[2]) };
		std::filesystem::path input{ argv[3] };
		std::filesystem::path output{ argv[4] };

		std::ifstream is{ input };
		if (!is) {
			LOG_ERROR("Can't read {}", input.string());
			return tool::BASIC_ERROR;
		}

		utils::CloseEnd isce{ is };
		std::string line;

		auto GetNextLine = [&is, &line]() -> const char* {
			if (!is || !std::getline(is, line)) {
				return nullptr;
			}

			return line.c_str();
		};

		std::vector<byte> writer{};

		const char* tmp;
		switch (type) {
		case SHMT_T7: {
			while (tmp = GetNextLine()) {
				utils::WriteValue<uint32_t>(writer, hash::HashT7(tmp));
				utils::WriteSizedString<byte>(writer, tmp);
			}
			break;
		}
		case SHMT_FNV: {
			while (tmp = GetNextLine()) {
				utils::WriteValue<uint64_t>(writer, hash::Hash64(tmp));
				utils::WriteSizedString<byte>(writer, tmp);
			}
			break;
		}
		case SHMT_T8: {
			while (tmp = GetNextLine()) {
				utils::WriteValue<uint32_t>(writer, hash::HashT89Scr(tmp));
				utils::WriteValue<uint64_t>(writer, hash::Hash64(tmp));
				utils::WriteSizedString<byte>(writer, tmp);
			}
			break;
		}
		default:
			LOG_ERROR("Invalid type {}", argv[2]);
			break;
		}

		if (!utils::WriteFile(output, writer)) {
			LOG_ERROR("Can't write to {}", output.string());
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Dump into {}", output.string());

		return tool::OK;
	}


	ADD_TOOL(serious_hashmap_read, "compatibility", " [type] [file] [out]", "Dump hashmap file, type = t7/fnv/t8", serious_hashmap_read);
	ADD_TOOL(serious_hashmap_write, "compatibility", " [type] [file] [out]", "Write hashmap file, type = t7/fnv/t8", serious_hashmap_write);
}