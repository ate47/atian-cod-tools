#include <includes.hpp>
#include <cli/clicolor.hpp>
#include <hook/memory.hpp>
#include <hook/error.hpp>
#include <tools/fastfile.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/ff/fastfile_packed.hpp>
#include <games/bo4/pool.hpp>
#include <utils/compress_utils.hpp>
#include <utils/data_utils.hpp>
#include <tools/ff/fastfile_bdiff.hpp>

namespace {
	enum IWFFVersion {
		IWFV_MW19 = 0x0B,
		IWFV_MW22 = 0x17,
		IWFV_MW23 = 0x18,
		IWFV_BO6 = 0x19,


		IWFV_MW19_PATCH = 0x06,
		IWFV_BO6_PATCH = 0x12,
	};

	constexpr char FFMagicType(byte* b) {
		return b[5];
	}

	struct DB_FFHeader {
		byte magic[8];
		unsigned int headerVersion;
		unsigned int xfileVersion;
		uint32_t flags;
		uint32_t unk14;
		uint32_t unk18;
		uint32_t unk1c;
		uint32_t unk20;
		uint32_t unk24;
		uint32_t unk28;
		uint32_t unk2c;
		uint32_t unk30;
		uint32_t unk34;
	};
	static_assert(sizeof(DB_FFHeader) == 0x38);

	struct EncryptionHeader {
		unsigned int isEncrypted;
		unsigned __int8 IV[16];
	};
	static_assert(sizeof(EncryptionHeader) == 0x14);

	struct DB_FFHeaderMW19 {
		byte magic[8];
		uint32_t headerVersion;
		uint32_t xfileVersion;
		uint32_t flags;
		uint32_t fileSize;
		uint32_t unk18;
		uint32_t unk1c;
		uint64_t size;
		uint64_t preloadWalkSize;
		uint64_t blockSize[8];
		EncryptionHeader encryption;
	}; static_assert(sizeof(DB_FFHeaderMW19) == 0x88);

	struct DB_FFHeaderMWII {
		byte magic[8];
		unsigned int headerVersion;
		unsigned int xfileVersion;
		uint32_t flags;
		uint32_t fileSize;
		uint64_t timestamp;
		uint64_t unk20;
		uint64_t unk28;
		uint64_t size;
		uint64_t preloadWalkSize;
		uint64_t blockSize[16];
		EncryptionHeader encryption;
	}; static_assert(sizeof(DB_FFHeaderMWII) == 0xd8);

	struct DB_FFHeaderMWIII {
		byte magic[8];
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
	}; static_assert(sizeof(DB_FFHeaderMWIII) == 0xe0);

	struct DB_FFHeaderBO6 {
		byte magic[8];
		unsigned int headerVersion;
		unsigned int xfileVersion;
		uint32_t flags;
		uint32_t fileSize;
		uint64_t unk18;
		uint32_t unk20;
		uint32_t unk24;
		uint64_t unk28;
		uint64_t unk30;
		uint64_t size;
		uint64_t preloadWalkSize;
		uint64_t blockSize[16];
		EncryptionHeader encrypt;

		void Print() {
			LOG_INFO("size:0x{:x}", size);
			LOG_INFO("preloadWalkSize:0x{:x}", preloadWalkSize);
			LOG_INFO("Blocks: {}", utils::data::ArrayAsString<uint64_t>(blockSize, ARRAYSIZE(blockSize), ",", "", "", [](const uint64_t& blk) { return std::format("0x{:x}", blk); }));
			LOG_INFO("isEncrypted: {}", encrypt.isEncrypted ? "true" : "false");
		}
	}; static_assert(sizeof(DB_FFHeaderBO6) == 0xe0);

	union IWFastFileHeader {
		DB_FFHeader header;
		DB_FFHeaderMW19 mw19;
		DB_FFHeaderMWII mwii;
		DB_FFHeaderMWIII mwiii;
		DB_FFHeaderBO6 bo6;
		byte __size[0xe0]{};
	}; static_assert(sizeof(IWFastFileHeader) == 0xe0);

	// bo6  "48 83 EC 28 FF C9 83"
	// mw23 "48 83 EC 28 FF C9 83 F9 10 77"
	void DecompressBlock(fastfile::FastFileIWCompression type, const void* src, void* dest, unsigned int srcLen, unsigned int destLen) {
		utils::compress::CompressionAlgorithm alg{ fastfile::GetFastFileCompressionAlgorithm(type) };
		if (!utils::compress::Decompress(alg, dest, destLen, src, srcLen)) {
			throw std::runtime_error(std::format("Can't decompress {} data", fastfile::GetFastFileCompressionName(type)));
		}
	}

	void PrintHeader(DB_FFHeader* header, const char* title, bool ffHeader) {
		char type{ FFMagicType(header->magic) };
		LOG_INFO("---- {}{} header {}{} ----", cli::clicolor::COLOR_RED, ffHeader ? "ff" : "fp", title, cli::clicolor::CD_RESET);
		LOG_INFO("version: 0x{:x} / 0x{:x}", header->headerVersion, header->xfileVersion);
		LOG_INFO("flags: 0x{:x}, plt: {}", header->flags, type);

		LOG_INFO("unk14:0x{:x}", header->unk14);
		LOG_INFO("unk18:0x{:x}({}) / unk1c:0x{:x}/({})", header->unk18, header->unk18, header->unk1c, header->unk1c);
		LOG_INFO("unk20:0x{:x}({}) / unk24:0x{:x}/({})", header->unk20, header->unk20, header->unk24, header->unk24);
		LOG_INFO("unk28:0x{:x}({}) / unk2c:0x{:x}/({})", header->unk28, header->unk28, header->unk2c, header->unk2c);
		LOG_INFO("unk30:0x{:x}({}) / unk34:0x{:x}/({})", header->unk30, header->unk30, header->unk34, header->unk34);

		if (ffHeader) {
			switch (header->headerVersion) {
			case IWFV_BO6:
				switch (header->xfileVersion) {
				case 1:
					((IWFastFileHeader*)header)->bo6.Print();
					return;
				}
				break;
			}
			LOG_WARNING("Unknown header data");
		}
	}

	struct DBUnpackHeaderCtxLoadData {
		//uint64_t unk0;
		//char** unk8;
		//char** unk10;
		core::bytebuffer::ByteBuffer* reader{};
	};
	struct DBUnpackHeaderCtxFuncs {
		bool(__fastcall* f1)(DBUnpackHeaderCtxLoadData* data, void** pData, __int64* pToRead, bool* pErrorState);
		void(__fastcall* unkf2)(void* a1, void* a2);
		void(__fastcall* unkf3)(void* a1, void* a2);
		void(__fastcall* nulled_f4)(DBUnpackHeaderCtxLoadData* data);
	};
	struct DBUnpackHeaderCtxLoad {
		DBUnpackHeaderCtxFuncs* vtable;
		DBUnpackHeaderCtxLoadData data;
	};

	namespace {
		bool f1(DBUnpackHeaderCtxLoadData* data, void** pData, __int64* pToRead, bool* pErrorState) {
			data->reader->Read(*pData, *pToRead);
			return true;
		}
		void nulled_f4(void* ctx) {}


		DBUnpackHeaderCtxFuncs dbUnpackHeaderCtxFuncs{ .f1 = f1 };
	}

	class IWFFDecompressor : public fastfile::FFDecompressor {
		hook::module_mapper::Module game{};

		bool (*DB_UnpackHeader)(void* data, const char* ffname, bool* error, DBUnpackHeaderCtxLoad* funcs) {};
		std::unique_ptr<byte[]> decryptBuffer;

	public:
		IWFFDecompressor() : fastfile::FFDecompressor("IW", 0x3030317066665749, 0xFFFFFF00FFFFFFFF) {}

		void Init(fastfile::FastFileOption& opt) override {
			//DB_UnpackHeader = game->ScanSingle("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 88 B4").GetPtr<decltype(DB_UnpackHeader)>();
			//game->Redirect("48 89 5C 24 ? 55 56 57 B8 ? ? ? ? E8 ? ? ? ? 48 2B E0 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 48 8B EA", []() { return true; });
			//decryptBuffer = std::make_unique<byte[]>(0x5000); // 0x4928, but maybe it'll change by few bytes
		}

		void LoadFastFile(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx, std::vector<byte>& ffdata) {
			if (!reader.CanRead(sizeof(DB_FFHeader))) {
				throw std::runtime_error("Can't read XFile header");
			}
			std::filesystem::path fpfile;
			std::vector<byte> fileFPBuff{};
			bool hasFdFile{};
			if (opt.m_fd) {
				fpfile = ctx.file;
				fpfile.replace_extension(".fp");
				if (opt.ReadFile(fpfile.string(), fileFPBuff)) {
					hasFdFile = true;
				}
				else {
					if (opt.m_fdIgnoreMissing) {
						throw std::runtime_error(std::format("Can't read {}", fpfile.string()));
					}
					fileFPBuff.clear();
				}
			}


			std::filesystem::path ffnamet{ ctx.file };
			ffnamet.replace_extension();
			ffnamet = ffnamet.filename();
			std::string ffnamets{ ffnamet.string() };
			sprintf_s(ctx.ffname, "%s", ffnamets.data());

			hashutils::Add(ctx.ffname, true, true);
			

			DB_FFHeader* header{ reader.Ptr<DB_FFHeader>() };

			enum SecureType {
				ST_NONE = 0,
				ST_MW19,
				ST_MW22
			};

			IWFastFileHeader ffHeader{};
			size_t ffHeaderSize{};
			SecureType secureType{};
			size_t endSize{ std::string::npos };

			utils::compress::CompressionAlgorithm alg{};

			ctx.hasGSCBin = header->headerVersion <= IWFV_MW22;
			ctx.gscPlatform = tool::gsc::opcode::PLATFORM_PC;

			switch (header->headerVersion) {
			case IWFV_MW19: {
				ffHeaderSize = sizeof(ffHeader.mw19);
				reader.Read(&ffHeader.mw19, sizeof(ffHeader.mw19));
				secureType = ST_MW19;
				ctx.blocksCount = opt.handler && opt.handler->forceNumXBlocks ? opt.handler->forceNumXBlocks : 8;
				endSize = ffHeader.mw19.size;

				uint64_t* blockSizes{ ffHeader.mw19.blockSize };
				for (size_t i = 0; i < ctx.blocksCount; i++) {
					ctx.blockSizes[i].size = blockSizes[i];
				}

				break;
			}
			case IWFV_MW22: {
				ffHeaderSize = sizeof(ffHeader.mwii);
				reader.Read(&ffHeader.mwii, sizeof(ffHeader.mwii));
				secureType = ST_MW22;
				ctx.blocksCount = 16;

				uint64_t* blockSizes{ ffHeader.mwii.blockSize };
				for (size_t i = 0; i < ctx.blocksCount; i++) {
					ctx.blockSizes[i].size = blockSizes[i];
				}
				break;
			}
			case IWFV_MW23: {
				ffHeaderSize = sizeof(ffHeader.mwiii);
				reader.Read(&ffHeader.mwiii, sizeof(ffHeader.mwiii));
				secureType = ST_MW22;
				ctx.blocksCount = 16;

				uint64_t* blockSizes{ ffHeader.mwiii.blockSize };
				for (size_t i = 0; i < ctx.blocksCount; i++) {
					ctx.blockSizes[i].size = blockSizes[i];
				}

				break;
			}
			case IWFV_BO6: {
				ffHeaderSize = sizeof(ffHeader.bo6);
				reader.Read(&ffHeader.bo6, sizeof(ffHeader.bo6));
				secureType = ST_MW22;
				ctx.blocksCount = 16;

				uint64_t* blockSizes{ ffHeader.bo6.blockSize };
				for (size_t i = 0; i < ctx.blocksCount; i++) {
					ctx.blockSizes[i].size = blockSizes[i];
				}

				break;
			}
			default:
				throw std::runtime_error(std::format("version not supported 0x{:x}", header->headerVersion));
			}

			if (opt.m_header) {
				PrintHeader(&ffHeader.header, "ff::header", true);
			}

			byte data[4];
			bool secure{};
			switch (secureType) {
			case ST_MW19: {
				constexpr size_t secureChunkSize = 0x800000;
				constexpr size_t rsaBlockSize = 0x4000;
				bool found{};
				while (reader.CanRead(sizeof(uint32_t))) {
					if (*reader.Ptr<uint32_t>() == 0x43574902) {
						found = true;
						break;
					}
					reader.Skip(1);
				}
				if (!found) {
					throw std::runtime_error("can't find iwc");
				}
				size_t rsaEnd{ reader.Loc() };
				if (rsaEnd > rsaBlockSize * 2) {
					// go at rsa start
					reader.Goto(rsaEnd - rsaBlockSize * 2);
					if (reader.Read<uint32_t>() == 0x66665749) {
						secure = true;
						reader.Skip(0x7ffc);
						if (reader.Read<uint32_t>() != 0x43574902) {
							throw std::runtime_error(std::format("invalid iwc after RSA block at 0x{:x}", reader.Loc()));
						}

						size_t dataStart{ reader.Loc() + 4 };
						reader.Goto(dataStart - 8);// iwc + data
						byte* patchData{ reader.Ptr<byte>() };

						if (reader.CanRead(secureChunkSize + rsaBlockSize)) {
							// skip first chunk + rsa block
							reader.Skip(secureChunkSize + rsaBlockSize);
							LOG_TRACE("patch first chunk 0x{:x}", secureChunkSize);
							patchData += secureChunkSize;

							while (reader.CanRead(1)) {
								size_t chunkSize{ std::min<size_t>(secureChunkSize, reader.Remaining()) };
								byte* chunk{ reader.ReadPtr<byte>(chunkSize) };
								std::memmove(patchData, chunk, chunkSize);
								patchData += chunkSize;
								LOG_TRACE("patch chunk 0x{:x} <- 0x{:x}", chunkSize, reader.Loc());
								if (chunkSize == secureChunkSize && reader.CanRead(rsaBlockSize)) {
									reader.Skip(rsaBlockSize);
								}
							}

							reader.Goto(0);
							byte* start{ reader.Ptr<byte>() };
							size_t newSize{ (size_t)(patchData - start) };
							LOG_TRACE("new size 0x{:x}", newSize);
							reader = { start, newSize };
						}
					}
					else {
						LOG_TRACE("no rsa header");
					}
				}
				reader.Goto(rsaEnd + 8); // iwc + data
				reader.Read(data, sizeof(data));
				break;
			}
			case ST_MW22: {
				bool found{};
				while (reader.CanRead(sizeof(uint32_t))) {
					if (*reader.Ptr<uint32_t>() == 0x43574902) {
						found = true;
						break;
					}
					reader.Skip(1);
				}
				if (!found) {
					throw std::runtime_error("can't find iwc");
				}
				reader.Skip<uint32_t>(); // skip IWC
				if (reader.Read<uint32_t>() == 0x66665749) {
					secure = true;
					reader.Skip(0x8000);
				}
				reader.Read(data, sizeof(data));
				break;
			}
			default:
				throw std::runtime_error(std::format("no secure handler for 0x{:x}", header->headerVersion));
			}

			if (data[3] >= fastfile::FastFileIWCompression::IWFFC_COUNT) {
				throw std::runtime_error("Can't find compression type");
			}
			else {
				alg = fastfile::GetFastFileCompressionAlgorithm((fastfile::FastFileIWCompression)data[3]);
			}

			LOG_DEBUG("loaded header secure:{}, alg:{}({})  0x{:x}", secure, alg, (int)data[3], reader.Loc());

			size_t offset{};
			size_t count{};
			ffdata.clear();

			while (reader.CanRead(sizeof(uint32_t) * 3)) {
				size_t id{ count++ };

				if (secure && secureType == ST_MW22) {
					if ((id & 0x1ff) == 0x1ff) {
						if (!reader.CanRead(0x4000)) break;
						reader.Skip(0x4000);
					}
				}
				if (endSize != std::string::npos && endSize <= offset) {
					if (endSize != offset) {
						LOG_WARNING("extracted more than expected 0x{:x} != 0x{:x}", endSize, offset);
					}
					break;
				}

				size_t loc{ reader.Loc() };
				fastfile::XBlockCompressionBlockHeader* block{ reader.ReadPtr<fastfile::XBlockCompressionBlockHeader>() };

				if (!block->compressedSize) break; // done

				uint32_t alignedSize{ utils::Aligned<uint32_t>(block->compressedSize) };

				LOG_TRACE("Decompressing {} block#{:x} 0x{:x} (0x{:x}/0x{:x} -> 0x{:x}) encryptionCTR:0x{:x}",
					utils::compress::GetCompressionName(alg), id, loc, block->compressedSize, alignedSize, block->uncompressedSize, block->encryptionCTR
				);

				byte* blockBuff{ reader.ReadPtr<byte>(alignedSize) };

				// allc data
				ffdata.resize(offset + block->uncompressedSize);

				byte* decompressed{ &ffdata[offset] };
				offset += block->uncompressedSize;

				int r{ utils::compress::Decompress2(alg, decompressed, block->uncompressedSize, blockBuff, block->compressedSize) };
				if (r < 0) {
					throw std::runtime_error(std::format("Can't decompress block 0x{:x}: {} ({})", loc, utils::compress::DecompressResultName(r), r));
				}

			}

			if (opt.dump_decompressed) {
				std::filesystem::path of{ ctx.file };
				std::filesystem::path decfile{ opt.m_output / of.filename() };

				decfile.replace_extension(".ff.dec");

				std::filesystem::create_directories(decfile.parent_path());
				if (!utils::WriteFile(decfile, ffdata)) {
					LOG_ERROR("Can't dump {}", decfile.string());
				}
				else {
					LOG_INFO("Dump ff into {}", decfile.string());
				}
			}

			if (opt.m_header) {
				LOG_INFO("Decompressed size: 0x{:x}", ffdata.size());
			}

			if (hasFdFile) {
				LOG_TRACE("loading patch {}", fpfile.string());
				core::bytebuffer::ByteBuffer fpreader{ fileFPBuff };

				if (!fpreader.CanRead(8) || *fpreader.Ptr<uint64_t>() != 0x3030316466665749) {
					throw std::runtime_error(std::format("Can't read {}: bad magic", fpfile.string()));
				}
				if (!fpreader.CanRead(sizeof(DB_FFHeader))) {
					throw std::runtime_error("Can't read XFile header");
				}

				DB_FFHeader* fpHeader{ fpreader.ReadPtr<DB_FFHeader>() };
				IWFastFileHeader prevHeader{};
				IWFastFileHeader newHeader{};

				// load patch headers
				switch (fpHeader->headerVersion) {
				case IWFV_MW19_PATCH:
				case IWFV_BO6_PATCH: {
					fpreader.Read(&prevHeader, ffHeaderSize);
					fpreader.Read(&newHeader, ffHeaderSize);

					if (std::memcmp(&prevHeader, &ffHeader, ffHeaderSize)) {
						throw std::runtime_error("The patch file is not for this fast file");
					}
					break;
				}
				default:
					throw std::runtime_error(std::format("patch version not supported 0x{:x}", fpHeader->headerVersion));
				}
				size_t endSize{ std::string::npos };
				uint64_t* blockSizes{};

				switch (header->headerVersion) {
				case IWFV_MW19: {
					ctx.blocksCount = opt.handler && opt.handler->forceNumXBlocks ? opt.handler->forceNumXBlocks : 8;
					//endSize = ...;
					blockSizes = newHeader.mw19.blockSize;
					break;
				}
				case IWFV_MW22: {
					ctx.blocksCount = 16;
					blockSizes = newHeader.mwii.blockSize;
					break;
				}
				case IWFV_MW23: {
					ctx.blocksCount = 16;
					blockSizes = newHeader.mwiii.blockSize;
					break;
				}
				}

				if (blockSizes) {
					for (size_t i = 0; i < ctx.blocksCount; i++) {
						ctx.blockSizes[i].size = blockSizes[i];
					}
				}

				if (opt.m_header) {
					PrintHeader(fpHeader, "fp::header", false);
				}

				if (opt.m_header) {
					PrintHeader(&prevHeader.header, "fp::prevHeader", true);
				}

				if (opt.m_header) {
					PrintHeader(&newHeader.header, "fp::newHeader", true);
				}

				// goto data
				switch (header->headerVersion) { 
				case IWFV_MW19: {
					constexpr size_t secureChunkSize = 0x800000;
					constexpr size_t rsaBlockSize = 0x4000;
					bool found{};
					while (fpreader.CanRead(sizeof(uint32_t))) {
						if (*fpreader.Ptr<uint32_t>() == 0x43574902) {
							found = true;
							break;
						}
						fpreader.Skip(1);
					}
					if (!found) {
						throw std::runtime_error("can't find iwc");
					}

					size_t rsaEnd{ fpreader.Loc() };
					if (rsaEnd > rsaBlockSize * 2) {
						// go at rsa start
						fpreader.Goto(rsaEnd - rsaBlockSize * 2);
						if (fpreader.Read<uint32_t>() == 0x66665749) {
							secure = true;
							fpreader.Skip(0x7ffc);
							if (fpreader.Read<uint32_t>() != 0x43574902) {
								throw std::runtime_error(std::format("invalid iwc after RSA block at 0x{:x}", fpreader.Loc()));
							}

							size_t dataStart{ fpreader.Loc() + 4 };
							fpreader.Goto(dataStart - 8);// iwc + data
							byte* patchData{ fpreader.Ptr<byte>() };

							if (fpreader.CanRead(secureChunkSize + rsaBlockSize)) {
								// skip first chunk + rsa block
								fpreader.Skip(secureChunkSize + rsaBlockSize);
								LOG_TRACE("patch first chunk 0x{:x}", secureChunkSize);
								patchData += secureChunkSize;

								while (fpreader.CanRead(1)) {
									size_t chunkSize{ std::min<size_t>(secureChunkSize, fpreader.Remaining()) };
									byte* chunk{ fpreader.ReadPtr<byte>(chunkSize) };
									std::memmove(patchData, chunk, chunkSize);
									patchData += chunkSize;
									LOG_TRACE("patch chunk 0x{:x} <- 0x{:x}", chunkSize, fpreader.Loc());
									if (chunkSize == secureChunkSize && fpreader.CanRead(rsaBlockSize)) {
										fpreader.Skip(rsaBlockSize);
									}
								}

								fpreader.Goto(0);
								byte* start{ fpreader.Ptr<byte>() };
								size_t newSize{ (size_t)(patchData - start) };
								LOG_TRACE("new size 0x{:x}", newSize);
								fpreader = { start, newSize };
							}
						}
						else {
							LOG_TRACE("no rsa header");
						}
					}
					fpreader.Goto(rsaEnd + 8); // iwc + data
					fpreader.Read(data, sizeof(data));
					break;
				}
				case IWFV_MW22:
				case IWFV_MW23:
				case IWFV_BO6: {
					bool found{};
					while (fpreader.CanRead(sizeof(uint32_t))) {
						if (*fpreader.Ptr<uint32_t>() == 0x43574902) {
							found = true;
							break;
						}
						fpreader.Skip(1);
					}
					if (!found) {
						LOG_WARNING("can't find patch iwc, ignore");
						return;
					}
					fpreader.Skip<uint32_t>(); // skip IWC

					secure = fpreader.Read<uint32_t>() == 0x66665749;
					if (secure) {
						fpreader.Skip(0x8000);
					}
					byte data[4];
					fpreader.Read(data, sizeof(data));


					if (data[3] >= fastfile::FastFileIWCompression::IWFFC_COUNT) {
						throw std::runtime_error("Can't find patch compression type");
					}
					else {
						alg = fastfile::GetFastFileCompressionAlgorithm((fastfile::FastFileIWCompression)data[3]);
					}

					LOG_DEBUG("loaded bo6 patch secure:{}, alg:{}({})  0x{:x}", secure, alg, (int)data[3], fpreader.Loc());
					break;
				}
				default:
					throw std::runtime_error(std::format("version not supported 0x{:x}", header->headerVersion));
				}

				// load fp data
				std::vector<byte> fpdata{};

				size_t countfp{};
				size_t offsetpatch{};
				while (fpreader.CanRead(sizeof(uint32_t) * 3)) {
					size_t id{ countfp++ };

					if (secure && secureType == ST_MW22) {
						if ((id & 0x1ff) == 0x1ff) {
							if (!fpreader.CanRead(0x4000)) break;
							fpreader.Skip(0x4000);
						}
					}

					size_t loc{ fpreader.Loc() };

					fastfile::XBlockCompressionBlockHeader* block{ fpreader.ReadPtr<fastfile::XBlockCompressionBlockHeader>() };

					if (!block->compressedSize) break; // done

					if (secureType == ST_MW19 && block->encryptionCTR) {
						break; // fixme: find better
					}

					uint32_t alignedSize{ utils::Aligned<uint32_t>(block->compressedSize) };

					LOG_TRACE("Decompressing {} block#{:x} 0x{:x} (0x{:x}/0x{:x} -> 0x{:x}) encryptionCTR:0x{:x}",
						utils::compress::GetCompressionName(alg), id, loc, block->compressedSize, alignedSize, block->uncompressedSize, block->encryptionCTR
					);

					byte* blockBuff{ fpreader.ReadPtr<byte>(alignedSize) };

					// allc data
					fpdata.resize(offsetpatch + block->uncompressedSize);

					byte* decompressed{ &fpdata[offsetpatch] };
					offsetpatch += block->uncompressedSize;

					int r{ utils::compress::Decompress2(alg, decompressed, block->uncompressedSize, blockBuff, block->compressedSize) };
					if (r < 0) {
						throw std::runtime_error(std::format("Can't decompress block 0x{:x}: {} ({})", loc, utils::compress::DecompressResultName(r), r));
					}

				}

				if (opt.dump_decompressed) {
					std::filesystem::path of{ ctx.file };
					std::filesystem::path decfile{ opt.m_output / of.filename() };

					decfile.replace_extension(".fp.dec");

					std::filesystem::create_directories(decfile.parent_path());
					if (!utils::WriteFile(decfile, fpdata)) {
						LOG_ERROR("Can't dump {}", decfile.string());
					}
					else {
						LOG_INFO("Dump fp patch into {}", decfile.string());
					}
				}

				LOG_TRACE("Loaded {} 0x{:x} bytes", fpdata.size(), fpdata.size());
				LOG_TRACE("rem: sd:0x{:x} pd:0x{:x}", reader.Remaining(), fpreader.Remaining());

				core::bytebuffer::ByteBuffer ffbb{ ffdata };
				core::bytebuffer::ByteBuffer fdbb{ fpdata };

				ffdata = fastfile::bdiff::bdiff(&ffbb, &fdbb, fastfile::bdiff::BDiffType::BDT_IW);

				if (opt.dump_decompressed) {
					std::filesystem::path of{ ctx.file };
					std::filesystem::path decfile{ opt.m_output / of.filename() };

					decfile.replace_extension(".patched.dec");

					std::filesystem::create_directories(decfile.parent_path());
					if (!utils::WriteFile(decfile, ffdata)) {
						LOG_ERROR("Can't dump {}", decfile.string());
					}
					else {
						LOG_INFO("Dump patched data into {}", decfile.string());
					}
				}


			}
			if (opt.m_header) {
				LOG_INFO("Decompressed size: 0x{:x}", ffdata.size());
			}
		}

	};
	utils::ArrayAdder<IWFFDecompressor, fastfile::FFDecompressor> arr{ fastfile::GetDecompressors() };
}