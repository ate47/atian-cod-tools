#include <includes.hpp>
#include <utils/compress_utils.hpp>
#include <core/bytebuffer.hpp>
#include <deps/oodle.hpp>
#include <utils/data_utils.hpp>

namespace {
	constexpr uint32_t OODLE_COMPRESS = 0x444F4F;

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

	uint32_t HashString(const char* str) {
		uint32_t h{ 5381 };
		while (*str) {
			h = *(str++) + 31 * h;
		}
		return h;
	}
	uint32_t HashString(const std::string& str) { return HashString(str.data()); }


	std::string ReadSizedString(core::bytebuffer::ByteBuffer& reader) {
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


			std::string indexFile{ ReadStringVal() };


			uint16_t numEntries{ reader.ReadNumber<uint16_t>() };
			os
				<< std::hex
				<< "u1=" << u1 << "\n"
				<< "numEntries=" << numEntries << "\n"
				<< "indexFile=" << indexFile << "\n"
				<< "\n"
				;

			for (size_t i = 0; i < numEntries; i++) {
				std::string v{ ReadStringVal() };
				os << std::hex << "#x" << i << " - " << v << "\n";
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
			size_t cc{};
			while (reader.CanRead(sizeof(uint32_t))) {
				uint32_t count{ reader.ReadNumber<uint32_t>() };
				UnkDeps* unk{ reader.ReadPtr<UnkDeps>(count) };
				os << "BLOCK #x" << std::hex << (block++) << "\n";
				for (size_t i = 0; i < count; i++) {
					os << "#x" << i << " - " << unk[i].unk4 << "/" << unk[i].unk0 << "\n";
					cc++;
				}
				os << "\n";

			}
			LOG_INFO("end count: {} (0x{:x})", cc, cc);
			break;
		}
		case IDT_MASTER_RES: {
			reader.SetEndian(std::endian::big);
			uint32_t magic{ reader.ReadNumber<uint32_t>() };

			if ((magic & 0xFFFFFF) != 0x534552) {
				LOG_ERROR("Bad magic");
				return tool::BASIC_ERROR;
			}
			uint32_t dataSize{ reader.ReadNumber<uint32_t>() };
			reader.Skip(0x18);
			if (reader.Remaining() != dataSize) {
				LOG_WARNING("bad datasize 0x{:x} != 0x{:x}", reader.Remaining(), dataSize);
			}
			uint32_t numEntries{ reader.ReadNumber<uint32_t>() };

			reader.SetEndian(std::endian::little);

			for (size_t i = 0; i < numEntries; i++) {
				uint32_t id{ reader.ReadNumber<uint32_t>() };
				std::string kty{ ReadStringVal() };
				std::string filename{ ReadStringVal() };
				std::string path{ ReadStringVal() };

				uint32_t offset{ reader.ReadNumber<uint32_t>() };
				uint32_t u1{ reader.ReadNumber<uint32_t>() }; // empty
				uint32_t decompressedSize{ reader.ReadNumber<uint32_t>() };
				uint32_t compressedSize{ reader.ReadNumber<uint32_t>() };
				uint32_t u4{ reader.ReadNumber<uint32_t>() }; // empty
				uint32_t u5{ reader.ReadNumber<uint32_t>() }; // empty
				byte u6{ reader.ReadNumber<byte>() }; // empty
				byte u7{ reader.ReadNumber<byte>() };
				byte u8{ reader.ReadNumber<byte>() }; // empty
				byte u9{ reader.ReadNumber<byte>() }; // empty
				byte fileId{ reader.ReadNumber<byte>() };
				byte u10{ reader.ReadNumber<byte>() }; // empty

				if (u1) LOG_INFO("u1 0x{:x}", id);
				if (u4) LOG_INFO("u4 0x{:x}", id);
				if (u5) LOG_INFO("u5 0x{:x}", id);
				if (u6) LOG_INFO("u6 0x{:x}", id);
				if (u8) LOG_INFO("u8 0x{:x}", id);
				if (u9) LOG_INFO("u9 0x{:x}", id);
				if (u10) LOG_INFO("u10 0x{:x}", id);

				os << std::hex
					<< "0x" << id
					<< ",off:0x" << offset
					<< ",ds:0x" << decompressedSize
					<< ",cs:0x" << compressedSize
					<< ",fileId:0x" << (int)fileId
					<< ",u7:" << (u7 ? "true" : "false")
					<< ",u8:0x" << (int)u8
					<< ",u10:0x" << (int)u10
					;

				os
					<< "," << kty << "," << filename << "," << path << "\n";
			}
			break;
		}
		default:
			LOG_ERROR("Type not implemented");
			break;
		}

		if (reader.Remaining()) {
			LOG_WARNING("Missing 0x{:x}", reader.Remaining());
		}


		return tool::OK;
	}

	int dtl_extract(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) {
			return tool::BAD_USAGE;
		}

		std::filesystem::path in{ argv[2] };
		std::filesystem::path out{ argv[3] };
		const char* typeFilter{ tool::NotEnoughParam(argc, 3) || !_strcmpi(argv[4], "all") ? nullptr : argv[4] };

		std::filesystem::path masterIndex{ in / "master.index" };
		std::filesystem::path masterResourcesIndex;

		std::vector<std::string> files{};
		{
			LOG_INFO("load master index... {}", masterIndex.string());
			std::vector<byte> buff{ utils::ReadFile<std::vector<byte>>(masterIndex) };
			core::bytebuffer::ByteBuffer reader{ buff };

reader.SetEndian(std::endian::big);
uint32_t magic{ reader.ReadNumber<uint32_t>() };

if ((magic & 0xFFFFFF) != 0x534552) {
	LOG_ERROR("Bad magic");
	return tool::BASIC_ERROR;
}

uint16_t u1{ reader.ReadNumber<uint16_t>() };


std::string indexFile{ ReadSizedString(reader) };
masterResourcesIndex = in / indexFile;


uint16_t numEntries{ reader.ReadNumber<uint16_t>() };
files.reserve(numEntries);

for (size_t i = 0; i < numEntries; i++) {
	files.emplace_back(ReadSizedString(reader));
}
		}
		LOG_INFO("loaded with {} entries", files.size());

		struct ResourceFile {
			uint32_t id;
			std::string path;
			uint32_t offset;
			uint32_t decompressedSize;
			uint32_t compressedSize;
		};

		std::map<size_t, std::vector<ResourceFile>> locFiles{};
		{
			LOG_INFO("load master resource index... {}", masterResourcesIndex.string());
			std::vector<byte> buff{ utils::ReadFile<std::vector<byte>>(masterResourcesIndex) };
			core::bytebuffer::ByteBuffer reader{ buff };

			reader.SetEndian(std::endian::big);
			uint32_t magic{ reader.ReadNumber<uint32_t>() };

			if ((magic & 0xFFFFFF) != 0x534552) {
				LOG_ERROR("Bad magic");
				return tool::BASIC_ERROR;
			}
			uint32_t dataSize{ reader.ReadNumber<uint32_t>() };
			reader.Skip(0x18);
			if (reader.Remaining() != dataSize) {
				LOG_WARNING("bad datasize 0x{:x} != 0x{:x}", reader.Remaining(), dataSize);
			}
			uint32_t numEntries{ reader.ReadNumber<uint32_t>() };

			reader.SetEndian(std::endian::little);

			size_t loaded{};
			for (size_t i = 0; i < numEntries; i++) {
				uint32_t id{ reader.ReadNumber<uint32_t>() };
				ResourceFile rf{};
				std::string kty{ ReadSizedString(reader) };
				std::string filename{ ReadSizedString(reader) };
				rf.path = ReadSizedString(reader);

				rf.offset = reader.ReadNumber<uint32_t>();
				uint32_t u1{ reader.ReadNumber<uint32_t>() }; // empty
				rf.decompressedSize = reader.ReadNumber<uint32_t>();
				rf.compressedSize = reader.ReadNumber<uint32_t>();
				uint32_t u4{ reader.ReadNumber<uint32_t>() }; // empty
				uint32_t u5{ reader.ReadNumber<uint32_t>() }; // empty
				byte u6{ reader.ReadNumber<byte>() }; // empty
				byte u7{ reader.ReadNumber<byte>() };
				byte u8{ reader.ReadNumber<byte>() }; // empty
				byte u9{ reader.ReadNumber<byte>() }; // empty
				byte fileId{ reader.ReadNumber<byte>() };
				byte u10{ reader.ReadNumber<byte>() }; // empty

				if (typeFilter && _strcmpi(typeFilter, kty.data())) {
					continue;
				}

				if (fileId >= files.size()) {
					LOG_ERROR("found invalid file id {} >= {}", fileId, files.size());
					continue;
				}
				locFiles[fileId].push_back(rf);
				loaded++;
			}
			if (!loaded) {
				LOG_ERROR("Can't find any file with type {}", typeFilter);
				return tool::BASIC_ERROR;
			}
			LOG_INFO("loaded {} files", loaded);

			for (auto& [fileId, resFiles] : locFiles) {
				std::filesystem::path dataFile{ in / files[fileId] };
				LOG_INFO("Dumping {} entries from data file {}...", resFiles.size(), dataFile.string());
				std::vector<byte> buff{ utils::ReadFile<std::vector<byte>>(dataFile) };
				for (ResourceFile& rf : resFiles) {
					std::filesystem::path of{ out / rf.path };
					if ((size_t)rf.offset + rf.compressedSize > buff.size()) {
						LOG_ERROR("File {} out of bound", rf.path);
						continue;
					}
					byte* data{ &buff[rf.offset] };
					bool oodleCompressed{ rf.compressedSize > 12 && !std::memcmp("OOD", data, 3) && (data[4] & 0xF) != 3 };
					std::filesystem::create_directories(of.parent_path());
					if (oodleCompressed) {
						std::unique_ptr<byte[]> unc{ std::make_unique<byte[]>(rf.decompressedSize) };
						if (!utils::compress::Decompress(utils::compress::COMP_OODLE, unc.get(), rf.decompressedSize, data + 12, rf.compressedSize - 12)) {
							LOG_ERROR("Can't decompress {}", rf.path);
							continue;
						}
						utils::WriteFile(of, unc.get(), rf.decompressedSize);
					}
					else {
						utils::WriteFile(of, data, rf.compressedSize);
					}


					// rf.offset
					// rf.decompressedSize
					// rf.compressedSize
					LOG_INFO("Dump {}{}", of.string(), oodleCompressed ? " (decompressed)" : "");
				}
			}
		}

	
		return tool::OK;
	}

	ADD_TOOL(dtl_idxd, "deathloop", " [type] [in] [out]", "decompress deathloop index file, type=base,master,master_res", dtl_idxd);
	ADD_TOOL(dtl_extract, "deathloop", " [path] [out] [type=all]", "decompress deathloop files", dtl_extract);
}