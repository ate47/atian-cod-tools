#include <includes.hpp>
#include <deps/oodle.hpp>
#include <hook/error.hpp>
#include <hook/memory.hpp>
#include <tools/fastfile.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <games/bo4/pool.hpp>
#include <games/bo4/t8_errors.hpp>
#include <tools/compatibility/acti_crypto_keys.hpp>
#include <bcrypt.h>
#include <bdiff.hpp>
#include <tools/utils/data_utils.hpp>

namespace {
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
			size_t aesIVLoc{};
			size_t aesIVSize{};

			bool xhashType{};

			// todo: probably we can scan the data and find the chunks
			switch (header->version) {
			case 0x251: // Black ops 3
				fastFileSize = 0x248;
				decompressedSizeLoc = 0x90;
				fastfileNameLoc = 0xF8;
				blockSizeLoc = 0xA8;
				ctx.blocksCount = 10;
				break;
			case 0x265: // Black ops 4 Dev
				fastFileSize = 0x638;
				decompressedSizeLoc = 0x490;
				fastfileNameLoc = 0x4E8;
				aesIVLoc = 0x628;
				aesIVSize = 16;
				blockSizeLoc = 0x4A8;
				ctx.blocksCount = 8;
				break;
			//case 0x27E:
				fastFileSize = 0x830;
				ctx.blocksCount = 9;
				xhashType = true;

			case 0x27F: // Black Ops 4
				fastFileSize = 0x840;
				decompressedSizeLoc = 0x490;
				fastfileNameLoc = 0x6F0;
				blockSizeLoc = 0x4A8;
				ctx.blocksCount = 9;
				xhashType = true;
				break;
			default:
				throw std::runtime_error(std::format("Fast file version not supported: 0x{:x}", header->version));
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

			byte* aesIV{};
			if (aesIVLoc) {
				if (!aesIVSize) throw std::runtime_error("Missing aesIVSize value");
				reader.Goto(aesIVLoc);
				aesIV = reader.ReadPtr<byte>(aesIVSize);
			}

			if (opt.m_header) {
				LOG_INFO("{} size:0x{:x}", ctx.ffname, decompressedSize);
			}

			reader.Goto(blockSizeLoc);

			for (size_t i = 0; i < ctx.blocksCount; i++) {
				ctx.blockSizes[i].size = reader.Read<uint64_t>();
			}

			reader.Goto(fastFileSize);


			BCRYPT_ALG_HANDLE hAlgorithm{};
			BCRYPT_KEY_HANDLE hKey{};
			utils::CloseEnd hProvCE{ [hAlgorithm, hKey] {
				if (hKey) BCryptDestroyKey(hKey);
				if (hAlgorithm) BCryptCloseAlgorithmProvider(hAlgorithm, 0);
			} };

			if (header->encrypted) {
				if (!aesIV) {
					throw std::runtime_error(std::format("No aesIV location for this fast file version 0x{:x}", header->version));
				}
				NTSTATUS status;
				compatibility::acti::crypto_keys::AesKeyLocal* key{ compatibility::acti::crypto_keys::GetKeyByName(ctx.ffname) };

				if (!key) {
					throw std::runtime_error(std::format("Missing aes key for ff {}", ctx.ffname));
				}

				status = BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_AES_ALGORITHM, NULL, 0);
				if (status < 0) {
					throw std::runtime_error(std::format("Can't acquire algorithm provider {}", status));
				}

				struct Aes256Blob {
					BCRYPT_KEY_DATA_BLOB_HEADER header;
					BYTE key[32];
				};
				Aes256Blob blob{};
				blob.header.dwMagic = BCRYPT_KEY_DATA_BLOB_MAGIC;
				blob.header.dwVersion = 1;
				blob.header.cbKeyData = sizeof(blob.key);

				static_assert(sizeof(blob.key) == sizeof(key->key));
				std::memcpy(blob.key, key->key, sizeof(blob.key));


				status = BCryptImportKey(hAlgorithm, NULL, BCRYPT_KEY_DATA_BLOB, &hKey, NULL, 0, (PUCHAR)&blob, sizeof(blob), 0);
				if (status < 0) {
					throw std::runtime_error(std::format("Failed to import key {}", status));
				}
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
				idx++;

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
					DWORD comp{ block->compressedSize };
					//if (!CryptDecrypt(hKey, hHash, true, 0, blockBuff, &comp)) {
					//	throw std::runtime_error(std::format("Can't read decrypt chunk 0x{:x}", GetLastError()));
					//}
					DWORD decryptedDataSize{};
					NTSTATUS status = BCryptDecrypt(hKey, blockBuff, block->alignedSize, NULL, aesIV, (ULONG)aesIVSize, NULL, 0, &decryptedDataSize, BCRYPT_BLOCK_PADDING);
					
					if (status < 0) {
						throw std::runtime_error(std::format("Can't decrypt block {:x}", status));
					}

					LOG_TRACE("decryptedDataSize=0x{:x}", decryptedDataSize);
					continue;
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
					}

					if (bdiffHeader->size != bdiffHeaderSize) {
						throw std::runtime_error(std::format("Can't read patch file header size: 0x{:x} != 0x{:x}", bdiffHeader->size, bdiffHeaderSize));
					}

					if (std::memcmp(baseXFileHdr, header, fastFileSize)) {
						throw std::runtime_error("The patch file is for this fast file");
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

					struct BdiffStates {
						BDiffHeaderPost* bdiffHeader{};
						byte* destWindow{};
						size_t destWindowSize{};
						size_t patchWindowOffsetLast{};
						size_t destWindowLastSize{};
						core::bytebuffer::ByteBuffer* ffbb{};
						core::bytebuffer::ByteBuffer* fdbb{};

						std::vector<byte> destData{};

						void SyncData() {
							if (!destWindowLastSize) return;

							LOG_TRACE("Sync 0x{:x} bytes", destWindowLastSize);
							utils::WriteValue(destData, destWindow, destWindowLastSize);

							destWindowLastSize = 0;
						}
					} bdiffStates{};

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

					bdiffStates.patchWindowOffsetLast = 0;

					bdiff::diffInfo diffInfo{};
					diffInfo.state = &bdiffStates;

					do {
						if (!bdiffStates.fdbb->CanRead(0x400)) {
							break; // can't read header
						}
						LOG_TRACE("Pre bdiff");
						if (!bdiff::bdiff_internal(&diffInfo,
							[](void* data, size_t offset, size_t size) -> uint8_t* {
								BdiffStates& bdiffStates{ *(BdiffStates*)data };
								// vcSourceCB_t
								bdiffStates.ffbb->Goto(offset);
								if (!bdiffStates.ffbb->CanRead(size)) {
									hook::error::DumpStackTraceFrom();
									throw std::runtime_error(std::format("vcSourceCB_t: read too much at 0x{:x}/0x{:x}", bdiffStates.ffbb->Loc(), size));
								}
								LOG_TRACE("vcSourceCB_t: read 0x{:x}:0x{:x}", bdiffStates.ffbb->Loc(), size);
								return bdiffStates.ffbb->ReadPtr<uint8_t>(size);
							},
							[](void* data, size_t offset, size_t size, size_t* pOffset) -> uint8_t* {
								BdiffStates& bdiffStates{ *(BdiffStates*)data };
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
							[](void* data, size_t size) -> uint8_t* {
								BdiffStates& bdiffStates{ *(BdiffStates*)data };
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
							throw std::runtime_error("bdiff error");
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

	utils::ArrayAdder<T78FFDecompressor, fastfile::FFDecompressor> arr{ fastfile::GetDecompressors() };
}