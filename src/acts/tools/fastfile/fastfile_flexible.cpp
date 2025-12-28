#include <includes.hpp>
#include <core/config.hpp>
#include <core/bytebuffer_file.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/fastfile_flexible.hpp>
#include <utils/data_utils.hpp>
#include <sha256.h>

namespace fastfile::flexible {
	void FlexibleFastFileReader::ReadHeader(core::bytebuffer::ByteBuffer& reader, TAFASecureInfo* secureInfo) {
		uint32_t magic{ reader.Read<uint32_t>() };
		if (!IsFlexibleDataMagic(magic)) {
			throw std::runtime_error(std::format("Invalid magic 0x{:x}", magic));
		}
		bool hasHash{ magic == MAGIC_V2 };
		chunksCount = reader.Read<uint32_t>();

		if (!chunksCount) {
			return;
		}
		if (chunksCount > ARRAYSIZE(chunks)) {
			throw std::runtime_error(std::format("too many flexible data chunks 0x{:x} > 0x{:x}", chunksCount, ARRAYSIZE(chunks)));
		}

		FlexibleFastFileChunk* chunk{ chunks };

		for (size_t i = 0; i < chunksCount; i++, chunk++) {
			size_t loc{ reader.Loc() };
			chunk->type = (SectionType)reader.Read<uint32_t>();
			chunk->size = reader.Read<uint32_t>();
			chunk->buffer = reader.ReadPtr<byte>(chunk->size);

			LOG_TRACE("loaded chunk 0x{:x} of size 0x{:x} at 0x{:x}", (uint32_t)chunk->type, chunk->size, loc);
		}

		if (hasHash) {
			TAFASignature* endData{ reader.ReadPtr<TAFASignature>() };

			if (endData->magic != MAGIC_V2) {
				throw std::runtime_error(std::format("invalid flexible data pre-signature magic: {:x} != {:x}", endData->magic, MAGIC_V2));
			}

			if (!secureInfo) {
				LOG_WARNING("No secure info provided, can't validate flexible header");
				return;
			}

			SHA256 sha256{};

			uint32_t block[2];

			block[0] = magic;
			block[1] = (uint32_t)chunksCount;
			sha256.add(secureInfo->fastfileName, std::strlen(secureInfo->fastfileName));
			sha256.add(block, sizeof(block));

			for (size_t i = 0; i < chunksCount; i++) {
				block[0] = (uint32_t)chunks[i].type;
				block[1] = (uint32_t)chunks[i].size;
				sha256.add(block, sizeof(block));
				sha256.add(chunks[i].buffer, chunks[i].size);
			}

			byte endHash[0x20];
			sha256.getHash(endHash);

			if (std::memcmp(endHash, endData->hash, sizeof(endHash))) {
				throw std::runtime_error(std::format("Invalid hash value: \n{}\n{}", utils::data::AsHex(endHash, sizeof(endHash)), utils::data::AsHex(endData->hash, sizeof(endData->hash))));
			}

			LOG_TRACE("flexible header sha256 hash validated {} at 0x{:x}", utils::data::AsHex(endHash, sizeof(endHash)), reader.Loc());

			// todo: handle signature
		}


	}
	FlexibleFastFileChunk* FlexibleFastFileReader::GetChunk(SectionType type, bool failMissing, size_t checkSize) {
		for (size_t i = 0; i < chunksCount; i++) {
			if (chunks[i].type == type) {
				if (checkSize && chunks[i].size < checkSize) {
					throw std::runtime_error(std::format("Section too small {:x} size:0x{:x} < wanted:0x{:x}", (uint32_t)type, chunks[i].size, checkSize));
				}
				return &chunks[i];
			}
		}

		if (failMissing) {
			throw std::runtime_error(std::format("Missing section type {:x}", (uint32_t)type));
		}
		return nullptr;
	}

	FlexibleFastFileWriter::FlexibleFastFileWriter(std::vector<byte>& data, uint32_t magic) : data(data), magic(magic) {
		utils::Allocate(data, sizeof(uint32_t) * 2); // magic + numBlock
		if (magic == MAGIC_V2) {
			// validation block
			hashOffset = utils::Allocate(data, sizeof(TAFASignature));
		}
	}

	void FlexibleFastFileWriter::AddBlock(SectionType type, const void* ptr, uint32_t len) {
		utils::WriteValue<uint32_t>(data, type);
		utils::WriteValue<uint32_t>(data, len);
		size_t off{ utils::Allocate(data, len) };
		
		if (chunksCount == ARRAYSIZE(chunks)) {
			throw std::runtime_error("too many flexible chunks to write");
		}

		FlexibleWriteChunk& chunk{ chunks[chunksCount++] };
		chunk.type = type;
		chunk.offset = off;
		chunk.buffer = ptr;
		chunk.size = len;
	}

	void FlexibleFastFileWriter::WriteEnd(TAFASecureInfo* secureInfo) {
		uint32_t* header{ (uint32_t*)data.data() };
		// complete header
		header[0] = magic;
		header[1] = (uint32_t)chunksCount;

		// write chunks
		for (size_t i = 0; i < chunksCount; i++) {
			FlexibleWriteChunk& chunk{ chunks[i] };
			std::memcpy(&data[chunk.offset], chunk.buffer, chunk.size);
		}

		if (hashOffset) {
			if (secureInfo) {
				TAFASignature* sig{ (TAFASignature*)&data[hashOffset] };
				sig->magic = magic;
				SHA256 sha256{};
				
				sha256.add(secureInfo->fastfileName, std::strlen(secureInfo->fastfileName));
				sha256.add(header, sizeof(header));

				uint32_t block[2];
				for (size_t i = 0; i < chunksCount; i++) {
					FlexibleWriteChunk& chunk{ chunks[i] };
					block[0] = (uint32_t)chunk.type;
					block[1] = (uint32_t)chunk.size;
					sha256.add(block, sizeof(block));
					sha256.add(chunk.buffer, chunk.size);
				}

				sha256.getHash(sig->hash);

				// todo: write signature
			}
			else {
				LOG_WARNING("Missing secure info for flexible fastfile writer");
			}
		}
	}

	namespace {

		int ffheadertest(int argc, const char* argv[]) {
			if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;
			std::vector<std::filesystem::path> files{};
			utils::GetFileRecurseExt(argv[2], files, ".ff\0");

			size_t split{ std::max<size_t>(files.size() / 25, 1) };
			for (size_t i = 0; i < files.size(); i++) {
				const std::filesystem::path& file{ files[i] };
				if ((i % split) == 0) {
					LOG_INFO("{}% {}/{} {}", i * 100 / files.size(), (i + 1), files.size(), file.string());
				}
				utils::InFileCE is{ file, true, std::ios::binary };
				core::bytebuffer::FileReader reader{ is };

				struct EncryptionHeader
				{
					unsigned int isEncrypted;
					unsigned __int8 IV[16];
				};

				struct DB_FFHeaderBO6 {
					byte magic[4];
					char type;
					byte unk5;
					byte unk6;
					byte unk7;
					unsigned int headerVersion;
					unsigned int xfileVersion;
					uint32_t flags;
					uint32_t fileSize;
					uint64_t timestamp;
					uint32_t unk20;
					uint32_t unk24;
					uint64_t unk28;
					uint64_t unk30;
					uint64_t size;
					uint64_t preloadWalkSize;
					uint64_t blockSize[16];
					EncryptionHeader encrypt;
				}; static_assert(sizeof(DB_FFHeaderBO6) == 0xe0);

				DB_FFHeaderBO6 header{};
				reader.Read(&header, sizeof(header));
				static EncryptionHeader empty{};
				if (std::memcmp(&header.encrypt, &empty, sizeof(empty))) {
					LOG_INFO("{}", file.string());
				}

			}

			LOG_INFO("done");
			return tool::OK;
		}

		ADD_TOOL(ffheadertest, "dev", " [ff]", "test header", ffheadertest);
	}
}