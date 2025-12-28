#include <includes.hpp>
#include <core/bytebuffer.hpp>
#include <utils/data_utils.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <sha256.h>

namespace {
	struct XFile
	{
		uint8_t magic[8];
		uint32_t version;
		uint8_t server;
		byte compression;
		uint8_t platform;
		uint8_t encrypted;
		uint64_t timestamp;
		uint32_t changelist;
		uint32_t archiveChecksum[4];
		char builder[32];
		uint32_t metaVersion;
		char mergeFastfile[64];
		char missionFastFiles[16][64];
		uint64_t size;
		uint64_t externalSize;
		uint64_t memMappedOffset;
		uint64_t blockSize[9];
		uint64_t unk4f0;
		uint64_t unk4f8;
		uint64_t unk500;
		uint64_t unk508;
		uint64_t unk510s;
		uint64_t unk518s;
		uint64_t unk520pa;
		byte pad0[392];
		uint8_t unkSign[64];
		char fastfileName[64];
		uint8_t signature[256];
		uint8_t aesIV[16];
	};



	int ffbo4testchecksum(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;

		std::vector<byte> file{ utils::ReadFile<std::vector<byte>>(argv[2]) };

		core::bytebuffer::ByteBuffer reader{ file };

		XFile* header{ reader.ReadPtr<XFile>() };

		byte* signature{ header->signature };

		LOG_INFO("signaturerr: {}", utils::data::ArrayAsString<byte>(header->signature, sizeof(header->signature), "", "", "", [](const byte& b) { return std::format("{:02x}", (int)b); }));
		LOG_INFO("signaturunk: {}", utils::data::ArrayAsString<byte>(header->unkSign, sizeof(header->unkSign), "", "", "", [](const byte& b) { return std::format("{:02x}", (int)b); }));

		SHA256 sha256[4]{};

		size_t offset{};
		size_t chunks{};
		while (offset < header->size) {
			size_t loc{ reader.Loc() };

			fastfile::DBStreamHeader* block{ reader.ReadPtr<fastfile::DBStreamHeader>() };

			if (block->offset != loc) {
				LOG_ERROR("bad block position: 0x{:x} != 0x{:x}", loc, block->offset);
				break;
			}

			LOG_INFO("#0x{:x} alignedSize=0x{:x} compressedSize=0x{:x} uncompressedSize=0x{:x} offset=0x{:x}", chunks, block->alignedSize, block->compressedSize, block->uncompressedSize, block->offset);

			if (!block->uncompressedSize) {
				reader.Align(0x800000);
				continue;
			}
			offset += block->uncompressedSize;

			byte* blockBuff{ reader.ReadPtr<byte>(block->alignedSize) };

			sha256[chunks % 4].add(blockBuff, block->compressedSize);
			byte buff[0x100]{};
			sha256[chunks % 4].getHash(buff);
			LOG_INFO("signature{}: {}", chunks % 4, utils::data::ArrayAsString<byte>(buff, 0x100, "", "", "", [](const byte& b) { return std::format("{:02x}", (int)b); }));
			chunks++;
		}
		LOG_INFO("chunk: {}", chunks);


		return tool::OK;
	}


	ADD_TOOL(ffbo4testchecksum, "dev", " [ff]", "test bo4 ff hash", ffbo4testchecksum);
}