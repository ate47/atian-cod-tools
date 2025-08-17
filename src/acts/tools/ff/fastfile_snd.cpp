#include <includes.hpp>
#include <core/bytebuffer_file.hpp>
#include <tools/utils/data_utils.hpp>

namespace {
	constexpr uint32_t MAGIC = 0x23585532;
	enum SndAssetBankVersion : uint32_t {
		SNDV15 = 0x15,
	};
	struct SndAssetBankHeader {
		uint32_t magic;
		SndAssetBankVersion version;
		uint32_t entrySize;
		uint32_t checksumSize;
		uint32_t dependencySize;
		uint32_t entryCount;
		uint32_t dependencyCount;
		uint32_t pad32;
		int64_t fileSize;
		int64_t entryOffset;
		int64_t checksumOffset;
		unsigned __int8 checksumChecksum[16];
		char dependencies[512];
		int64_t SourceChecksumOffset;
		int64_t AssetNameOffset;
		char zoneName[64];
		char platform[8];
		char language[2];
		char convertedAssetVersion0[4];
		char padding[1370];
	};
	static_assert(sizeof(SndAssetBankHeader) == 0x800);

	enum SndAssetFormat : byte {
		SND_ASSET_FORMAT_PCMS16 = 0x0,
		SND_ASSET_FORMAT_PCMS24 = 0x1,
		SND_ASSET_FORMAT_PCMS32 = 0x2,
		SND_ASSET_FORMAT_IEEE = 0x3,
		SND_ASSET_FORMAT_XMA4 = 0x4,
		SND_ASSET_FORMAT_MP3 = 0x5,
		SND_ASSET_FORMAT_MSADPCM = 0x6,
		SND_ASSET_FORMAT_WMA = 0x7,
		SND_ASSET_FORMAT_FLAC = 0x8,
		SND_ASSET_FORMAT_WIIUADPCM = 0x9,
		SND_ASSET_FORMAT_MPC = 0xA,
		SND_ASSET_FORMAT_COUNT = 0xB,
	};

	const char* SndAssetFormatNames[]{
		"PCMS16",
		"PCMS24",
		"PCMS32",
		"IEEE",
		"XMA4",
		"MP3",
		"MSADPCM",
		"WMA",
		"FLAC",
		"WIIUADPCM",
		"MPC",
	};
	const char* SndAssetFormatExts[]{
		".pcms16",
		".pcms24",
		".pcms32",
		".ieee",
		".xma4",
		".mp3",
		".msadpcm",
		".wma",
		".flac",
		".wiiuadpcm",
		".mpc",
	};

	const char* SndAssetFormatName(SndAssetFormat format) {
		if (format < ARRAYSIZE(SndAssetFormatNames)) return SndAssetFormatNames[format];
		return utils::va("Invalid:%d", (int)format);
	}

	const char* SndAssetFormatExt(SndAssetFormat format) {
		if (format < ARRAYSIZE(SndAssetFormatExts)) return SndAssetFormatExts[format];
		return ".bin";
	}

	struct SndAssetBankEntry
	{
		uint32_t id;
		uint32_t size;
		uint32_t frameCount;
		uint32_t order;
		uint64_t offset;
		uint8_t frameRateIndex;
		uint8_t channelCount;
		uint8_t looping;
		uint8_t format;
		uint8_t EnvelopeLoudness0;
		uint8_t EnvelopeLoudness1;
		uint8_t EnvelopeLoudness2;
		uint8_t EnvelopeLoudness3;
		uint16_t EnvelopeTime1;
		uint16_t EnvelopeTime2;
	};

	struct SndAssetBankEntryBo4 {
		uint64_t id;
		uint64_t idnull;
		uint64_t offset;
		uint32_t size;
		uint32_t frameCount;
		uint32_t order;
		uint32_t unk24;
		uint8_t frameRateIndex;
		uint8_t channelCount;
		uint8_t looping;
		SndAssetFormat format;
		uint32_t unk3c;

		void Read(core::bytebuffer::AbstractByteBuffer& buffer) {
			// XHash
			id = buffer.Read<uint64_t>(); // 8
			buffer.Skip<uint64_t>(); // 10
			offset = buffer.Read<uint64_t>(); // 18
			size = buffer.Read<uint32_t>(); // 1c
			frameCount = buffer.Read<uint32_t>(); // 20
			order = buffer.Read<uint32_t>(); // 24
			unk24 = buffer.Read<uint32_t>(); // 28
			frameRateIndex = buffer.Read<uint8_t>(); // 29
			channelCount = buffer.Read<uint8_t>(); // 2a
			looping = buffer.Read<uint8_t>(); // 2b
			format = buffer.Read<SndAssetFormat>(); // 2c
			unk3c = buffer.Read<uint32_t>(); // 30
		}
	};



	int ffsndr(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;

		utils::InFileCE is{ argv[2], true, std::ios::binary };
		core::bytebuffer::FileReader reader{ *is };

		SndAssetBankHeader header;
		reader.Read(&header, sizeof(header));

		if (header.magic != MAGIC) {
			LOG_ERROR("Invalid magic");
			return tool::BASIC_ERROR;
		}
		if (header.version != SNDV15) {
			LOG_ERROR("Unknown version 0x{:x}", (int)header.version);
			return tool::BASIC_ERROR;
		}

		if (header.fileSize > (int64_t)reader.Size()) {
			LOG_WARNING("File size too big in header");
		}

		LOG_INFO("trueSize: 0x{:x}", reader.Size());
		LOG_INFO("fileSize : 0x{:x}", header.fileSize);
		LOG_INFO("entrySize : 0x{:x}", header.entrySize);
		LOG_INFO("checksumSize : 0x{:x}", header.checksumSize);
		LOG_INFO("dependencySize : 0x{:x}", header.dependencySize);
		LOG_INFO("entryCount : 0x{:x}", header.entryCount);
		LOG_INFO("dependencyCount : 0x{:x}", header.dependencyCount);
		LOG_INFO("entryOffset : 0x{:x}", header.entryOffset);
		LOG_INFO("checksumOffset : 0x{:x}", header.checksumOffset);
		LOG_INFO("pad32 : 0x{:x}", header.pad32);
		LOG_INFO("SourceChecksumOffset : 0x{:x}", header.SourceChecksumOffset);
		LOG_INFO("AssetNameOffset : 0x{:x}", header.AssetNameOffset);
		LOG_INFO("zoneName : {}", header.zoneName);
		LOG_INFO("platform : {}", header.platform);
		char lang[3]{};
		std::memcpy(lang, header.language, 2);
		LOG_INFO("language : {}", lang);
		LOG_INFO("convertedAssetVersion : 0x{:x}", *(uint32_t*)(&header.convertedAssetVersion0[0]));

		if (!utils::data::IsNulled(header.padding, sizeof(header.padding))) {
			LOG_WARNING("Missing padding");
		}

		// empty??
		
		for (size_t i = 0; i < header.dependencyCount; i++) {
			void* dep{ &header.dependencies[header.dependencySize * i] };
			if (!utils::data::IsNulled(dep, header.dependencySize)) {
				LOG_WARNING("deps[{}] = {}", i, utils::data::AsHex(dep, header.dependencySize));
			}
		
		}
		// AssetNameOffset seem to contain the asset hashed names with a 0x78 padding (0x80), probably remnant from a char name[0x80]
		// SourceChecksumOffset and checksumOffset are 128 bits values for each entries (checksumSize?)


		if (header.entryCount) {
			if (header.entrySize != 0x30) {
				LOG_ERROR("UNKNOWN ENTRY SIZE"); // bo3 was with x32, bo4 is with xhash
				return tool::BASIC_ERROR;
			}
			SndAssetBankEntryBo4 entry;

			byte checksumbuff[0x100];
			byte srcchecksumbuff[sizeof(checksumbuff)];
			byte assetname[0x80];
			byte emptyAssetname[0x80]{};
			assert(sizeof(checksumbuff) <= header.checksumSize);
			for (size_t i = 0; i < header.entryCount; i++) {
				reader.Goto(header.entryOffset + (i * header.entrySize));
				entry.Read(reader);

				reader.Goto(header.checksumOffset + (i * header.checksumSize));
				reader.Read(&checksumbuff[0], header.checksumSize);

				reader.Goto(header.SourceChecksumOffset + (i * header.checksumSize));
				reader.Read(&srcchecksumbuff[0], header.checksumSize);

				reader.Goto(header.AssetNameOffset + (i * sizeof(assetname)));
				reader.Read(&assetname[0], sizeof(assetname));

				const char* name{ hashutils::ExtractPtr(entry.id) };
				if (!name) name = utils::va("hashed/snd/%s/file_%llx", lang[0] ? lang : "all", entry.id);

				std::filesystem::path of{ std::filesystem::path{"output_ff/snd/"} / std::format("{}{}", name, SndAssetFormatExt(entry.format)) };

				LOG_INFO(
					"dump {} ({})",
					of.string(),
					SndAssetFormatName(entry.format)
				);
				LOG_INFO(
					" - off:0x{:x}+0x{:x}:0x{:x}"
					" fc:0x{:x} or:0x{:x} fri:0x{:x} cc:0x{:x} lp:0x{:x} unk24:0x{:x} unk3c:0x{:x}"
					,
					entry.offset, entry.size, entry.offset + entry.size,
					entry.frameCount, entry.order, entry.frameRateIndex, entry.channelCount, entry.looping, entry.unk24, entry.unk3c
				);
				LOG_INFO(" - checksum f:{} / s:{}",
					utils::data::ArrayAsString<byte>(checksumbuff, header.checksumSize, "", "", "", [](const byte& b) -> std::string { return std::format("{:02x}", (int)b); }),
					utils::data::ArrayAsString<byte>(srcchecksumbuff, header.checksumSize, "", "", "", [](const byte& b) -> std::string { return std::format("{:02x}", (int)b); })
				);
				if (std::memcmp(&emptyAssetname[8], &assetname[8], sizeof(emptyAssetname) - 8) || *(uint64_t*)assetname != entry.id) {
					LOG_WARNING(" - assetname {} / {}",
						utils::data::ArrayAsString<byte>(assetname, sizeof(assetname), "", "", "", [](const byte& b) -> std::string { return std::format("{:02x}", (int)b); }),
						hashutils::ExtractTmp("hash", *(uint64_t*)assetname)
					);
				}

				reader.PushLocation();
				reader.Goto(entry.offset);
				std::unique_ptr<byte[]> tmp{ reader.ReadArray<byte>(entry.size) };
				std::filesystem::create_directories(of.parent_path());
				if (!utils::WriteFile(of, tmp.get(), entry.size)) {
					LOG_ERROR("Can't write file");
				}
				reader.PopLocation();
			}
		}

		return tool::OK;
	}

	ADD_TOOL(ffsndr, "fastfile", " [file]", "read snd file", ffsndr);
} 