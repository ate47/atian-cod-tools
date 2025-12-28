#include <includes.hpp>
#include <cli/clicolor.hpp>
#include <hook/memory.hpp>
#include <hook/error.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/fastfile_flexible.hpp>
#include <games/bo4/pool.hpp>
#include <utils/compress_utils.hpp>
#include <utils/data_utils.hpp>
#include <tools/fastfile/fastfile_bdiff.hpp>

namespace {
	constexpr uint64_t IW_FF_MAGIC_MASK = 0xFFFFFF00FFFFFFFF;
	constexpr uint64_t IW_FF_MAGIC = 0x3030310066665749;
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
	struct sha256Val {
		byte value[0x20];
	};
	static_assert(sizeof(sha256Val) == 0x20);

	struct XBlockCompressDataHeader {
		uint32_t unk0;
		byte unk4;
		byte unk5;
		byte unk6;
		fastfile::FastFileIWCompression compression;
	};
	static_assert(sizeof(XBlockCompressDataHeader) == 8);

	struct FS_Header {
		byte magic[8];
		uint32_t unk8;
		byte iv[0x20];
		byte signature[0x100];
		char name[0x40];
		uint32_t __pad16c;
		sha256Val rsaBlockHashes[1];
		byte __pad[0x3e70];
	};
	static_assert(sizeof(FS_Header) == 0x4000);

	struct FS_RSAChunk {
		sha256Val hashes[0x200];
	};
	static_assert(sizeof(FS_RSAChunk) == 0x4000);


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
		uint32_t headerVersion;
		uint32_t xfileVersion;
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

	void WriteHeaderFile_(utils::OutFileCE& header, std::string&& str) {
		if (*header) {
			header << str << "\n";
		}
		else {
			LOG_INFO("{}", str);
		}
	}
#define WriteHeaderFile(...) WriteHeaderFile_(hos, std::format(__VA_ARGS__))


	void PrintHeader(utils::OutFileCE& hos, DB_FFHeader* header, const char* title, bool ffHeader) {
		char type{ FFMagicType(header->magic) };
		WriteHeaderFile("---- {}{} header {}{} ----", cli::clicolor::COLOR_RED, ffHeader ? "ff" : "fp", title, cli::clicolor::CD_RESET);
		WriteHeaderFile("version: 0x{:x} / 0x{:x}", header->headerVersion, header->xfileVersion);
		WriteHeaderFile("flags: 0x{:x}, plt: {}", header->flags, type);

		if (ffHeader) {
			IWFastFileHeader& h{ *(IWFastFileHeader*)header };
			switch (header->headerVersion) {
			case IWFV_BO6:
				switch (header->xfileVersion) {
				case 1:
					WriteHeaderFile("size:0x{:x} fileSize:0x{:x} preloadWalkSize:0x{:x}", h.bo6.size, h.bo6.fileSize, h.bo6.preloadWalkSize);
					WriteHeaderFile("Blocks: {}", utils::data::ArrayAsString<uint64_t>(h.bo6.blockSize, ARRAYSIZE(h.bo6.blockSize), ",", "", "", [](const uint64_t& blk) { return std::format("0x{:x}", blk); }));
					WriteHeaderFile("isEncrypted: {}", h.bo6.encrypt.isEncrypted ? "true" : "false");
					break;
				default:
					LOG_WARNING("Unknown header data: headerVersion=0x{:x}/xfileVersion=0x{:x}", header->headerVersion, header->xfileVersion);
					break;
				}
				break;
			case IWFV_MW19:
				WriteHeaderFile("size:0x{:x} fileSize:0x{:x} preloadWalkSize:0x{:x}", h.mw19.size, h.mw19.fileSize, h.mw19.preloadWalkSize);
				return;
			case IWFV_MW23:
				WriteHeaderFile("size:0x{:x} fileSize:0x{:x} preloadWalkSize:0x{:x}", h.mwiii.size, h.mwiii.fileSize, h.mwiii.preloadWalkSize);
				WriteHeaderFile("timestamp:{}, unk20:0x{:x}({}) / unk24:0x{:x}/({})", h.mwiii.timestamp, h.mwiii.unk20, h.mwiii.unk20, h.mwiii.unk24, h.mwiii.unk24);
				WriteHeaderFile("unk28:0x{:x} unk30:0x{:x}", h.mwiii.unk28, h.mwiii.unk30);
				WriteHeaderFile("Blocks: {}", utils::data::ArrayAsString<uint64_t>(h.mwiii.blockSize, ARRAYSIZE(h.mwiii.blockSize), ",", "", "", [](const uint64_t& blk) { return std::format("0x{:x}", blk); }));
				WriteHeaderFile("isEncrypted: {}", h.mwiii.encrypt.isEncrypted ? "true" : "false");
				return;
			default:
				WriteHeaderFile("unk14:0x{:x}", header->unk14);
				WriteHeaderFile("unk18:0x{:x}({}) / unk1c:0x{:x}/({})", header->unk18, header->unk18, header->unk1c, header->unk1c);
				WriteHeaderFile("unk20:0x{:x}({}) / unk24:0x{:x}/({})", header->unk20, header->unk20, header->unk24, header->unk24);
				WriteHeaderFile("unk28:0x{:x}({}) / unk2c:0x{:x}/({})", header->unk28, header->unk28, header->unk2c, header->unk2c);
				WriteHeaderFile("unk30:0x{:x}({}) / unk34:0x{:x}/({})", header->unk30, header->unk30, header->unk34, header->unk34);
				LOG_WARNING("Unknown header data: headerVersion=0x{:x}/xfileVersion=0x{:x}", header->headerVersion, header->xfileVersion);
				break;
			}
		}
	}

	void ReadTafHeader(utils::OutFileCE& hos, core::bytebuffer::ByteBuffer& reader, const char* type, const char* filename) {
		if (reader.CanRead(sizeof(uint32_t))) {
			WriteHeaderFile("---- {}{} pack header{} ----", cli::clicolor::COLOR_RED, type, cli::clicolor::CD_RESET);
			uint32_t tafMagic{ *reader.Ptr<uint32_t>() };

			if (fastfile::flexible::IsFlexibleDataMagic(tafMagic)) {

				fastfile::flexible::FlexibleFastFileReader freader{};
				fastfile::flexible::TAFASecureInfo secure{};
				secure.fastfileName = filename;
				secure.rsaKeyName = "iw";
				// flexible data
				freader.ReadHeader(reader, &secure);

				for (fastfile::flexible::FlexibleFastFileChunk& chunk : freader) {
					switch (chunk.type) {
					case fastfile::flexible::ST_IW_FASTFILE_CHECKSUM: {
						fastfile::flexible::PFFIWChecksums& checksums{ chunk.GetVal<fastfile::flexible::PFFIWChecksums>() };
						WriteHeaderFile(
							"ff header iw checksums: loaded:{} 0x{:x} 0x{:x} 0x{:x} 0x{:x}",
							checksums.loaded ? "true" : "false",
							checksums.checksum[0], checksums.checksum[1], checksums.checksum[2], checksums.checksum[3]
						);
						break;
					}
					case fastfile::flexible::ST_IW_DEPS: {
						fastfile::flexible::PFFIWDeps& deps{ chunk.GetVal<fastfile::flexible::PFFIWDeps>() };
						WriteHeaderFile("ff header iw deps: 0x{:x} 0x{:x}", deps.fastfileHash, deps.unk8);
						break;
					}
					case fastfile::flexible::ST_IW_UNK_0xc95c6b9c: {
						fastfile::flexible::PFFIW_0xc95c6b9c& iw0xc95c6b9c{ chunk.GetVal<fastfile::flexible::PFFIW_0xc95c6b9c>() };
						WriteHeaderFile("ff header iw 0xc95c6b9c: {} 0x{:x}", iw0xc95c6b9c.loaded ? "true" : "false", iw0xc95c6b9c.unk8);
						break;
					}
					default: {
						std::string view;
						switch (chunk.size) {
						case 1: view = std::format("0x{:x}", (int)chunk.GetVal<byte>()); break;
						case 2: view = std::format("0x{:x}", chunk.GetVal<uint16_t>()); break;
						case 4: view = std::format("0x{:x}", chunk.GetVal<uint32_t>()); break;
						case 8: view = std::format("0x{:x}", chunk.GetVal<uint64_t>()); break;
						default: {
							if (chunk.size & 7) {
								view = utils::data::AsHex(chunk.buffer, chunk.size);
							}
							else {
								view = utils::data::ArrayAsString<uint64_t>((uint64_t*)chunk.buffer, chunk.size / 8, ",", "", "", [](const uint64_t& v) { return std::format("0x{:x}", v); });
							}
							break;
						}
						}
						WriteHeaderFile("ff header iw 0x{:x}, size=0x{:x} {}", (uint32_t)chunk.type, chunk.size, view);
						break;
					}
					}
				}
			}
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
		IWFFDecompressor() : fastfile::FFDecompressor("IW", IW_FF_MAGIC, IW_FF_MAGIC_MASK) {}

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
			

			DB_FFHeader* header{ reader.Ptr<DB_FFHeader>() };

			enum SecureType {
				ST_NONE = 0,
				ST_MW19,
				ST_MW22
			};

			utils::OutFileCE hos{};

			if (opt.headerDump) {
				std::filesystem::path headerDumpLoc{ std::filesystem::path{opt.headerDump} / ctx.GetFFType() / std::format("{}.txt", ctx.ffname) };
				std::filesystem::create_directories(headerDumpLoc.parent_path());
				hos->open(headerDumpLoc);
				if (!hos) {
					throw std::runtime_error(std::format("Can't open {}", headerDumpLoc.string()));
				}
				LOG_INFO("dump header into {}...", headerDumpLoc.string());
			}

			IWFastFileHeader ffHeader{};
			SecureType secureType{};
			size_t endSize{ std::string::npos };

			utils::compress::CompressionAlgorithm alg{};

			ctx.hasGSCBin = header->headerVersion <= IWFV_MW22;
			ctx.gscPlatform = tool::gsc::opcode::PLATFORM_PC;

			switch (header->headerVersion) {
			case IWFV_MW19: {
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

			LOG_DEBUG("blocks sizes:");
			for (size_t i = 0; i < ctx.blocksCount; i++) {
				LOG_DEBUG("blocks[{}] = 0x{:x}", i, ctx.blockSizes[i].size);
			}

			if (opt.m_header) {
				PrintHeader(hos, &ffHeader.header, "ff::header", true);
				ReadTafHeader(hos, reader, "ff", ctx.ffname);
			}
			XBlockCompressDataHeader compressDataHeader{};
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
				reader.Goto(rsaEnd + 4); // iwc + data
				reader.Read(&compressDataHeader, sizeof(compressDataHeader));
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
				if (reader.CanRead(sizeof(uint32_t)) && *reader.Ptr<uint32_t>() == 0x66665749) {
					secure = true;
					reader.Skip(sizeof(FS_Header) + sizeof(FS_RSAChunk));
				}
				reader.Read(&compressDataHeader, sizeof(compressDataHeader));
				break;
			}
			default:
				throw std::runtime_error(std::format("no secure handler for 0x{:x}", header->headerVersion));
			}

			if (compressDataHeader.compression >= fastfile::FastFileIWCompression::IWFFC_COUNT) {
				throw std::runtime_error("Can't find compression type");
			}
			else {
				alg = fastfile::GetFastFileCompressionAlgorithm(compressDataHeader.compression);
			}

			LOG_DEBUG("loaded header secure:{}, alg:{}({})  0x{:x}", secure, alg, (int)compressDataHeader.compression, reader.Loc());

			size_t offset{};
			size_t count{};
			ffdata.clear();

			while (reader.CanRead(sizeof(uint32_t) * 3)) {
				size_t id{ count++ };

				if (secure && secureType == ST_MW22) {
					if ((id & 0x1ff) == 0x1ff) {
						if (!reader.CanRead(sizeof(FS_RSAChunk))) break;
						reader.Skip<FS_RSAChunk>();
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
				WriteHeaderFile("Decompressed size: 0x{:x}", ffdata.size());
			}

			if (hasFdFile) {
				LOG_TRACE("loading patch {}", fpfile.string());
				core::bytebuffer::ByteBuffer fpreader{ fileFPBuff };

				if (!fpreader.CanRead(8) || *fpreader.Ptr<uint64_t>() != 0x3030316466665749) {
					throw std::runtime_error(std::format("Can't read {}: bad magic", fpfile.string()));
				}
				if (!fpreader.CanRead(sizeof(DB_FFHeader) + sizeof(uint64_t))) { // fp header + prevHeader magic
					throw std::runtime_error("Can't read XFile header");
				}

				DB_FFHeader* fpHeader{ fpreader.ReadPtr<DB_FFHeader>() };
				IWFastFileHeader prevHeader{};
				IWFastFileHeader newHeader{};

				if ((*fpreader.Ptr<uint64_t>() & IW_FF_MAGIC_MASK) != IW_FF_MAGIC) {
					throw std::runtime_error("Invalid prevHeader magic");
				}

				uint64_t iwMagic{ IW_FF_MAGIC };
				uint64_t iwMagicMask{ IW_FF_MAGIC_MASK };

				size_t newHeaderLocation{ fpreader.FindMasked(&iwMagic, &iwMagicMask, sizeof(iwMagic), 8) };

				if (newHeaderLocation == std::string::npos) {
					throw std::runtime_error("Can't find newHeader magic");
				}

				size_t ffHeaderSize{ newHeaderLocation - fpreader.Loc() };

				LOG_DEBUG("ffHeaderSize: 0x{:x}", ffHeaderSize);

				if (ffHeaderSize > sizeof(IWFastFileHeader)) {
					throw std::runtime_error(std::format("Computed ffHeaderSize too big: 0x{:x}", ffHeaderSize));
				}

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

					LOG_DEBUG("patched blocks sizes:");
					for (size_t i = 0; i < ctx.blocksCount; i++) {
						LOG_DEBUG("blocks[{}] = 0x{:x}", i, ctx.blockSizes[i].size);
					}
				}

				if (opt.m_header) {
					PrintHeader(hos, fpHeader, "fp::header", false);
					PrintHeader(hos, &prevHeader.header, "fp::prevHeader", true);
					PrintHeader(hos, &newHeader.header, "fp::newHeader", true);
					ReadTafHeader(hos, fpreader, "fp", ctx.ffname);
				}

				if (!fpreader.CanRead(1)) {
					ffdata = {};
					return;
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
					fpreader.Goto(rsaEnd + 4); // iwc + data
					fpreader.Read(&compressDataHeader, sizeof(compressDataHeader));
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

					secure = fpreader.CanRead(sizeof(uint32_t)) && *fpreader.Ptr<uint32_t>() == 0x66665749;
					if (secure) {
						fpreader.Skip(sizeof(FS_Header) + sizeof(FS_RSAChunk));
					}
					fpreader.Read(&compressDataHeader, sizeof(compressDataHeader));


					if (compressDataHeader.compression >= fastfile::FastFileIWCompression::IWFFC_COUNT) {
						throw std::runtime_error("Can't find patch compression type");
					}
					else {
						alg = fastfile::GetFastFileCompressionAlgorithm(compressDataHeader.compression);
					}

					LOG_DEBUG("loaded bo6 patch secure:{}, alg:{}({})  0x{:x}", secure, alg, (int)compressDataHeader.compression, fpreader.Loc());
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
				WriteHeaderFile("Decompressed size: 0x{:x}", ffdata.size());
			}
		}

	};
	utils::ArrayAdder<IWFFDecompressor, fastfile::FFDecompressor> arr{ fastfile::GetDecompressors() };
}