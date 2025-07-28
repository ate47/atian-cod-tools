#include <includes.hpp>
#include <tools/fastfile.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/ff/fastfile_packed.hpp>
#include <games/bo4/pool.hpp>
#include <tools/utils/data_utils.hpp>
#include <utils/compress_utils.hpp>
#include <tools/compatibility/acti_crypto_keys.hpp>
#include <hook/error.hpp>
#include <hook/memory.hpp>
#include <bdiff.hpp>
#include <games/bo4/t8_errors.hpp>
#include <wincrypt.h>

namespace {

	namespace cod2020 {
		enum XFileBlock : __int32 {
			XFILE_BLOCK_TEMP = 0x0,
			XFILE_BLOCK_TEMP_PRELOAD = 0x1,
			XFILE_BLOCK_RUNTIME_VIRTUAL = 0x2,
			XFILE_BLOCK_RUNTIME_PHYSICAL = 0x3,
			XFILE_BLOCK_VIRTUAL = 0x4,
			XFILE_BLOCK_CPU_GPU_SHARE = 0x5,
			XFILE_BLOCK_PHYSICAL = 0x6,
			XFILE_BLOCK_STREAMER = 0x7,
			XFILE_BLOCK_STREAMER_CPU = 0x8,
			XFILE_BLOCK_MEMMAPPED = 0x9,
			XFILE_BLOCK_LOAD_TIME = 0xA,
			XFILE_BLOCK_TEMP_PRELOAD_SAVED_PTR = 0xB,
			XFILE_BLOCK_COUNT = 0xC,
		};
		struct XFileData {
			uint32_t version;
			bool server;
			fastfile::FastFileCompression compression;
			fastfile::FastFilePlatform platform;
			bool encrypted;
			uint64_t unk8;
			uint64_t timestamp;
			uint32_t changelist;
			uint32_t archiveChecksum[4];
			char builderName[32];
			uint32_t unk4c;
			uint32_t unk50;
			byte linkResultCode;
			char linkResult[256];
			uint64_t size;
			uint64_t externalSize;
			uint64_t memMappedOffset;
			uint64_t blockSize[12];
			byte pad[536];
			char fastfileName[64];
			uint8_t signature[256];
			uint8_t aesIV[16];
		}; static_assert(sizeof(XFileData) == 0x538);
	}
	namespace cwrelease {
		enum XFileBlock : __int32 {
			XFILE_BLOCK_TEMP = 0x0,
			XFILE_BLOCK_TEMP_PRELOAD = 0x1,
			XFILE_BLOCK_RUNTIME_VIRTUAL = 0x2,
			XFILE_BLOCK_RUNTIME_PHYSICAL = 0x3,
			XFILE_BLOCK_VIRTUAL = 0x4,
			XFILE_BLOCK_CPU_GPU_SHARE = 0x5,
			XFILE_BLOCK_PHYSICAL = 0x6,
			XFILE_BLOCK_STREAMER = 0x7,
			XFILE_BLOCK_STREAMER_CPU = 0x8,
			XFILE_BLOCK_MEMMAPPED = 0x9,
			XFILE_BLOCK_LOAD_TIME = 0xA,
			XFILE_BLOCK_TEMP_PRELOAD_SAVED_PTR = 0xB,
			XFILE_BLOCK_MESH = 0xC,
			XFILE_BLOCK_COUNT = 0xD,
		};
		struct XFileData {
			uint32_t version;
			bool server;
			fastfile::FastFileCompression compression;
			fastfile::FastFilePlatform platform;
			bool encrypted;
			uint64_t unk8;
			uint64_t timestamp;
			uint32_t changelist;
			uint32_t archiveChecksum[4];
			char builderName[32];
			uint32_t unk4c;
			uint32_t unk50;
			byte linkResultCode;
			char linkResult[256];
			uint64_t size;
			uint64_t externalSize;
			uint64_t memMappedOffset;
			uint64_t blockSize[13];
			byte pad[584];
			char fastfileName[64];
			uint8_t signature[256];
			uint8_t aesIV[16];
		}; static_assert(sizeof(XFileData) == 0x570);

	}

	void DumpHeader(fastfile::packed::PackedFastFileReader& data, const char* title) {
		LOG_INFO("{}: v{} server:{}, comp:{}, plt:{} enc:{}, bld:{}", 
			data.ff.fastfileName, data.version, data.platform.server ? "true" : "false",
			fastfile::GetFastFileCompressionName(data.platform.compression), fastfile::GetFastFilePlatformName(data.platform.platform),
			data.platform.server ? "true" : "false", data.build.builderName
		);
	}

	class T9FFDecompressor : public fastfile::FFDecompressor {
	public:
        T9FFDecompressor() : fastfile::FFDecompressor("Black Ops Cold War", 0x46464154, fastfile::MASK32) {}

        void LoadFastFile(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx, std::vector<byte>& ffdata) {
			fastfile::packed::PackedFastFileReader pheader{};

			pheader.ReadHeader(reader);

			if (opt.m_header) {
				DumpHeader(pheader, "header");
			}


			if (pheader.version != 0x64) {
				throw std::runtime_error(std::format("Fast file version not supported: 0x{:x}", pheader.version));
			}
			if (pheader.platform.encrypted) {
				throw std::runtime_error("Encrypted Fast file version not supported");
			}

			const char* ffname = pheader.ff.fastfileName;
			sprintf_s(ctx.ffname, "%s", ffname);
			// add the ff name for the lookup
			hashutils::Add(ctx.ffname, true, true);

			ctx.blocksCount = pheader.blocksCount;
			for (size_t i = 0; i < ctx.blocksCount; i++) {
				ctx.blockSizes[i].size = pheader.blockSizes[i];
			}

            utils::compress::CompressionAlgorithm alg{ fastfile::GetFastFileCompressionAlgorithm(pheader.platform.compression) };

            size_t idx{};
            while (true) {
                size_t loc{ reader.Loc() };

                fastfile::DBStreamHeader* block{ reader.ReadPtr<fastfile::DBStreamHeader>() };
                if (!block->alignedSize) break; // end

                if (block->offset != loc) {
                    LOG_ERROR("bad block position: 0x{:x} != 0x{:x}", loc, block->offset);
                    break;
                }
                idx++;

                if (!block->uncompressedSize) {
                    reader.Align(0x800000);
                    continue;
                }

                byte* blockBuff{ reader.ReadPtr<byte>(block->alignedSize) };
                LOG_TRACE("Decompressing block 0x{:x} {}(0x{:x}/0x{:x} -> 0x{:x})", loc, pheader.platform.encrypted ? "encrypted " : "", block->compressedSize, block->alignedSize, block->uncompressedSize);

                size_t unloc{ utils::Allocate(ffdata, block->uncompressedSize) };
                byte* decompressed{ &ffdata[unloc] };

                if (!utils::compress::Decompress(alg, decompressed, block->uncompressedSize, blockBuff, block->compressedSize)) {
                    throw std::runtime_error(std::format("Can't decompress block 0x{:x}", loc));
                }
            }

			if (opt.m_fd) {
				std::filesystem::path fdfile{ ctx.file };

				fdfile.replace_extension(".fd");

				std::vector<byte> fileFDBuff{};
				if (opt.ReadFile(fdfile.string(), fileFDBuff)) {
					core::bytebuffer::ByteBuffer fdreader{ fileFDBuff };

					
					struct BDiffHeader {
						uint32_t size;
						float version;
						uint32_t flags;
						size_t maxDestWindowSize;
						uint64_t maxSourceWindowSize;
						uint64_t maxDiffWindowSize;
					}; static_assert(sizeof(BDiffHeader) == 0x28);

					BDiffHeader* bdiffHeader{ fdreader.ReadPtr<BDiffHeader>() };

					fastfile::packed::PackedFastFileReader newXFileHeader{};
					fastfile::packed::PackedFastFileReader baseXFileHdr{};
					newXFileHeader.ReadHeader(fdreader);
					baseXFileHdr.ReadHeader(fdreader);
					
					
					// patch header B96310


					if (opt.m_header) {
						DumpHeader(newXFileHeader, "fd header");
						DumpHeader(baseXFileHdr, "ff header");
					}

					if (std::memcmp(&baseXFileHdr, &pheader, sizeof(baseXFileHdr))) {
						throw std::runtime_error("The patch file is for this fast file");
					}

					for (size_t i = 0; i < ctx.blocksCount; i++) {
						LOG_DEBUG("New size for block {} 0x{:x} -> 0x{:x}", i, ctx.blockSizes[i].size, newXFileHeader.blockSizes[i]);
						ctx.blockSizes[i].size = newXFileHeader.blockSizes[i];
					}

					if (bdiffHeader->maxDiffWindowSize < 0x10000) bdiffHeader->maxDiffWindowSize = 0x10000;
					if (bdiffHeader->maxDestWindowSize < 0x10000) bdiffHeader->maxDestWindowSize = 0x10000;
					if (bdiffHeader->maxSourceWindowSize < 0x10000) bdiffHeader->maxSourceWindowSize = 0x10000;

					size_t compressedSize{ fdreader.Remaining() };
					size_t fdDecompressedSize{ newXFileHeader.size.size };

					LOG_TRACE("Decompressing patch {} 0x{:x}: 0x{:x} -> 0x{:x}", fastfile::GetFastFileCompressionName(newXFileHeader.platform.compression), fdreader.Loc(), compressedSize, fdDecompressedSize);
					auto uncompress{ std::make_unique<byte[]>(fdDecompressedSize) };

					utils::compress::CompressionAlgorithm alg{};
					switch (newXFileHeader.platform.compression) {
					case fastfile::XFILE_BDELTA_UNCOMP:
						alg = utils::compress::COMP_NONE;
						break;
					case fastfile::XFILE_BDELTA_ZLIB:
						alg = utils::compress::COMP_ZLIB;
						break;
					case fastfile::XFILE_BDELTA_LZMA:
						alg = utils::compress::COMP_LZMA;
						break;
					default:
						throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)newXFileHeader.platform.compression));
					}

					if (!utils::compress::Decompress(alg, uncompress.get(), fdDecompressedSize, fdreader.ReadPtr<byte>(compressedSize), compressedSize)) {
						throw std::runtime_error(std::format("Error when decompressing fd data for type {}", fastfile::GetFastFileCompressionName(newXFileHeader.platform.compression)));
					}

					LOG_TRACE("Decompressed 0x{:x} byte(s) from patch file", fdDecompressedSize);

					if (opt.dump_decompressed) {
						std::filesystem::path of{ ctx.file };
						std::filesystem::path decfile{ opt.m_output / ctx.ffname };

						decfile.replace_extension(".fd.dec");

						std::filesystem::create_directories(decfile.parent_path());
						if (!utils::WriteFile(decfile, uncompress.get(), fdDecompressedSize)) {
							LOG_ERROR("Can't dump {}", decfile.string());
						}
						else {
							LOG_INFO("Dump fd into {}", decfile.string());
						}
					}

					typedef uint8_t* vcSourceCB_t(size_t offset, size_t size);
					typedef uint8_t* vcDiffCB_t(size_t offset, size_t size, size_t* pOffset);
					typedef uint8_t* vcDestCB_t(size_t size);
					struct BDiffState {
						bool headerRead{};
						bool error{};
						bool eof{};
						unsigned int features{};
					};


					static struct {
						BDiffHeader* bdiffHeader{};
						byte* destWindow{};
						size_t destWindowSize{};
						size_t patchWindowOffsetLast{};
						size_t destWindowLastSize{};
						core::bytebuffer::ByteBuffer* ffbb{};
						core::bytebuffer::ByteBuffer* fdbb{};

						bool (*bdiff)(BDiffState* diffState, vcSourceCB_t* sourceDataCB, vcDiffCB_t* patchDataCB, vcDestCB_t* destDataCB) {};

						std::vector<byte> destData{};

						void SyncData() {
							if (!destWindowLastSize) return;

							LOG_TRACE("Sync 0x{:x} bytes", destWindowLastSize);
							utils::WriteValue(destData, destWindow, destWindowLastSize);

							destWindowLastSize = 0;
						}
					} bdiffStates{};
					if (!bdiffStates.bdiff) {
						hook::library::ScanResult bdiffOff{ opt.GetGame(true).FindAnyScan(
							"bdiff",
							"40 53 55 41 54 41 56 B8", // cw/cod2020
							"40 55 41 54 41 56 41 57 B8" // bo4
						) };

						LOG_TRACE("find bdiff: {}", hook::library::CodePointer{ bdiffOff.location });
						bdiffStates.bdiff = reinterpret_cast<decltype(bdiffStates.bdiff)>(bdiffOff.location);
					}

					bdiffStates.bdiffHeader = bdiffHeader;
					std::vector<byte> outwindow{};
					bdiffStates.destWindowSize = bdiffHeader->maxDestWindowSize + bdiffHeader->maxSourceWindowSize + 2 * (bdiffHeader->maxDiffWindowSize + 0x80000);
					outwindow.resize(bdiffStates.destWindowSize);
					bdiffStates.destWindow = outwindow.data();
					bdiffStates.destData.clear();
					bdiffStates.destWindowLastSize = 0;

					core::bytebuffer::ByteBuffer ffbb{ ffdata };
					core::bytebuffer::ByteBuffer fdbb{ uncompress.get(), fdDecompressedSize };
					bdiffStates.fdbb = &fdbb;
					bdiffStates.ffbb = &ffbb;

					BDiffState state{};
					bdiffStates.patchWindowOffsetLast = 0;
					do {
						if (!bdiffStates.fdbb->CanRead(0x400)) {
							break; // can't read header
						}
						LOG_TRACE("Pre bdiff");
						if (!bdiffStates.bdiff(&state,
							[](size_t offset, size_t size) -> uint8_t* {
								// vcSourceCB_t
								bdiffStates.ffbb->Goto(offset);
								if (!bdiffStates.ffbb->CanRead(size)) {
									hook::error::DumpStackTraceFrom();
									throw std::runtime_error(std::format("vcSourceCB_t: read too much at 0x{:x}/0x{:x}", bdiffStates.ffbb->Loc(), size));
								}
								LOG_TRACE("vcSourceCB_t: read 0x{:x}:0x{:x}", bdiffStates.ffbb->Loc(), size);
								return bdiffStates.ffbb->ReadPtr<uint8_t>(size);
							},
							[](size_t offset, size_t size, size_t* pOffset) -> uint8_t* {
								if (offset) {
									bdiffStates.patchWindowOffsetLast = offset;
								}
								else {
									offset = bdiffStates.patchWindowOffsetLast;
								}
								if (pOffset) *pOffset = offset;

								// vcDiffCB_t
								bdiffStates.fdbb->Goto(offset);
								if (!bdiffStates.fdbb->CanRead(size)) {
									hook::error::DumpStackTraceFrom();
									throw std::runtime_error(std::format("vcDiffCB_t: read too much at 0x{:x}/0x{:x}", bdiffStates.fdbb->Loc(), size));
								}
								LOG_TRACE("vcDiffCB_t: read 0x{:x}:0x{:x}", bdiffStates.fdbb->Loc(), size);
								return bdiffStates.fdbb->ReadPtr<uint8_t>(size);
							},
							[](size_t size) -> uint8_t* {
								// vcDestCB_t
								bdiffStates.SyncData();
								bdiffStates.destWindowLastSize = size;
								LOG_TRACE("vcDestCB_t: give 0x{:x}", size);
								if (size <= bdiffStates.bdiffHeader->maxDestWindowSize) {
									return bdiffStates.destWindow;
								}
								throw std::runtime_error(std::format("vcDestCB_t: dest window too small 0x{:x} < 0x{:x}", bdiffStates.bdiffHeader->maxDestWindowSize, size));
							}
						)) {
							throw std::runtime_error(std::format("bdiff error: 0x{:x}", state.features));
						}
					} while (bdiffStates.destWindowLastSize);
					bdiffStates.SyncData();
					LOG_TRACE("end size: 0x{:x}", bdiffStates.destData.size());

					ffdata = bdiffStates.destData;

					if (opt.dump_decompressed) {
						std::filesystem::path of{ ctx.file };
						std::filesystem::path decfile{ opt.m_output / ctx.ffname };

						decfile.replace_extension(".fd.patch.dec");

						std::filesystem::create_directories(decfile.parent_path());
						if (!utils::WriteFile(decfile, bdiffStates.destData)) {
							LOG_ERROR("Can't dump {}", decfile.string());
						}
						else {
							LOG_INFO("Dump fd patch into {}", decfile.string());
						}
					}

				}
				else {
					throw std::runtime_error(std::format("No patch file {}", fdfile.string()));
				}
			}
        }
	};

	utils::ArrayAdder<T9FFDecompressor, fastfile::FFDecompressor> arr{ fastfile::GetDecompressors() };
}