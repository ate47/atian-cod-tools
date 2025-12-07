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
#include <games/bo4/t8_errors.hpp>
#include <wincrypt.h>
#include <tools/ff/fastfile_bdiff.hpp>

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
			compatibility::acti::crypto_keys::AesKeyLocal* aeskey{};

			if (pheader.platform.encrypted) {
				aeskey = compatibility::acti::crypto_keys::GetKeyByName(pheader.ff.fastfileName, compatibility::acti::crypto_keys::VER_BO4);
				compatibility::acti::crypto_keys::RsaKeyLocal* rsa{ compatibility::acti::crypto_keys::GetRSAKeyByName("bo4")};

				if (!aeskey || !rsa) {
					throw std::runtime_error(std::format("Missing key set for ff {}", pheader.ff.fastfileName));
				}

				int shaHash{ find_hash("sha256") };

				if (shaHash == -1) {
					throw std::runtime_error(std::format("Missing sha256 for ff {}", pheader.ff.fastfileName));
				}

				rsa_key rsakey{};

				int r;

				if ((r = rsa_import(rsa->key, sizeof(rsa->key), &rsakey)) != CRYPT_OK) {
					throw std::runtime_error(std::format("Failed to import key {} for ff {}", error_to_string(r), pheader.ff.fastfileName));
				}

				uint8_t digest[20]{};

				int stat{};

				unsigned long digestSize{ sizeof(digest) };
				if ((r == rsa_decrypt_key(pheader.ff.signature, 0x100, digest, &digestSize, nullptr, 0, shaHash, &stat, &rsakey)) != CRYPT_OK) {
					throw std::runtime_error(std::format("Failed to import decrypt key {} for ff {}", error_to_string(r), pheader.ff.fastfileName));
				}
				rsa_free(&rsakey);
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

			ctx.hasGSCBin = false;
			switch (pheader.platform.platform) {
			case fastfile::XFILE_PC: {
				ctx.gscPlatform = tool::gsc::opcode::PLATFORM_PC;
				break;
			}
			case fastfile::XFILE_XBOX: {
				ctx.gscPlatform = tool::gsc::opcode::PLATFORM_XBOX;
				break;
			}
			case fastfile::XFILE_PLAYSTATION: {
				ctx.gscPlatform = tool::gsc::opcode::PLATFORM_PLAYSTATION;
				break;
			}
			case fastfile::XFILE_DEV: {
				ctx.gscPlatform = tool::gsc::opcode::PLATFORM_PC_ALPHA;
				break;
			}
			}
			// fixme
			ctx.gscPlatform = tool::gsc::opcode::PLATFORM_PC;

			byte aesIV[0x10];
			std::memcpy(aesIV, pheader.ff.aesIV, sizeof(aesIV));

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

				if (pheader.platform.encrypted) {
					symmetric_CTR ctr{};
					int aesCipher{ find_cipher("aes") };
					if (aesCipher == -1) {
						throw std::runtime_error(std::format("Missing aes for ff {}", ctx.ffname));
					}
					int r;

					if ((r = ctr_start(aesCipher, aesIV, aeskey->key, sizeof(aeskey->key), 0, 0, &ctr)) != CRYPT_OK) {
						throw std::runtime_error(std::format("Failed to start ctr for ff {}/{}", error_to_string(r), ctx.ffname));
					}

					if ((r = ctr_decrypt(blockBuff, blockBuff, block->alignedSize, &ctr)) != CRYPT_OK) {
						throw std::runtime_error(std::format("Can't decrypt block 0x{:x}: {}", loc, error_to_string(r)));
					}

					*((uint64_t*)&aesIV[0]) += block->compressedSize;
				}

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


					core::bytebuffer::ByteBuffer ffbb{ ffdata };
					core::bytebuffer::ByteBuffer fdbb{ uncompress.get(), fdDecompressedSize };

					ffdata = fastfile::bdiff::bdiff(
						&ffbb, &fdbb, fastfile::bdiff::BDiffType::BDT_TREYARCH, 
						bdiffHeader->maxDestWindowSize + bdiffHeader->maxSourceWindowSize + 2 * (bdiffHeader->maxDiffWindowSize + 0x80000)
					);

					if (opt.dump_decompressed) {
						std::filesystem::path of{ ctx.file };
						std::filesystem::path decfile{ opt.m_output / ctx.ffname };

						decfile.replace_extension(".fd.patch.dec");

						std::filesystem::create_directories(decfile.parent_path());
						if (!utils::WriteFile(decfile, ffdata)) {
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