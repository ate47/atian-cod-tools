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
		char padding[1374];
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
		uint32_t unk;
		uint8_t frameRateIndex;
		uint8_t channelCount;
		uint8_t looping;
		SndAssetFormat format;
		byte pad[0x30];
		size_t padLen;

		void Read(core::bytebuffer::AbstractByteBuffer& buffer) {
			// XHash
			size_t loc{ buffer.Loc() };
			id = buffer.Read<uint64_t>();
			buffer.Skip<uint64_t>();
			offset = buffer.Read<uint64_t>();
			size = buffer.Read<uint32_t>();
			frameCount = buffer.Read<uint32_t>();
			order = buffer.Read<uint32_t>();
			unk = buffer.Read<uint32_t>();
			frameRateIndex = buffer.Read<uint8_t>();
			channelCount = buffer.Read<uint8_t>();
			looping = buffer.Read<uint8_t>();
			format = buffer.Read<SndAssetFormat>();

			padLen = sizeof(pad) - (buffer.Loc() - loc);
			buffer.Read(pad, padLen);
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
		LOG_INFO("entrySize : 0x{:x}", header.entrySize);
		LOG_INFO("checksumSize : 0x{:x}", header.checksumSize);
		LOG_INFO("dependencySize : 0x{:x}", header.dependencySize);
		LOG_INFO("entryCount : 0x{:x}", header.entryCount);
		LOG_INFO("dependencyCount : 0x{:x}", header.dependencyCount);
		LOG_INFO("fileSize : 0x{:x}", header.fileSize);
		LOG_INFO("entryOffset : 0x{:x}", header.entryOffset);
		LOG_INFO("checksumOffset : 0x{:x}", header.checksumOffset);

		if (header.entryCount) {
			if (header.entrySize != 0x30) {
				LOG_ERROR("UNKNOWN ENTRY SIZE"); // bo3 was with x32, bo4 is with xhash
				return tool::BASIC_ERROR;
			}
			SndAssetBankEntryBo4 entry;
			reader.Goto(header.entryOffset);

			for (size_t i = 0; i < header.entryCount; i++) {
				entry.Read(reader);

				const char* name{ hashutils::ExtractPtr(entry.id) };
				if (!name) name = utils::va("hashed/snd/file_%llx", entry.id);

				std::filesystem::path of{ std::filesystem::path{"output_ff/snd/"} / std::format("{}{}", name, SndAssetFormatExt(entry.format)) };

				LOG_INFO(
					"dump {}"
					" - {} off:0x{:x}+0x{:x}:0x{:x}"
					" fc:0x{:x} or:0x{:x} fri:0x{:x} cc:0x{:x} lp:0x{:x} uk:0x{:x}"
				    "  {}"
					,
					of.string(),
					SndAssetFormatName(entry.format),
					entry.offset, entry.size, entry.offset + entry.size,
					entry.frameCount, entry.order, entry.frameRateIndex, entry.channelCount, entry.looping, entry.unk,

					utils::data::AsHex(entry.pad, entry.padLen)
				);

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