#include <includes.hpp>
#include <core/config.hpp>
#include <core/bytebuffer_file.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/ff/fastfile_flexible.hpp>
#include <utils/data_utils.hpp>

namespace fastfile::flexible {
	void FlexibleFastFileReader::ReadHeader(core::bytebuffer::ByteBuffer& reader) {
		uint32_t magic{ reader.Read<uint32_t>() };
		if (magic != MAGIC && magic != MAGIC_A) {
			throw std::runtime_error(std::format("Invalid magic 0x{:x}", magic));
		}
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

	FlexibleFastFileWriter::FlexibleFastFileWriter(std::vector<byte>& data) : data(data) {
		utils::Allocate(data, sizeof(uint32_t) * 2); // magic + numBlock
	}

	void FlexibleFastFileWriter::AddBlock(SectionType type, const void* ptr, uint32_t len) {
		utils::WriteValue<uint32_t>(data, type);
		utils::WriteValue<uint32_t>(data, len);
		size_t off{ utils::Allocate(data, len) };
		
		chunks.emplace_back(off, ptr, len);
	}

	void FlexibleFastFileWriter::WriteEnd() {
		uint32_t* header{ (uint32_t*)data.data() };
		// complete header
		header[0] = MAGIC;
		header[1] = (uint32_t)chunks.size();

		// write chunks
		for (PackChunk& chunk : chunks) {
			std::memcpy(&data[chunk.offset], chunk.buffer, chunk.size);
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