#include <includes.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/fastfile_flexible.hpp>
#include <tools/fastfile/linkers/linker_cw.hpp>
#include <tools/compatibility/scobalula_wnigen.hpp>

namespace {
	using namespace fastfile;

	class FFCompressorBOCW : public FFCompressor {
	public:
		FFCompressorBOCW() : FFCompressor("CW", "Black ops Cold War fast file compressor") {
		}
		void Init(FastFileLinkerOption& opt) override {
			constexpr size_t maxSize{ utils::GetMaxSize<int32_t>() };
			if (opt.chunkSize > maxSize) {
				LOG_WARNING("Chunk size can't be above {}", maxSize);
				opt.chunkSize = maxSize;
			}
		}

		void Compress(FastFileLinkerContext& ctx) override {
			// like bo4, bocw seems to constantly use oodle, but DB_DecompressIOStream can handle everything
			FastFileCompression compression{ FastFileCompression::XFILE_UNCOMPRESSED };
			const char* compressionType{ ctx.zone.GetConfig("compression") };
			if (compressionType) {
				compression = GetFastFileCompression(compressionType);

				if (compression == FastFileCompression::XFILE_COMPRESSION_COUNT) {
					throw std::runtime_error(std::format("Invalid compression format name: {}", compressionType));
				}

			}
			utils::compress::CompressionAlgorithm alg{ fastfile::GetFastFileCompressionAlgorithm(compression) };
			if (ctx.zone.GetConfigBool("compression.high", false)) {
				alg = alg | utils::compress::COMP_HIGH_COMPRESSION;
			}

			uint32_t chunkSize = (uint32_t)(ctx.opt.chunkSize ? ctx.opt.chunkSize : 0x3fee0);
			for (fastfile::FastFile& ff : ctx.fastfiles) {
				std::vector<byte> out{};
				fastfile::flexible::FlexibleFastFileWriter hwriter{ out, fastfile::flexible::MAGIC };

				struct {
					uint32_t version{};
					fastfile::flexible::PFFBuildData build{};
					fastfile::flexible::PFFFastFileInfo ff{};
					fastfile::flexible::PFFSizeData size{};
					fastfile::flexible::PFPlatformData platform{};
					uint64_t blockSizes[fastfile::linker::cw::XFILE_BLOCK_COUNT]{};

				} header;
				hwriter.AddBlock(fastfile::flexible::ST_VERSION, &header.version, sizeof(header.version));
				hwriter.AddBlock(fastfile::flexible::ST_PLATFORM_DATA, &header.platform, sizeof(header.platform));
				hwriter.AddBlock(fastfile::flexible::ST_BUILD_DATA, &header.build, sizeof(header.build));
				hwriter.AddBlock(fastfile::flexible::ST_BLOCK_SIZES, &header.blockSizes, sizeof(header.blockSizes));
				hwriter.AddBlock(fastfile::flexible::ST_SIZE_DATA, &header.size, sizeof(header.size));
				hwriter.AddBlock(fastfile::flexible::ST_FASTFILE_INFO, &header.ff, sizeof(header.ff));


				// write blocks

				byte* toCompress{ ff.linkedData.data() };
				size_t remainingSize{ ff.linkedData.size() };

				size_t idx{};
				std::vector<byte> compressBuffer{};
				size_t compressedSize{};

				LOG_TRACE("start compressing chunk 0x{:x} byte(s) using {} with chunks of size 0x{:x}...", remainingSize, alg, chunkSize);
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

					LOG_TRACE("Compressed 0x{:x}->0x{:x} at chunk 0x{:x}, remaining 0x{:x}", uncompressedSize, h.compressedSize, idx, remainingSize);

					idx++;
				}

				// end stream header
				uint32_t endBlockOffset{ (uint32_t)utils::Allocate(out, sizeof(fastfile::DBStreamHeader)) };
				fastfile::DBStreamHeader& h{ *(fastfile::DBStreamHeader*)&out[endBlockOffset] };
				h.offset = endBlockOffset;
				h.uncompressedSize = 0;
				h.alignedSize = 0;
				h.compressedSize = 0;
				utils::Allocate(out, 0x40);


				// write header data
				header.version = 0x64;
				header.platform.platform = ctx.opt.platform;
				header.platform.server = ctx.opt.server;
				header.build.timestamp = utils::GetTimestamp() / 1000;
				header.platform.encrypted = false;
				header.size.size = ff.linkedData.size();
				header.platform.compression = compression;

				static uint32_t archiveChecksum[4]{ 0xf70752af, 0x7044510f, 0xfaa71cc5, 0x9fd5dfc7 };
				static_assert(sizeof(archiveChecksum) == sizeof(header.build.archiveChecksum));
				std::memcpy(header.build.archiveChecksum, archiveChecksum, sizeof(archiveChecksum));

				// build data
				TCHAR szBuffer[std::max<size_t>(MAX_COMPUTERNAME_LENGTH + 1, 16)];
				DWORD dwSize = sizeof(szBuffer);
				if (!GetComputerNameA(szBuffer, &dwSize)) {
					LOG_WARNING("Can't get computer name");
					strncpy_s(szBuffer, "ERROR", sizeof(szBuffer));
				}

				snprintf(header.build.builderName, sizeof(header.build.builderName), "%s", szBuffer);
				snprintf(header.ff.fastfileName, sizeof(header.ff.fastfileName), "%s", ff.ffname);

				// blocks load data
				for (size_t i = 0; i < fastfile::linker::cw::XFILE_BLOCK_COUNT; i++) {
					header.blockSizes[i] = (uint64_t)ff.blockSizes[i] * 2;
				}

				// todo: write other data

				hwriter.WriteEnd();

				std::filesystem::path outputFileFF{ ctx.opt.m_output / "zone" / std::format("{}.ff", ff.ffname) };

				std::filesystem::create_directories(outputFileFF.parent_path());
				if (!utils::WriteFile(outputFileFF, out)) {
					throw std::runtime_error(std::format("Can't write into {}", outputFileFF.string()));
				}

				if (ctx.opt.m_fd) {
					LOG_WARNING(".fd file generator not implemented");
				}

				LOG_INFO("Compressed {} into {} [{}]({} -> {} bytes / {}% saved)", ff.ffname, outputFileFF.string(), alg, ff.linkedData.size(), compressedSize, (100 - 100 * compressedSize / ff.linkedData.size()));
			}

			if (!ctx.storedHashes.empty()) {
				std::filesystem::path outputFileWNI{ ctx.opt.m_output / "package_index" / std::format("{}.wni", ctx.mainFFName) };
				std::filesystem::create_directories(outputFileWNI.parent_path());

				if (compatibility::scobalula::wnigen::CompressWNIFile(ctx.storedHashes, outputFileWNI) != tool::OK) {
					throw std::runtime_error(std::format("Can't write wni info into {}", outputFileWNI.string()));
				}
			}
		}

	};


	utils::ArrayAdder<FFCompressorBOCW, FFCompressor> impl{ GetCompressors() };
}