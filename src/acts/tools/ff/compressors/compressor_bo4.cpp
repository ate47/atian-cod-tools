#include <includes.hpp>
#include <tools/fastfile.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>

namespace {
	using namespace fastfile;
	struct XFile {
		uint8_t magic[8];
		uint32_t version;
		uint8_t server;
		FastFileCompression compression;
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
		uint64_t blockSize[fastfile::linker::bo4::XFILE_BLOCK_COUNT];
		uint64_t unk4f0;
		uint64_t unk4f8;
		uint64_t unk500;
		uint64_t unk508;
		uint64_t unk510s;
		uint64_t unk518s;
		uint64_t unk520pa;
		byte pad0[392];
		char unkSign[64];
		char fastfileName[64];
		uint8_t signature[256];
		uint8_t aesIV[16];
	}; static_assert(sizeof(XFile) == 0x840);


	class FFCompressorBO4 : public FFCompressor {
		// bo4 seems constantly use oodle, but DB_DecompressIOStream can handle everything
		FastFileCompression compression{ FastFileCompression::XFILE_UNCOMPRESSED };
		utils::compress::CompressionAlgorithm alg{ utils::compress::COMP_NONE };
	public:
		FFCompressorBO4() : FFCompressor("BO4", "Black ops 4 fast file compressor") {
		}
		void Init(FastFileLinkerOption& opt) override {
			if (opt.compressionType) {
				compression = GetFastFileCompression(opt.compressionType);

				if (compression == FastFileCompression::XFILE_COMPRESSION_COUNT) {
					throw std::runtime_error(std::format("Invalid compression format name: {}", opt.compressionType));
				}

			}
			alg = fastfile::GetFastFileCompressionAlgorithm(compression);
			if (opt.useHC) {
				alg = alg | utils::compress::COMP_HIGH_COMPRESSION;
			}
			constexpr size_t maxSize{ utils::GetMaxSize<int32_t>() };
			if (opt.chunkSize > maxSize) {
				LOG_WARNING("Chunk size can't be above {}", maxSize);
			}
			opt.chunkSize = maxSize;
		}

		void Compress(FastFileLinkerContext& ctx) override {
			std::vector<byte> out{};
			utils::Allocate<XFile>(out);

			// write blocks
			uint32_t chunkSize = (uint32_t)(ctx.opt.chunkSize ? ctx.opt.chunkSize : 0x3fee0);

			byte* toCompress{ ctx.linkedData.data() };
			size_t remainingSize{ ctx.linkedData.size() };

			size_t idx{};
			std::vector<byte> compressBuffer{};
			size_t compressedSize{};
			LOG_TRACE("start compressing chunk 0x{:x} byte(s) using {}...", remainingSize, alg);
			while (remainingSize > 0) {
				uint32_t uncompressedSize{ (uint32_t)std::min<size_t>(chunkSize, remainingSize) };

				compressBuffer.clear();
				if (!utils::compress::CompressBuffer(alg, toCompress, uncompressedSize, compressBuffer)) {
					throw std::runtime_error(std::format("Can't compress chunk 0x{:x} of size 0x{:x}", idx - 1, uncompressedSize));
				}
				uint32_t alignedSize{ utils::Aligned<uint32_t>((uint32_t)compressBuffer.size()) };
				compressedSize += compressBuffer.size();

				uint32_t blockOffset{ (uint32_t)utils::Allocate(out, sizeof(fastfile::DBStreamHeader) + alignedSize) };

				fastfile::DBStreamHeader& h{ *(fastfile::DBStreamHeader*)&out[blockOffset] };
				
				h.offset = blockOffset;
				h.uncompressedSize = uncompressedSize;
				h.compressedSize = (uint32_t)compressBuffer.size();
				h.alignedSize = (uint32_t)alignedSize;

				// write compressed chunk
				std::memcpy(&out[blockOffset + sizeof(fastfile::DBStreamHeader)], compressBuffer.data(), compressBuffer.size());


				// move to the next buffer
				toCompress += uncompressedSize;
				remainingSize -= uncompressedSize;

				LOG_TRACE("Compressed 0x{:x} at chunk 0x{:x}, remaining 0x{:x}", h.compressedSize, idx, remainingSize);

				idx++;
			}

			// end stream header
			utils::Allocate<fastfile::DBStreamHeader>(out);


			// write header data
			XFile& header{ *(XFile*)out.data() };
			*(uint64_t*)&header.magic[0] = 0x3030303066664154;
			header.version = 0x27F;
			header.platform = ctx.opt.platform;
			header.server = ctx.opt.server;
			header.timestamp = utils::GetTimestamp() / 1000;
			header.encrypted = false;
			header.size = ctx.linkedData.size();
			header.compression = compression;
			static uint32_t archiveChecksum[4]{ 0x34FF23CB, 0xE4505D2, 0xB3C783A, 0x3208003D };
			static_assert(sizeof(archiveChecksum) == sizeof(header.archiveChecksum));
			std::memcpy(header.archiveChecksum, archiveChecksum, sizeof(archiveChecksum));

			// build data
			TCHAR szBuffer[std::max<size_t>(MAX_COMPUTERNAME_LENGTH + 1, 16)];
			DWORD dwSize = sizeof(szBuffer);
			if (!GetComputerNameA(szBuffer, &dwSize)) {
				LOG_WARNING("Can't get computer name");
				strncpy_s(szBuffer, "ERROR", sizeof(szBuffer));
			}

			snprintf(header.builder, sizeof(header.builder), "%s", szBuffer);
			snprintf(header.fastfileName, sizeof(header.fastfileName), "%s", ctx.ffname);

			// blocks load data
			for (size_t i = 0; i < fastfile::linker::bo4::XFILE_BLOCK_COUNT; i++) {
				header.blockSize[i] = (uint64_t)ctx.blockSizes[i] * 2;
			}

			// todo: write other data

			std::filesystem::path outputFile{ ctx.opt.m_output / "bo4" / std::format("{}.ff", ctx.ffname) };

			std::filesystem::create_directories(outputFile.parent_path());
			if (!utils::WriteFile(outputFile, out)) {
				throw std::runtime_error(std::format("Can't write into {}", outputFile.string()));
			}

			if (ctx.opt.m_fd) {
				LOG_WARNING(".fd file generator not implemented");
			}

			LOG_INFO("Compressed {} into {} ({} -> {} bytes / {}% saved)", ctx.ffname, outputFile.string(), ctx.linkedData.size(), compressedSize, (100 - 100 * compressedSize / ctx.linkedData.size()));
		}

	};


	utils::ArrayAdder<FFCompressorBO4, FFCompressor> impl{ GetCompressors() };
}