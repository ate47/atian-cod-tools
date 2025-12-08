#include <includes.hpp>
#include <deps/oodle.hpp>
#include <hook/error.hpp>
#include <hook/memory.hpp>
#include <tools/fastfile.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <games/bo4/pool.hpp>
#include <games/bo4/t8_errors.hpp>
#include <tools/compatibility/acti_crypto_keys.hpp>
#include <tools/utils/data_utils.hpp>
#include <tools/ff/fastfile_bdiff.hpp>

namespace {
	struct XFileBO3_x132 {
		uint8_t magic[8];
		uint32_t version;
		uint8_t server;
		uint8_t compression;
		uint8_t platform;
		uint8_t encrypted;
		uint64_t timestamp;
		uint32_t changelist;
		uint32_t archiveChecksum[4];
		char builder[32];
		uint64_t size;
		uint64_t externalSize;
		uint64_t blockSize[9];
		int8_t fastfileName[64];
		uint8_t signature[256];
		uint8_t aesIV[16];
	}; static_assert(sizeof(XFileBO3_x132) == 0x1F8);

	struct XFileBO3 {
		uint8_t magic[8];
		uint32_t version;
		uint8_t server;
		fastfile::FastFileCompression compression;
		uint8_t platform;
		uint8_t encrypted;
		uint64_t timestamp;
		uint32_t changelist;
		uint32_t archiveChecksum[4];
		char builder[32];
		uint32_t metaVersion;
		char mergeFastfile[64];
		uint64_t size;
		uint64_t externalSize;
		uint64_t memMappedOffset;
		uint64_t blockSize[10];
		char fastfileName[64];
		uint8_t signature[256];
		uint8_t aesIV[16];
	}; static_assert(sizeof(XFileBO3) == 0x248);

	struct XFileBO4_Dev {
		uint8_t magic[8];
		uint32_t version;
		uint8_t server;
		fastfile::FastFileCompression compression;
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
		uint64_t blockSize[8];
		int8_t fastfileName[64];
		uint8_t signature[256];
		uint8_t aesIV[16];
	}; static_assert(sizeof(XFileBO4_Dev) == 0x638);

	struct XFileBO4_0x27F {
		uint8_t magic[8];
		uint32_t version;
		uint8_t server;
		fastfile::FastFileCompression compression;
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
		char unkSign[64];
		char fastfileName[64];
		uint8_t signature[256];
		uint8_t aesIV[16];
	}; static_assert(sizeof(XFileBO4_0x27F) == 0x840);


	void ErrorStub(uint32_t errcode) {
		throw std::runtime_error(games::bo4::errors::TranslateError(errcode));
	}

	class T78FFDecompressor : public fastfile::FFDecompressor {
	public:
		T78FFDecompressor() : fastfile::FFDecompressor("Black Ops 3/4", 0x3030303066664154, fastfile::MASK64) {}

		void LoadFastFile(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx, std::vector<byte>& ffdata) {
			if (!reader.CanRead(sizeof(fastfile::TXFileHeader))) {
				throw std::runtime_error("Can't read XFile header");
			}

			fastfile::TXFileHeader* header{ reader.Ptr<fastfile::TXFileHeader>() };

			if (opt.m_header) {
				LOG_INFO("{} fastfile version 0x{:x}, encrypted:{}, platform: {}, builder: {}",
					fastfile::GetFastFileCompressionName(header->compression), header->version, header->encrypted ? "true" : "false",
					fastfile::GetFastFilePlatformName(header->platform), header->builder
				);
			}

			size_t fastFileSize;
			size_t decompressedSizeLoc;
			size_t fastfileNameLoc;
			size_t blockSizeLoc;
			size_t signLoc{};
			size_t aesIVLoc{};
			compatibility::acti::crypto_keys::KeyVersion keyVersion{};
			const char* rsaKeyName{};

			bool xhashType{};

			// todo: probably we can scan the data and find the chunks
			switch (header->version) {
			//case 0x132: // Black ops 3 alpha
			//	fastFileSize = sizeof(XFileBO3_x132);
			//	decompressedSizeLoc = offsetof(XFileBO3_x132, size);
			//	fastfileNameLoc = offsetof(XFileBO3_x132, fastfileName);
			//	signLoc = offsetof(XFileBO3_x132, signature);
			//	aesIVLoc = offsetof(XFileBO3_x132, aesIV);
			//	blockSizeLoc = offsetof(XFileBO3_x132, blockSize);
			//	rsaKeyName = "bo3dev";
			//	keyVersion = compatibility::acti::crypto_keys::KeyVersion::VER_BO3;
			//	ctx.blocksCount = ARRAYSIZE(XFileBO3_x132::blockSize);
			//	break;
			case 0x251: // Black ops 3
				fastFileSize = sizeof(XFileBO3);
				decompressedSizeLoc = offsetof(XFileBO3, size);
				fastfileNameLoc = offsetof(XFileBO3, fastfileName);
				signLoc = offsetof(XFileBO3, signature);
				aesIVLoc = offsetof(XFileBO3, aesIV);
				blockSizeLoc = offsetof(XFileBO3, blockSize);
				rsaKeyName = "bo3";
				keyVersion = compatibility::acti::crypto_keys::KeyVersion::VER_BO3;
				ctx.blocksCount = ARRAYSIZE(XFileBO3::blockSize);
				break;
			case 0x265: // Black ops 4 Dev
				fastFileSize = sizeof(XFileBO4_Dev);
				decompressedSizeLoc = offsetof(XFileBO4_Dev, size);
				fastfileNameLoc = offsetof(XFileBO4_Dev, fastfileName);
				signLoc = offsetof(XFileBO4_Dev, signature);
				aesIVLoc = offsetof(XFileBO4_Dev, aesIV);
				blockSizeLoc = offsetof(XFileBO4_Dev, blockSize);
				rsaKeyName = "bo4dev";
				keyVersion = compatibility::acti::crypto_keys::KeyVersion::VER_BO4;
				ctx.blocksCount = ARRAYSIZE(XFileBO4_Dev::blockSize);
				break;
			case 0x27E: // Black Ops 4 Old
				fastFileSize = 0x840;
				xhashType = true;
				fastfileNameLoc = 0x6F0;
				decompressedSizeLoc = 0x490;
				blockSizeLoc = 0x4A8;
				ctx.blocksCount = 9;
				break;
			case 0x27F:// Black Ops 4
				fastFileSize = sizeof(XFileBO4_0x27F);
				decompressedSizeLoc = offsetof(XFileBO4_0x27F, size);
				fastfileNameLoc = offsetof(XFileBO4_0x27F, fastfileName);
				blockSizeLoc = offsetof(XFileBO4_0x27F, blockSize);
				ctx.blocksCount = ARRAYSIZE(XFileBO4_0x27F::blockSize);
				signLoc = offsetof(XFileBO4_0x27F, signature);
				aesIVLoc = offsetof(XFileBO4_0x27F, aesIV);
				rsaKeyName = "bo4";
				keyVersion = compatibility::acti::crypto_keys::KeyVersion::VER_BO4;
				xhashType = true;
				if (opt.m_header) {
					if (!reader.CanRead(sizeof(XFileBO4_0x27F))) {
						throw std::runtime_error("Can't read XFile header");
					}
					XFileBO4_0x27F* xfile{ reader.Ptr<XFileBO4_0x27F>() };

					LOG_INFO(
						"archive checksum: 0x{:x} 0x{:x} 0x{:x} 0x{:x}",
						xfile->archiveChecksum[0], xfile->archiveChecksum[1], xfile->archiveChecksum[2], xfile->archiveChecksum[3]
					);
				}
				break;
			default:
				throw std::runtime_error(std::format("Fast file version not supported: 0x{:x}", header->version));
			}
			ctx.hasGSCBin = false;
			switch (header->platform) {
			case fastfile::XFILE_DEV:
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
			}

			if (!reader.CanRead(fastFileSize)) {
				throw std::runtime_error("Can't read XFile header");
			}
			reader.Goto(decompressedSizeLoc);
			uint64_t decompressedSize{ reader.Read<uint64_t>() };

			reader.Goto(fastfileNameLoc);
			const char* ffname{ reader.ReadString() };
			sprintf_s(ctx.ffname, "%s", ffname);


			if (xhashType) {
				hashutils::Add(ctx.ffname, true, true);
			}

			byte aesIV[0x10];
			byte* signature{};

			if (aesIVLoc && signLoc) {
				reader.Goto(signLoc);
				signature = reader.ReadPtr<byte>(0x100);
				reader.Goto(aesIVLoc);
				reader.Read(&aesIV, sizeof(aesIV));
			}

			if (opt.m_header) {
				LOG_INFO("{} size:0x{:x}", ctx.ffname, decompressedSize);
			}

			reader.Goto(blockSizeLoc);

			for (size_t i = 0; i < ctx.blocksCount; i++) {
				ctx.blockSizes[i].size = reader.Read<uint64_t>();
			}

			reader.Goto(fastFileSize);
			compatibility::acti::crypto_keys::AesKeyLocal* aeskey{};

			if (header->encrypted) {
				// import aes key
				if (!aesIV || !rsaKeyName) {
					throw std::runtime_error(std::format("Missing decrypt data for this fast file version 0x{:x}", header->version));
				}
				aeskey = compatibility::acti::crypto_keys::GetKeyByName(ctx.ffname, keyVersion);
				compatibility::acti::crypto_keys::RsaKeyLocal* rsa{ compatibility::acti::crypto_keys::GetRSAKeyByName(rsaKeyName) };

				if (!aeskey || !rsa) {
					throw std::runtime_error(std::format("Missing key set for ff {}/{}", ctx.ffname, rsaKeyName));
				}

				int shaHash{ find_hash("sha256") };

				if (shaHash == -1) {
					throw std::runtime_error(std::format("Missing sha256 for ff {}/{}", ctx.ffname, rsaKeyName));
				}

				rsa_key rsakey{};

				int r;

				if ((r = rsa_import(rsa->key, sizeof(rsa->key), &rsakey)) != CRYPT_OK) {
					throw std::runtime_error(std::format("Failed to import key {} for ff {}/{}", error_to_string(r), ctx.ffname, rsaKeyName));
				}

				uint8_t digest[20]{};

				int stat{};
				if ((r = rsa_verify_hash(signature, 0x100, digest, sizeof(digest), shaHash, 8, &stat, &rsakey)) != CRYPT_OK) {
					throw std::runtime_error(std::format("Hash verify error {} for ff {}/{}", error_to_string(r), ctx.ffname, rsaKeyName));
				}

				unsigned long digestSize{ sizeof(digest) };
				if ((r == rsa_decrypt_key(signature, 0x100, digest, &digestSize, nullptr, 0, shaHash, &stat, &rsakey)) != CRYPT_OK) {
					throw std::runtime_error(std::format("Failed to import decrypt key {} for ff {}/{}", error_to_string(r), ctx.ffname, rsaKeyName));
				}
				rsa_free(&rsakey);
			}

			utils::compress::CompressionAlgorithm alg{ fastfile::GetFastFileCompressionAlgorithm(header->compression) };

			ffdata.resize(decompressedSize);

			size_t idx{};
			size_t offset{};
			while (offset < decompressedSize) {
				size_t loc{ reader.Loc() };

				fastfile::DBStreamHeader* block{ reader.ReadPtr<fastfile::DBStreamHeader>() };

				if (block->offset != loc) {
					LOG_ERROR("bad block position: 0x{:x} != 0x{:x}", loc, block->offset);
					break;
				}
				size_t blockId{ idx++ };

				if (!block->uncompressedSize) {
					reader.Align(0x800000);
					continue;
				}

				byte* blockBuff{ reader.ReadPtr<byte>(block->alignedSize) };


				LOG_TRACE("Decompressing {}{} block 0x{:x} (0x{:x}/0x{:x} -> 0x{:x})",
					header->encrypted ? "encrypted " : "",
					fastfile::GetFastFileCompressionName(header->compression),
					loc,
					block->compressedSize, block->alignedSize, block->uncompressedSize
				);

				byte* decompressed{ &ffdata[offset] };
				offset += block->uncompressedSize;

				if (header->encrypted) {
					symmetric_CTR ctr{};
					int aesCipher{ find_cipher("aes") };
					if (aesCipher == -1) {
						throw std::runtime_error(std::format("Missing aes for ff {}/{}", ctx.ffname, rsaKeyName));
					}
					int r;

					if ((r = ctr_start(aesCipher, aesIV, aeskey->key, sizeof(aeskey->key), 0, 0, &ctr)) != CRYPT_OK) {
						throw std::runtime_error(std::format("Failed to start ctr {} for ff {}/{}", error_to_string(r), ctx.ffname, rsaKeyName));
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

			LOG_TRACE("Decompressed 0x{:x} byte(s) from 0x{:x} block(s)", ffdata.size(), idx);

			if (opt.m_fd) {
				std::filesystem::path fdfile{ ctx.file };

				fdfile.replace_extension(".fd");

				std::vector<byte> fileFDBuff{};
				if (opt.ReadFile(fdfile.string(), fileFDBuff)) {
					core::bytebuffer::ByteBuffer fdreader{ fileFDBuff };

					struct BDiffHeaderPost {
						uint32_t size;
						float version;
						uint32_t flags;
						size_t maxDestWindowSize;
						uint64_t maxSourceWindowSize;
						uint64_t maxDiffWindowSize;
					}; static_assert(sizeof(BDiffHeaderPost) == 0x28);

					size_t bdiffHeaderSize{ fastFileSize * 2 + sizeof(BDiffHeaderPost) };

					if (!fdreader.CanRead(bdiffHeaderSize)) {
						throw std::runtime_error("Can't read patch file header");
					}

					fastfile::TXFileHeader* newXFileHeader{ fdreader.Ptr<fastfile::TXFileHeader>() };
					fdreader.Goto(decompressedSizeLoc);
					uint64_t fdDecompressedSize{ fdreader.Read<uint64_t>() };

					// use patch block sizes
					fdreader.Goto(blockSizeLoc);

					for (size_t i = 0; i < ctx.blocksCount; i++) {
						uint64_t s{ fdreader.Read<uint64_t>() };
						LOG_DEBUG("New size for block {} 0x{:x} -> 0x{:x}", i, ctx.blockSizes[i].size, s);
						ctx.blockSizes[i].size = s;
					}

					fdreader.Goto(fastFileSize);
					fastfile::TXFileHeader* baseXFileHdr{ fdreader.Ptr<fastfile::TXFileHeader>() };
					fdreader.Goto(fastFileSize * 2);
					BDiffHeaderPost* bdiffHeader{ fdreader.ReadPtr<BDiffHeaderPost>() };

					if (opt.m_header) {
						LOG_INFO("s:0x{:x}, version:{}, flags:0x{:x} 0x{:x} 0x{:x} 0x{:x}, decompSize:0x{:x}",
							bdiffHeader->size, bdiffHeader->version, bdiffHeader->flags, bdiffHeader->maxDestWindowSize, bdiffHeader->maxSourceWindowSize, bdiffHeader->maxDiffWindowSize, fdDecompressedSize);
						
						if (newXFileHeader->version == 0x27f) {
							XFileBO4_0x27F* xfile{ (XFileBO4_0x27F*)newXFileHeader };

							LOG_INFO(
								"archive checksum: 0x{:x} 0x{:x} 0x{:x} 0x{:x}",
								xfile->archiveChecksum[0], xfile->archiveChecksum[1], xfile->archiveChecksum[2], xfile->archiveChecksum[3]
							);
						}
					}

					if (bdiffHeader->size != bdiffHeaderSize) {
						throw std::runtime_error(std::format("Can't read patch file header size: 0x{:x} != 0x{:x}", bdiffHeader->size, bdiffHeaderSize));
					}

					if (std::memcmp(baseXFileHdr, header, fastFileSize)) {
						throw std::runtime_error("The patch file is not for this fast file");
					}

					if (bdiffHeader->maxDiffWindowSize < 0x10000) bdiffHeader->maxDiffWindowSize = 0x10000;
					if (bdiffHeader->maxDestWindowSize < 0x10000) bdiffHeader->maxDestWindowSize = 0x10000;
					if (bdiffHeader->maxSourceWindowSize < 0x10000) bdiffHeader->maxSourceWindowSize = 0x10000;

					size_t compressedSize{ fdreader.Remaining() };

					LOG_TRACE("Decompressing patch {} 0x{:x}: 0x{:x} -> 0x{:x}", fastfile::GetFastFileCompressionName(newXFileHeader->compression), fdreader.Loc(), compressedSize, fdDecompressedSize);
					auto uncompress{ std::make_unique<byte[]>(fdDecompressedSize) };

					utils::compress::CompressionAlgorithm alg{};
					switch (newXFileHeader->compression) {
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
						throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)newXFileHeader->compression));
					}

					if (!utils::compress::Decompress(alg, uncompress.get(), fdDecompressedSize, fdreader.ReadPtr<byte>(compressedSize), compressedSize)) {
						throw std::runtime_error(std::format("Error when decompressing fd data for type {}", fastfile::GetFastFileCompressionName(newXFileHeader->compression)));
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
					if (!opt.m_fdIgnoreMissing) {
						throw std::runtime_error(std::format("No patch file {}", fdfile.string()));
					}
					LOG_INFO("No patch file {}", fdfile.string());
				}
			}
		}
	};

	utils::ArrayAdder<T78FFDecompressor, fastfile::FFDecompressor> arr{ fastfile::GetDecompressors() };
}