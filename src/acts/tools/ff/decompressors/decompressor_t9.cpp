#include <includes.hpp>
#include <tools/fastfile.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <games/bo4/pool.hpp>
#include <tools/utils/data_utils.hpp>
#include <tools/utils/compress_utils.hpp>
#include <tools/compatibility/acti_crypto_keys.hpp>
#include <hook/error.hpp>
#include <hook/memory.hpp>
#include <games/bo4/t8_errors.hpp>
#include <wincrypt.h>

namespace {
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
		byte pad[640];
		char fastfileName[64];
		uint8_t signature[256];
		uint8_t aesIV[16];
	}; static_assert(sizeof(XFileData) == 0x538);

	void DumpHeader(XFileData& data, const char* title) {
		LOG_INFO("{}: v{} server:{}, comp:{}, plt:{} enc:{}, unk:{} bld:{}", 
			title, data.version, data.server ? "true" : "false",
			fastfile::GetFastFileCompressionName(data.compression), fastfile::GetFastFilePlatformName(data.platform),
			data.server ? "true" : "false", data.unk8, data.builderName
		);
	}

	struct XFileMagic {
		byte magic[4];
		uint32_t blocks;
	};
	void ErrorStub(uint32_t errcode) {
		throw std::runtime_error(games::bo4::errors::TranslateError(errcode));
	}

    static byte ff_magic[4]{ 'T', 'A', 'F', 'F' };
	XFileData& DecryptHeader(core::bytebuffer::ByteBuffer& headerBuffer) {
		XFileData tmp{};
		// to write the result inside the ff data at the end
		if (!headerBuffer.CanRead(sizeof(XFileData))) throw std::runtime_error("Can't read xfile header size");
		XFileData* res{ headerBuffer.Ptr<XFileData>() };
        byte* data{ (byte*)&tmp };
        __int64 v7; // rcx
        __int64 v8; // r8
        byte v9; // dl
        __int64 v10; // rax
        byte v11; // r9
        size_t v12; // r14d
        __int64 v14; // rbx
        byte* v15; // rcx
        uint32_t v17[2]; // [rsp+20h] [rbp-48h] BYREF
        XFileMagic magicData; // [rsp+28h] [rbp-40h] BYREF

        std::memset(data, 0, sizeof(XFileData));
        //read_func->stream_readskip(&magicData, 8i64, 0i64, &read_func->streamff);
        headerBuffer.Read(&magicData, sizeof(magicData));
        v7 = 0i64;
        v8 = 4i64;
        while (1) {
            v9 = magicData.magic[v7];
            v10 = v8;
            v11 = ff_magic[v7++];
            --v8;
            if (!v10) {
            LABEL_5:
                v12 = 0;
                if (!magicData.blocks) {
					std::memcpy(res, &tmp, sizeof(tmp));
                    return *res;
                }
                while (1) {
                    //read_func->stream_readskip(v17, 8i64, 0i64, &read_func->streamff);
                    headerBuffer.Read(&v17[0], sizeof(v17));

                    v14 = 0i64;
                    if (v17[0] > 0x39CB44F1) {
                        switch (v17[0]) {
                        case 0x66C65056u:
                            v14 = 96i64;
                            if (v17[1] <= 0x60)
                                v14 = v17[1];
                            v15 = data + 368;
                            goto LABEL_44;
                        case 0x6AEBB196u:
                            v14 = 96i64;
                            if (v17[1] <= 0x60)
                                v14 = v17[1];
                            v15 = data + 464;
                            goto LABEL_44;
                        case 0x8578C004:
                            v14 = 264i64;
                            if (v17[1] <= 0x108)
                                v14 = v17[1];
                            v15 = data + 80;
                            goto LABEL_44;
                        case 0xC7708CDA:
                            v14 = 336i64;
                            if (v17[1] <= 0x150)
                                v14 = v17[1];
                            v15 = data + 1000;
                            goto LABEL_44;
                        }
                    }
                    else {
                        switch (v17[0]) {
                        case 0x39CB44F1u:
                            v14 = 4i64;
                            if (v17[1] <= 4)
                                v14 = v17[1];
                            v15 = data;
                            goto LABEL_44;
                        case 0xF992DFEu:
                            v14 = 24i64;
                            if (v17[1] <= 0x18)
                                v14 = v17[1];
                            v15 = data + 344;
                            goto LABEL_44;
                        case 0x1CE68F50u:
                            v14 = 440i64;
                            if (v17[1] <= 0x1B8)
                                v14 = v17[1];
                            v15 = data + 560;
                            goto LABEL_44;
                        case 0x2C2381CFu:
                            v14 = 5i64;
                            if (v17[1] <= 5)
                                v14 = v17[1];
                            v15 = data + 4;
                            goto LABEL_44;
                        case 0x37F9D612u:
                            v14 = 64i64;
                            if (v17[1] <= 0x40)
                                v14 = v17[1];
                            v15 = data + 16;
                        LABEL_44:
                            //read_func->stream_readskip(v15, v14, 0i64, &read_func->streamff);
                            headerBuffer.Read(v15, v14);
                            break;
                        }
                    }
                    if (v14 != v17[1]) {
                        //read_func->stream_readskip(0i64, v17[1] - v14, 1i64, &read_func->streamff);
                        headerBuffer.Skip(v17[1] - v14);
                    }
                    if (++v12 >= magicData.blocks) {
						std::memcpy(res, &tmp, sizeof(tmp));
						return *res;
                    }
                }
            }
            if (v9 != v11)
                break;
            if (!v9)
                goto LABEL_5;

        }
		throw std::runtime_error("Invalid fastfile magic");
	}


	class T9FFDecompressor : public fastfile::FFDecompressor {
	public:
        T9FFDecompressor() : fastfile::FFDecompressor("Black Ops Cold War", 0x46464154, fastfile::MASK32) {}

        void LoadFastFile(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx, std::vector<byte>& ffdata) {
            if (!reader.CanRead(sizeof(XFileData))) {
                throw std::runtime_error("Can't read XFile header");
            }

            XFileMagic* header{ reader.Ptr<XFileMagic>() };
            // patch header B96310

			XFileData& data{ DecryptHeader(reader) };

			if (opt.m_header) {
				DumpHeader(data, "header");
			}

            if (data.version != 0x64) {
                throw std::runtime_error(std::format("Fast file version not supported: 0x{:x}", data.version));
            }

			ctx.ffname = data.fastfileName;

            utils::compress::CompressionAlgorithm alg{ fastfile::GetFastFileCompressionAlgorithm(data.compression) };

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
                LOG_TRACE("Decompressing block 0x{:x} {}(0x{:x}/0x{:x} -> 0x{:x})", loc, data.encrypted ? "encrypted " : "", block->compressedSize, block->alignedSize, block->uncompressedSize);

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

					// todo: handle fd file

					
					struct BDiffHeader {
						uint32_t size;
						float version;
						uint32_t flags;
						size_t maxDestWindowSize;
						uint64_t maxSourceWindowSize;
						uint64_t maxDiffWindowSize;
					}; static_assert(sizeof(BDiffHeader) == 0x28);

					BDiffHeader* bdiffHeader{ fdreader.ReadPtr<BDiffHeader>() };

					XFileData& newXFileHeader{ DecryptHeader(fdreader) };
					XFileData& baseXFileHdr{ DecryptHeader(fdreader) };
					
					
					// patch header B96310


					if (opt.m_header) {
						DumpHeader(newXFileHeader, "fd header");
						DumpHeader(baseXFileHdr, "ff header");
					}

					if (opt.dump_decompressed) {
						std::vector<byte> headerData{};

						utils::WriteValue(headerData, &newXFileHeader, sizeof(newXFileHeader));
						utils::WriteValue(headerData, &baseXFileHdr, sizeof(baseXFileHdr));

						std::filesystem::path of{ ctx.file };
						std::filesystem::path decfile{ opt.m_output / of.filename() };

						decfile.replace_extension(".fd.header.dec");

						std::filesystem::create_directories(decfile.parent_path());
						if (!utils::WriteFile(decfile, headerData)) {
							LOG_ERROR("Can't dump {}", decfile.string());
						}
						else {
							LOG_INFO("Dump fd header into {}", decfile.string());
						}
					}

					if (std::memcmp(&baseXFileHdr, header, sizeof(baseXFileHdr))) {
						throw std::runtime_error("The patch file is for this fast file");
					}

					if (bdiffHeader->maxDiffWindowSize < 0x10000) bdiffHeader->maxDiffWindowSize = 0x10000;
					if (bdiffHeader->maxDestWindowSize < 0x10000) bdiffHeader->maxDestWindowSize = 0x10000;
					if (bdiffHeader->maxSourceWindowSize < 0x10000) bdiffHeader->maxSourceWindowSize = 0x10000;

					size_t compressedSize{ fdreader.Remaining() };
					size_t fdDecompressedSize{ newXFileHeader.size };

					LOG_TRACE("Decompressing patch {} 0x{:x}: 0x{:x} -> 0x{:x}", fastfile::GetFastFileCompressionName(newXFileHeader.compression), fdreader.Loc(), compressedSize, fdDecompressedSize);
					auto uncompress{ std::make_unique<byte[]>(fdDecompressedSize) };

					utils::compress::CompressionAlgorithm alg{};
					switch (newXFileHeader.compression) {
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
						throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)newXFileHeader.compression));
					}

					if (!utils::compress::Decompress(alg, uncompress.get(), fdDecompressedSize, fdreader.ReadPtr<byte>(compressedSize), compressedSize)) {
						throw std::runtime_error(std::format("Error when decompressing fd data for type {}", fastfile::GetFastFileCompressionName(newXFileHeader.compression)));
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
					// todo: read patch data
					bool init{};
					hook::library::Library game{ opt.GetGame(true, &init) };

					//if (init) {
					//	LOG_TRACE("Init bo4 game");
					//	hook::memory::RedirectJmp(game[0x288B110], ErrorStub, true);
					//	LOG_TRACE("Init bo4 game done");
					//}

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
						hook::library::ScanResult bdiffOff{ game.FindAnyScan(
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