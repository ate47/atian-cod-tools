#include <includes.hpp>
#include <core/bytebuffer.hpp>
namespace {
	enum RadianKeysType : byte {
		RKT_UNK0 = 0,
		RKT_UNK1 = 1,
		RKT_STRING = 2,
		RKT_UNK3 = 3,
		RKT_VECTOR = 4,
		RKT_UNK5 = 5,
		RKT_FLOAT = 6,
		RKT_INTEGER = 7,
		RKT_UNK8 = 8,
		RKT_HASH_LOCALIZED = 9,
	};

	const char* RadianKeysTypeName(RadianKeysType type) {
		switch (type) {
		case RKT_STRING: return "string";
		case RKT_VECTOR: return "vector";
		case RKT_FLOAT: return "float";
		case RKT_INTEGER: return "int";
		case RKT_HASH_LOCALIZED: return "localized";
		default: return utils::va("unk%d", type);
		}
	}

	int bo6_radiant_keys(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;
		std::vector<byte> data{};
		if (!utils::ReadFile(argv[2], data)) {
			LOG_ERROR("Can't read {}", argv[2]);
			return tool::BASIC_ERROR;
		}
		std::filesystem::path out;

		if (tool::NotEnoughParam(argc, 2)) {
			out = argv[2];
			out.replace_extension(".dec.txt");
		}
		else {
			out = argv[3];
		}

		utils::OutFileCE os{ out };
		if (!os) {
			LOG_ERROR("Can't open {}", argv[3]);
			return tool::BASIC_ERROR;
		}
		core::bytebuffer::ByteBuffer buffer{ data };

		while (buffer.CanRead(sizeof(uint64_t))) {
			uint64_t key{ buffer.Read<uint64_t>() };

			if (!key) break;

			RadianKeysType type{ buffer.Read<RadianKeysType>() };

			os << std::setw(10) << std::setfill(' ') << std::left << RadianKeysTypeName(type) << " " << hashutils::ExtractTmp("hash", key) << "\n";
		}

		LOG_INFO("Dump into {}", out.string());

		return tool::OK;
	}


	int lua_file_delta(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 3)) {
			return tool::BAD_USAGE;
		}
		std::vector<byte> f1{};
		std::vector<byte> f2{};
		size_t offset{ std::strtoull(argv[4], nullptr, 10) };
		if (!utils::ReadFile(argv[2], f1) || !utils::ReadFile(argv[3], f2)) {
			LOG_ERROR("Can't read files");
			return tool::BASIC_ERROR;
		}

		size_t max{ std::max<size_t>(f1.size(), f2.size()) };

		std::map<byte, std::unordered_set<byte>> diff{};

		for (size_t i = offset; i < max; i++) {
			if (f1[i] != f2[i]) {
				diff[f1[i]].insert(f2[i]);
			}
		}

		for (auto& [old, news] : diff) {
			std::stringstream ss{};

			for (byte nw : news) {
				ss << " " << std::dec << (int)nw;
			}

			LOG_INFO("{} ->{}", (int)old, ss.str());
		}

		


		return tool::OK;
	}

	ADD_TOOL(bo6_radiant_keys, "bo6", " [keys] [out]", "Read bo6 radiant keys", bo6_radiant_keys);
	ADD_TOOL(lua_file_delta, "dev", " [a1] [a2] [offset]", "Lua file delta", lua_file_delta);
}