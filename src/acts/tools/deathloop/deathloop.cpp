#include <includes.hpp>
#include <utils/compress_utils.hpp>
#include <core/bytebuffer.hpp>
#include <deps/oodle.hpp>

namespace {
	enum DTL_CompressType : uint32_t {
		DTL_CT_OODLE = 0x444F4F,
	};

	int dtl_resd(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) {
			return tool::BAD_USAGE;
		}

		std::filesystem::path in{ argv[2] };
		std::filesystem::path out{ argv[3] };

		std::vector<byte> buff{ utils::ReadFile<std::vector<byte>>(in) };
		core::bytebuffer::ByteBuffer reader{ buff };

		if (reader.Read<uint32_t>() != 0x52455304) {
			LOG_ERROR("Bad magic");
			return tool::BASIC_ERROR;
		}

		deps::oodle::Oodle& oodle{ deps::oodle::GetInstance() };

		uint32_t compression{ reader.Read<uint32_t>() };
		uint32_t decLen{ reader.Read<uint32_t>() };

		std::unique_ptr<byte[]> data{ std::make_unique<byte[]>(decLen) };

		if (!oodle.Decompress(reader.Ptr(), (uint32_t)reader.Remaining(), data.get(), decLen, deps::oodle::OODLE_FS_NO, deps::oodle::OODLE_CRC_YES, deps::oodle::OODLE_VERB_LOTS)) {
			LOG_ERROR("Bad decomp");
			return tool::BASIC_ERROR;
		}

		//std::vector<byte> uc{ oodle.Decompress(reader.Ptr(), reader.Remaining()) };
		//LOG_INFO("end size: 0x{:x}", uc.size());



		return tool::OK;
	}

	enum IdxType {
		IDT_UNK = 0,
		IDT_BASE,
		IDT_MASTER,
		IDT_MASTER_RES,
		IDT_RSC_DEPS,
		IDT_COUNT
	};
	const char* IdxTypeNames[]{
		"unknown",
		"base",
		"master",
		"master_res",
		"rsc_deps"
	};
	IdxType GetIdxType(const char* cfg) {
		for (size_t i = 0; i < ARRAYSIZE(IdxTypeNames); i++) {
			if (!_strcmpi(cfg, IdxTypeNames[i])) {
				return (IdxType)i;
			}
		}
		return IdxType::IDT_UNK;
	}

	uint64_t HashString(const char* str) {
		uint64_t h{ 5381 };
		while (*str) {
			h = *(str++) + 31 * h;
		}
		return h;
	}
	uint64_t HashString(const std::string& str) { return HashString(str.data()); }

	int dtl_idxd(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 3)) {
			return tool::BAD_USAGE;
		}

		IdxType type{ GetIdxType(argv[2]) };
		std::filesystem::path in{ argv[3] };
		std::filesystem::path dir{ argv[4] };
		std::filesystem::path out{ dir / in.filename() };
		std::filesystem::create_directories(dir);
		out.replace_extension(".index.txt");

		if (!type) {
			LOG_ERROR("Invalid type");
			return tool::BAD_USAGE;
		}

		std::vector<byte> buff{ utils::ReadFile<std::vector<byte>>(in) };
		core::bytebuffer::ByteBuffer reader{ buff };

		auto ReadStringVal = [&reader]() -> std::string {
			reader.PushEndian();
			reader.SetEndian(std::endian::little);
			int32_t len{ reader.ReadNumber<int32_t>() };
			reader.PopEndian();

			if (len <= 0) {
				return {};
			}
			std::string v{};
			v.resize(len);
			reader.Read(v.data(), len);
			return v;
		};
		LOG_INFO("dump to {}", out.string());
		utils::OutFileCE os{ out, true };

		switch (type) {
		case IDT_BASE: {
			reader.SetEndian(std::endian::little);
			uint32_t numEntries{ reader.ReadNumber<uint32_t>() };
			LOG_INFO("numEntries={}", numEntries);

			for (size_t i = 0; i < numEntries; i++) {
				std::string v{ ReadStringVal() };
				os << "0x" << std::hex << HashString(v) << "," << v << "\n";
			}
			break;
		}
		case IDT_MASTER: {
			reader.SetEndian(std::endian::big);
			uint32_t magic{ reader.ReadNumber<uint32_t>() };

			if ((magic & 0xFFFFFF) != 0x534552) {
				LOG_ERROR("Bad magic");
				return tool::BASIC_ERROR;
			}

			uint16_t u1{ reader.ReadNumber<uint16_t>() };


			std::string v1{ ReadStringVal() };


			uint16_t numEntries{ reader.ReadNumber<uint16_t>() };
			LOG_INFO("u1={}", u1);
			LOG_INFO("numEntries={}", numEntries);
			LOG_INFO("v1={}", v1);

			for (size_t i = 0; i < numEntries; i++) {
				std::string v{ ReadStringVal() };
				os << "0x" << std::hex << HashString(v) << "," << v << "\n";
			}
			break;
		}
		case IDT_RSC_DEPS: {
			reader.SetEndian(std::endian::little);
			size_t block{};
			struct UnkDeps {
				uint32_t unk0;
				uint32_t unk4;
			};
			while (reader.CanRead(sizeof(uint32_t))) {
				uint32_t count{ reader.ReadNumber<uint32_t>() };
				UnkDeps* unk{ reader.ReadPtr<UnkDeps>(count) };
				os << "BLOCK #x" << std::hex << (block++) << "\n";
				for (size_t i = 0; i < count; i++) {
					os << "#x" << i << " - " << unk[i].unk4 << "/" << unk[i].unk0 << "\n";
				}
				os << "\n";

			}
			break;
		}
		case IDT_MASTER_RES:
		default:
			LOG_ERROR("Type not implemented");
			break;
		}

		if (reader.Remaining()) {
			LOG_WARNING("Missing 0x{:x}", reader.Remaining());
		}


		return tool::OK;
	}

	ADD_TOOL(dtl_resd, "deathloop", " [in] [out]", "decompress deathloop resource file", dtl_resd);
	ADD_TOOL(dtl_idxd, "deathloop", " [type] [in] [out]", "decompress deathloop index file, type=base,master,master_res", dtl_idxd);
}