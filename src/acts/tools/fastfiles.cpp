#include <includes.hpp>
#include <core/bytebuffer.hpp>
#include <hook/module_mapper.hpp>
#include <hook/memory.hpp>
#include <deps/oodle.hpp>
#include <deps/bo4.hpp>
#include <games/bo4/pool.hpp>
#include <utils/decrypt.hpp>

namespace fastfiles {
	int ComputeChecksum32(const char* buffer, unsigned int len, int start) {
		int* v3; // rbx
		int* v4; // r9
		int v5; // r10d
		int v6; // r11d
		int v7; // eax
		int v8; // ecx
		int v9; // r8d
		int v10; // eax
		unsigned int v11; // eax
		int* i; // rdx
		int v13; // ecx
		unsigned __int64 v14; // r8
		unsigned __int64 v15; // r10
		int v16; // ecx
		unsigned int v17; // edx

		v3 = (int*)&buffer[len];
		v4 = (int*)buffer;
		if (len < 0x10) {
			v10 = start + 374761393;
		}
		else {
			v5 = start + 606290984;
			v6 = start - 2048144777;
			v7 = start + 1640531535;
			do
			{
				v5 = -1640531535 * RotateLeft32(v5 - 2048144777 * *v4, 13);
				v6 = -1640531535 * RotateLeft32(v6 - 2048144777 * v4[1], 13);
				v8 = 2048144777 * v4[3];
				v9 = RotateLeft32(start - 2048144777 * v4[2], 13);
				v4 += 4;
				start = -1640531535 * v9;
				v7 = -1640531535 * RotateLeft32(v7 - v8, 13);
			} while (v4 <= v3 - 4);
			v10 = RotateLeft32(v5, 1) + RotateLeft32(v6, 7) + RotateLeft32(start, 12) + RotateLeft32(v7, 18);
		}
		v11 = len + v10;
		for (i = v4 + 1; i <= v3; v11 = 668265263 * RotateLeft32(v11 - v13, 17)) {
			v13 = 1028477379 * *v4;
			v4 = i++;
		}
		v14 = 0i64;
		v15 = (char*)v3 - (char*)v4;
		if (v4 > v3)
			v15 = 0i64;
		if (v15)
		{
			do
			{
				v16 = *(unsigned __int8*)v4;
				v4 = (int*)((char*)v4 + 1);
				++v14;
				v11 = -1640531535 * RotateLeft32(v11 + 374761393 * v16, 11);
			} while (v14 < v15);
		}
		v17 = -1028477379 * ((-2048144777 * (v11 ^ (v11 >> 15))) ^ ((-2048144777 * (v11 ^ (v11 >> 15))) >> 13));
		return v17 ^ HIWORD(v17);
	}
}
#include <deps/base64.hpp>

#pragma warning(push)
#pragma warning(disable:4996)
#include <CascLib.h>
#include <CascCommon.h>
#pragma warning(pop)

namespace {

	void WriteHex(std::ostream& out, uintptr_t base, byte* buff, size_t size) {
		for (size_t j = 0; j < size; j++) {
			if (j % 8 == 0) {
				if (base) {
					out << std::hex << std::setw(16) << std::setfill('0') << (base + j) << "~";
				}
				out << std::hex << std::setw(3) << std::setfill('0') << j << "|";
				if (j + 7 < size) {
					out << std::hex << std::setw(16) << std::setfill('0') << *reinterpret_cast<uint64_t*>(&buff[j]);
				}
			}
			if (j - j % 8 + 7 >= size) {
				out << std::hex << std::setw(2) << std::setfill('0') << (int)buff[j];
			}
			if ((j + 1) % 8 == 0) {
				out << "|";

				for (size_t i = j - 7; i <= j; i++) {
					if (buff[i] >= ' ' && buff[i] <= '~') {
						out << (char)buff[i];
					}
					else {
						out << ".";
					}
				}

				// check x64 values
				if (j >= 7) {
					auto val = *reinterpret_cast<uint64_t*>(&buff[j - 7]);
					if (val) {
						// not null, hash?
						auto* h = hashutils::ExtractPtr(val);
						if (h) {
							out << " #" << h;
						}
					}
				}
				out << "\n";
			}
		}
		out << "\n";
	}
	enum XFileCompression : byte {
		XFILE_UNCOMPRESSED = 0x0,
		XFILE_ZLIB = 0x1,
		XFILE_ZLIB_HC = 0x2,
		XFILE_LZ4 = 0x3,
		XFILE_LZ4_HC = 0x4,
		XFILE_BDELTA_UNCOMP = 0x5,
		XFILE_BDELTA_ZLIB = 0x6,
		XFILE_BDELTA_LZMA = 0x7,
		XFILE_OODLE_KRAKEN = 0x8,
		XFILE_OODLE_MERMAID = 0x9,
		XFILE_OODLE_SELKIE = 0xA,
		XFILE_OODLE_LZNA = 0xB,
		XFILE_COMPRESSION_COUNT = 0xC,
	};
	const char* CompressionNames[]{
		"none",
		"zlib",
		"zlib_hc",
		"lz4",
		"lz4_hc",
		"bdelta_uncomp",
		"bdelta_zlib",
		"bdelta_lzma",
		"oodle_kraken",
		"oodle_mermaid",
		"oodle_selkie",
		"oodle_lzna",
	};
	struct XFile {
		uint8_t magic[8];
		uint32_t version;
		uint8_t server;
		XFileCompression compression;
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
		byte pad0[184];
		char key[8];
		byte pad1[328];
	}; static_assert(sizeof(XFile) == 0x840);
	struct BDiffHeader {
		XFile newXFileHeader;
		XFile baseXFileHdr;
		uint32_t size;
		float version;
		uint32_t flags;
		size_t maxDestWindowSize;
		uint64_t maxSourceWindowSize;
		uint64_t maxDiffWindowSize;
	}; static_assert(sizeof(BDiffHeader) == 0x10a8);

	struct DBStreamHeader {
		uint32_t compressedSize;
		uint32_t uncompressedSize;
		uint32_t alignedSize;
		uint32_t offset;
	}; static_assert(sizeof(DBStreamHeader) == 16);
	struct aesKey_t {
		uint32_t fileNameHash;
		uint32_t version;
		byte key[32];
	};
	struct aesKeyLocal_t {
		const char* fileNameHash;
		uint32_t version;
		byte key[32];
	};

	aesKeyLocal_t keys[64] = {
		{ "mp_common", 0x1, { 0x2c, 0xa0, 0x5c, 0x8c, 0x0b, 0x4b, 0x16, 0xda, 0xe1, 0x19, 0x0a, 0x97, 0x64, 0xfd, 0xc6, 0x40, 0xdd, 0x8c, 0x5d, 0xb1, 0x9e, 0xe3, 0x20, 0x40, 0xe0, 0x22, 0x7c, 0x28, 0xaf, 0x93, 0x9b, 0x2c } },
		{ "cp_common", 0x1, { 0xd5, 0x79, 0xac, 0xf1, 0xb8, 0xaf, 0x0e, 0x7b, 0xed, 0x95, 0xef, 0x46, 0x94, 0xdc, 0xb7, 0x20, 0x7b, 0x03, 0x05, 0x88, 0xb9, 0xab, 0xa9, 0x12, 0x25, 0x3a, 0x8c, 0x25, 0x46, 0x5b, 0x50, 0x1a } },
		{ "zm_common", 0x1, { 0xb9, 0x59, 0x31, 0x39, 0x3c, 0x4d, 0xda, 0xf2, 0x82, 0x24, 0x7b, 0x76, 0xa6, 0x26, 0x39, 0xb4, 0x6a, 0xb2, 0xdf, 0x72, 0xca, 0x67, 0x47, 0x4a, 0x94, 0xa8, 0xb3, 0xba, 0xd6, 0x62, 0x7e, 0xe2 } },
		{ "mp_common_script", 0x1, { 0xed, 0xee, 0x8a, 0x02, 0x6a, 0xb1, 0x08, 0xad, 0xe7, 0x37, 0x75, 0xbb, 0xd6, 0xb8, 0xd9, 0x1e, 0x97, 0x45, 0x6a, 0xa0, 0x12, 0x78, 0x12, 0x6b, 0x28, 0x95, 0xf7, 0x30, 0xb9, 0xfa, 0x4e, 0x00 } },
		{ "cp_common_script", 0x1, { 0xa9, 0xe8, 0x1c, 0x1d, 0x3f, 0x48, 0x4f, 0xe3, 0xa3, 0x3d, 0xe0, 0x75, 0x84, 0x3f, 0x0a, 0x75, 0x0d, 0x4b, 0xcb, 0xe9, 0xa3, 0x6e, 0x47, 0x35, 0xeb, 0x9c, 0xb6, 0xa3, 0x22, 0x6b, 0x2a, 0x31 } },
		{ "zm_common_script", 0x1, { 0x19, 0x43, 0xc6, 0xb6, 0xb3, 0x64, 0x74, 0xa0, 0xdb, 0x86, 0xff, 0x41, 0xb1, 0xe4, 0x45, 0x88, 0x15, 0xac, 0xef, 0x93, 0xea, 0xcb, 0x0b, 0xea, 0x0f, 0xb6, 0xb7, 0xa8, 0xe4, 0x59, 0x33, 0x3e } },
		{ "wz_common", 0x1, { 0xa9, 0x83, 0xb0, 0xdd, 0x11, 0xe9, 0x7f, 0xfe, 0x0b, 0xa0, 0xda, 0xa1, 0x6b, 0xb0, 0xfc, 0xc3, 0x49, 0x75, 0xb8, 0x9b, 0x35, 0x6b, 0xc7, 0xf5, 0x19, 0x7b, 0xf3, 0xf5, 0x14, 0xa5, 0x72, 0x97 } },
		{ "wz_common_script", 0x1, { 0x83, 0x10, 0xd0, 0x8b, 0x91, 0xd1, 0x79, 0x0f, 0xa3, 0x55, 0xeb, 0x8c, 0xd9, 0x58, 0x9d, 0x19, 0xf6, 0x0f, 0x1b, 0xfb, 0x48, 0x86, 0xc0, 0x82, 0x1f, 0x9c, 0xde, 0x16, 0x2f, 0x84, 0x7f, 0x8d } },
		{ "4k_mp_common", 0x1, { 0xf2, 0x2b, 0x37, 0x58, 0xd9, 0xec, 0xdf, 0x05, 0x33, 0xfb, 0xf8, 0x8f, 0xfa, 0x02, 0xea, 0x38, 0x4e, 0xcf, 0xbd, 0x4a, 0xc4, 0x8c, 0x09, 0x42, 0xeb, 0x9e, 0x2e, 0x48, 0x25, 0x6a, 0x45, 0xf7 } },
		{ "4k_cp_common", 0x1, { 0x35, 0xce, 0x15, 0x0d, 0xe2, 0xdb, 0x3d, 0x7f, 0xee, 0x4d, 0x51, 0x6d, 0xf1, 0xd9, 0xad, 0x37, 0x38, 0x02, 0xc6, 0xb8, 0xa6, 0x40, 0xdf, 0x19, 0x64, 0x39, 0xee, 0x84, 0x8c, 0x38, 0x4f, 0x81 } },
		{ "4k_zm_common", 0x1, { 0x07, 0x6d, 0xff, 0x8e, 0x01, 0x88, 0x93, 0xf6, 0x4d, 0x99, 0x09, 0x12, 0x0b, 0xdf, 0x20, 0x21, 0x3a, 0x5e, 0x36, 0x8a, 0xc1, 0x90, 0xb7, 0x55, 0x17, 0xaf, 0x3e, 0x28, 0xf4, 0xa9, 0x11, 0xc2 } },
		{ "4k_wz_common", 0x1, { 0x69, 0xdd, 0xbf, 0x42, 0xd8, 0x00, 0xfc, 0xce, 0xad, 0x31, 0x4b, 0x34, 0x81, 0x31, 0xa5, 0x14, 0xa1, 0xc0, 0x28, 0x18, 0xea, 0xaf, 0xf6, 0x83, 0xe0, 0x2e, 0xd3, 0xc1, 0x07, 0xd5, 0x00, 0x3d } },
		{ "1080_mp_common", 0x1, { 0x1c, 0x17, 0x3a, 0xe6, 0xa0, 0x0a, 0x0e, 0xe5, 0x7b, 0x20, 0xea, 0xa2, 0x1c, 0x62, 0xba, 0x30, 0xa8, 0x67, 0xaa, 0x04, 0x2a, 0x12, 0xe1, 0xab, 0x5c, 0x4e, 0x34, 0xa3, 0xf3, 0xc4, 0xa3, 0x9b } },
		{ "1080_cp_common", 0x1, { 0xd6, 0xf6, 0xea, 0x66, 0x1c, 0x6f, 0x17, 0xad, 0x86, 0x06, 0xf6, 0x34, 0xea, 0x7e, 0x91, 0x63, 0x56, 0x0a, 0x34, 0x2d, 0xeb, 0x5b, 0x0b, 0x3c, 0x12, 0x91, 0xff, 0xc9, 0x6b, 0x3b, 0xc8, 0x35 } },
		{ "1080_zm_common", 0x1, { 0x4c, 0x90, 0x93, 0x14, 0x76, 0x54, 0xce, 0x47, 0x01, 0xe7, 0x1d, 0x29, 0x06, 0x15, 0x93, 0x21, 0xfc, 0x45, 0x9a, 0x74, 0xda, 0x3d, 0x1d, 0x42, 0x96, 0xbd, 0x29, 0x48, 0xc4, 0x82, 0x12, 0xcc } },
		{ "1080_wz_common", 0x1, { 0x88, 0x85, 0x7a, 0xe5, 0x22, 0x0a, 0x36, 0xf9, 0x76, 0xe2, 0x18, 0xe8, 0x7b, 0x7b, 0xbc, 0x81, 0xeb, 0x8f, 0x5d, 0xa2, 0x7c, 0xad, 0x1d, 0x7c, 0x77, 0x2e, 0xc6, 0xbb, 0x12, 0xbd, 0xd7, 0xa0 } },
	};


	class FastFileOption {
	public:
		bool m_help{};
		bool m_fd{};
		bool m_header{};
		const char* m_casc{};
		const char* game{};
		HANDLE cascStorage{};
		const char* m_output{ "output_ff" };
		std::vector<const char*> files{};

		~FastFileOption() {
			if (cascStorage) {
				CascCloseStorage(cascStorage);
			}
		}

		bool Compute(const char** args, INT startIndex, INT endIndex) {
			// default values
			for (size_t i = startIndex; i < endIndex; i++) {
				const char* arg = args[i];

				if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
					m_help = true;
				}
				else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					m_output = args[++i];
				}
				else if (!strcmp("-g", arg) || !_strcmpi("--game", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					game = args[++i];
				}
				else if (!strcmp("-C", arg) || !_strcmpi("--casc", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					m_casc = args[++i];

					if (!CascOpenStorage(m_casc, 0, &cascStorage)) {
						LOG_ERROR("Can't open casc storage {}: 0x{:x}", m_casc, GetCascError());
						return false;
					}
				}
				else if (!_strcmpi("--fd", arg) || !strcmp("-p", arg)) {
					m_fd = true;
				}
				else if (!_strcmpi("--header", arg) || !strcmp("-H", arg)) {
					m_header = true;
				}
				else if (*arg == '-') {
					std::cerr << "Invalid argument: " << arg << "!\n";
					return false;
				}
				else {
					files.push_back(arg);
				}
			}
			return true;
		}
		void PrintHelp() {
			LOG_INFO("-h --help            : Print help");
			LOG_INFO("-o --output [d]      : Output dir");
			LOG_INFO("-H --header          : Dump header info");
			LOG_INFO("-C --casc [c]        : Use casc db");
			LOG_INFO("-g --game [g]        : exe");
			LOG_INFO("-p --fd              : Use patch file (fd)");
		}

		std::vector<std::string> GetFileRecurse(const char* path) {
			std::vector<std::string> res{};
			if (cascStorage) {
				CASC_FIND_DATA data;

				HANDLE firstFileHandle{ CascFindFirstFile(cascStorage, path, &data, NULL) };

				if (!firstFileHandle) {
					LOG_ERROR("Can't find path {}", path);
					return res;
				}
				utils::CloseEnd ce{ [firstFileHandle] { CascFindClose(firstFileHandle); } };


				do {
					if (data.bFileAvailable) {
						res.push_back(data.szFileName);
					}
				} while (CascFindNextFile(firstFileHandle, &data));
			}
			else {
				std::vector<std::filesystem::path> files{};

				utils::GetFileRecurse(path, files);


				for (const std::filesystem::path& file : files) {
					res.push_back(file.string());
				}
			}

			return res;

		}

		bool ReadFile(const char* path, std::vector<byte>& buff) {
			if (cascStorage) {
				CASC_FIND_DATA data;

				HANDLE firstFileHandle{ CascFindFirstFile(cascStorage, path, &data, NULL) };

				if (!firstFileHandle || !data.bFileAvailable) {
					LOG_ERROR("Can't find path {}", path);
					return false;
				}
				utils::CloseEnd ce{ [firstFileHandle] { CascFindClose(firstFileHandle); } };


				buff.clear();
				if (!data.FileSize) {
					return true; // nothing to read
				}
				buff.resize(data.FileSize);
				HANDLE fhandle;
				if (!CascOpenFile(cascStorage, data.szFileName, CASC_LOCALE_ALL, 0, &fhandle)) {
					LOG_ERROR("Can't open path {}", data.szFileName);
					return false;
				}
				utils::CloseEnd cef{ [fhandle] { CascCloseFile(fhandle); } };

				DWORD read{};
				return CascReadFile(fhandle, buff.data(), (DWORD)data.FileSize, &read) && read == data.FileSize;
			}

			return utils::ReadFile(path, buff);
		}
	};

	enum BDiffFlags : byte {
		BDF_START_BD = 1,
		BDF_UNK2 = 2,
		BDF_MASK21 = BDF_START_BD | BDF_UNK2,
		BDF_FROM_START = 4,
		BDF_UNK8 = 8,
	};

	void bdiff(core::bytebuffer::ByteBuffer& patchData, core::bytebuffer::ByteBuffer& buffer) {
		size_t offset{ 0x405 };
		if (!patchData.CanRead(offset)) {
			throw std::runtime_error(std::format("Can't read 0x{:x} bytes", offset));
		}

		byte* bdiffMagic{ patchData.ReadPtr<byte>(5) };

		if (std::memcmp(bdiffMagic, "\xd6\xc3\xc4", 3)) {
			throw std::runtime_error(std::format("Invalid bdiff magic for 0x{:x} 0x{:x} 0x{:x}", bdiffMagic[0], bdiffMagic[1], bdiffMagic[2]));
		}

		if (bdiffMagic[4] & 0xF3) {
			LOG_WARNING("unk & 0xF3, error?");
		}

		// ok bdiff

		while (patchData.CanRead(1)) {
			byte bdiffFlags{ patchData.Read<byte>() };

			size_t unkShift{ (size_t)((bdiffFlags >> 1) & 4) };

			if ((bdiffFlags & BDF_UNK2)) {
				throw std::runtime_error("flag 2 sets, can't load");
			}

			LOG_INFO("---------- {:x}", patchData.Loc());
			// diff? maybe offset/size -> data[]
			size_t unk0;
			byte* diffBlockPost{};
			if (bdiffFlags & BDF_MASK21) {
				// layer start
				unk0 = patchData.ReadVByte();
				size_t sourceSegmentLocationEx{ patchData.ReadVByte() };

				size_t sourceSegmentLocation;

				if (bdiffFlags & BDF_FROM_START) {
					sourceSegmentLocation = patchData.ReadVByte();
				}
				else {
					sourceSegmentLocation = sourceSegmentLocationEx;
				}

				size_t sizeUnk{ sourceSegmentLocationEx - sourceSegmentLocation };
				size_t rsized{ sizeUnk + unk0 };

				LOG_INFO("{:x} {:x} {:x} {:x} {:x}", unk0, sourceSegmentLocationEx, sourceSegmentLocation, sizeUnk, rsized);
				buffer.Goto(sourceSegmentLocation);
				byte* diffblock{ buffer.ReadPtr<byte>(rsized) };
				diffBlockPost = diffblock + sizeUnk;
			}
			else {
				// layer post
				throw std::runtime_error("bdiff 0 part not implemented");
			}

			size_t unkvbyte2{ patchData.ReadVByte() };
			LOG_INFO("{:x} {:x}", unkvbyte2, unkvbyte2 + unkShift);

			// patch data:
			// off: ptr + offset - bdiffDataStart 
			// siz: unkvbyte2 + unkShift
			byte* patchDataBf{ patchData.ReadPtr<byte>(unkvbyte2 + unkShift) };

			//size_t unkvbyte3{ patchData.ReadVByte() };


			//utils::OutFileCE os{ "output_ff/test.ff.dec" };
			//WriteHex(os, sizeof(*fdheader), uncompress.get(), uncompressSize);
		}
	}

	int fftest(int argc, const char* argv[]) {
		FastFileOption opt{};

		if (!opt.Compute(argv, 2, argc) || opt.m_help) {
			opt.PrintHelp();
			return opt.m_help ? tool::OK : tool::BASIC_ERROR;
		}

		deps::bo4::Bo4 bo4{};

		if (opt.m_fd) {
			if (!bo4.LoadFromDeps()) {
				LOG_ERROR("Can't load bo4 exec from deps, did you put a dump inside the deps/ directory?");
				return tool::BASIC_ERROR;
			}
			bo4.PatchErrorAsThrow();
			LOG_INFO("{} loaded", *bo4);
		}

		std::vector<byte> fileBuff{};
		std::vector<byte> fileFDBuff{};
		for (const char* f : opt.files) {
			for (const std::string filename : opt.GetFileRecurse(f)) {
				if (!filename.ends_with(".ff")) {
					LOG_DEBUG("Ignore {}", filename);
					continue;
				}
				if (!opt.ReadFile(filename.data(), fileBuff)) {
					LOG_ERROR("Can't read file {}", filename);
					return tool::BASIC_ERROR;
				}

				LOG_INFO("Reading {}", filename);

				core::bytebuffer::ByteBuffer reader{ fileBuff };

				int ret{};
				auto PrintXFile = [&opt, &ret, &reader](XFile* buffer) {
					if (*reinterpret_cast<uint64_t*>(buffer->magic) != 0x3030303066664154) {
						LOG_ERROR("Invalid FF magic: {:x}", *reinterpret_cast<uint64_t*>(buffer));
						ret = tool::BASIC_ERROR;
						return;
					}

					if (buffer->version != 0x27F && buffer->version != 0x27E) {
						LOG_ERROR("Not a T8 FF, version: 0x{:X}", buffer->version);
						ret = tool::BASIC_ERROR;
						return;
					}

					if (buffer->compression >= XFILE_COMPRESSION_COUNT) {
						LOG_ERROR("Not implemented for this compression type: {}", (int)buffer->compression);
						ret = tool::BASIC_ERROR;
						return;
					}

					if (!opt.m_header) return;


					if (buffer->encrypted) {
						auto checksum32 = fastfiles::ComputeChecksum32(buffer->key, (unsigned int)strlen(buffer->key), 0);
						LOG_INFO("checksum32 . {}", checksum32);
					}

					LOG_INFO("server ............. {}", (buffer->server ? "true" : "false"));
					LOG_INFO("encrypted .......... {}", (buffer->encrypted ? "true" : "false"));
					LOG_INFO("compress ........... {} (0x{:x})", CompressionNames[buffer->compression], (int)buffer->compression);
					LOG_INFO("platform ........... {}", (int)buffer->platform);
					LOG_INFO("builder ............ {}", buffer->builder);
					LOG_INFO("timestamp .......... {}", buffer->timestamp);

					for (size_t i = 0; i < 4; i++) {
						LOG_INFO("archiveChecksum[{}] . {:x}", i, buffer->archiveChecksum[i]);
					}

					LOG_INFO("key ................ {}", buffer->key);
					};

				if (opt.m_header) LOG_INFO("size ............... {}B (0x{:x})", utils::FancyNumber(fileBuff.size()), fileBuff.size());
				XFile* buffer = reader.ReadPtr<XFile>();
				PrintXFile(buffer);
				if (ret) return ret;

				deps::oodle::Oodle oodle{ deps::oodle::OO2CORE_8 };

				if (!oodle) return tool::BASIC_ERROR;
				
				size_t idx{};

				std::vector<byte> ffdata{};
				while (true) {
					size_t loc{ reader.Loc() };

					DBStreamHeader* block{ reader.ReadPtr<DBStreamHeader>() };
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


					LOG_TRACE("Decompressing block 0x{:x} (0x{:x}/0x{:x} -> 0x{:x})", loc, block->compressedSize, block->alignedSize, block->uncompressedSize);

					auto decompressed{ std::make_unique<byte[]>(block->uncompressedSize) };

					switch (buffer->compression) {
					case XFILE_UNCOMPRESSED:
						if (block->uncompressedSize > block->compressedSize) {
							throw std::runtime_error(std::format("Can't decompress block, decompressed size isn't big enough: 0x{:x} != 0x{:x}", block->compressedSize, block->uncompressedSize));
						}
						memcpy(decompressed.get(), blockBuff, block->uncompressedSize);
						break;
					case XFILE_ZLIB:
					case XFILE_ZLIB_HC: {

						uLongf sizef = (uLongf)block->uncompressedSize;
						uLongf sizef2{ (uLongf)block->compressedSize };
						int ret;
						if (ret = uncompress2(decompressed.get(), &sizef, reader.Ptr<const Bytef>(), &sizef2) < 0) {
							throw std::runtime_error(std::format("error when decompressing {}", zError(ret)));
						}
						break;
					}
					case XFILE_OODLE_KRAKEN:
					case XFILE_OODLE_MERMAID:
					case XFILE_OODLE_SELKIE:
					case XFILE_OODLE_LZNA: {
						int ret{ oodle.Decompress(blockBuff, block->compressedSize, decompressed.get(), block->uncompressedSize, deps::oodle::OODLE_FS_YES) };

						if (ret != block->uncompressedSize) {
							throw std::runtime_error(std::format("Can't decompress block, returned size isn't the expected one: 0x{:x} != 0x{:x}", ret, block->uncompressedSize));
						}
						break;
					}
					default:
						throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)buffer->compression));
					}


					utils::WriteValue(ffdata, decompressed.get(), block->uncompressedSize);
				}

				LOG_INFO("Decompressed 0x{:x} byte(s) from 0x{:x} block(s)", ffdata.size(), idx);

				core::bytebuffer::ByteBuffer buff{ ffdata };

				if (opt.m_fd) {
					std::filesystem::path fdfile{ filename };

					fdfile.replace_extension(".fd");
					const std::string filenameFd{ fdfile.string() };

					if (!opt.ReadFile(filenameFd.data(), fileFDBuff)) {
						LOG_INFO("No fd file for {}: {}", filename, filenameFd);
					}
					else {
						core::bytebuffer::ByteBuffer bufffd{ fileFDBuff };

						BDiffHeader* fdheader{ bufffd.ReadPtr<BDiffHeader>() };

						if (fdheader->size != sizeof(BDiffHeader)) {
							throw std::runtime_error(std::format("Invalid xfile fd header size 0x{:x}", fdheader->size));
						}

						LOG_INFO("Loading fd file {}", filenameFd);

						if (opt.m_header) {
							LOG_INFO("fd size ............... {}B (0x{:x})", utils::FancyNumber(fileFDBuff.size()), fileFDBuff.size());
							LOG_INFO("---- header ----");
						}
						PrintXFile(&fdheader->baseXFileHdr);
						if (opt.m_header) LOG_INFO("---- new header ----");
						PrintXFile(&fdheader->newXFileHeader);
						if (ret) return ret;

						LOG_INFO("size 0x{:x}", fdheader->size);
						LOG_INFO("version {}", fdheader->version);
						LOG_INFO("flags 0x{:x}", fdheader->flags);
						LOG_INFO("maxDestWindowSize   0x{:x}", fdheader->maxDestWindowSize);
						LOG_INFO("maxSourceWindowSize 0x{:x}", fdheader->maxSourceWindowSize);
						LOG_INFO("maxDiffWindowSize   0x{:x}", fdheader->maxDiffWindowSize);

						auto uncompress{ std::make_unique<byte[]>(fdheader->maxSourceWindowSize)};
						size_t uncompressSize{};

						switch (fdheader->newXFileHeader.compression) {
						case XFILE_BDELTA_ZLIB: {
							uLongf sizef = (uLongf)fdheader->maxSourceWindowSize;
							uLongf sizef2{ (uLongf)(fileFDBuff.size() - sizeof(BDiffHeader)) };
							int ret;
							if (ret = uncompress2((Bytef*)uncompress.get(), &sizef, bufffd.Ptr<const Bytef>(), &sizef2) < 0) {
								throw std::runtime_error(std::format("error when decompressing {}", zError(ret)));
							}

							uncompressSize = sizef;
						}
							break;
						//case XFILE_BDELTA_UNCOMP:
						//case XFILE_BDELTA_LZMA:
						default:
							throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)fdheader->baseXFileHdr.compression));
						}

						core::bytebuffer::ByteBuffer buffbd{ uncompress.get(), uncompressSize };

						deps::bo4::BDiffState state{};

						static struct {
							core::bytebuffer::ByteBuffer* buffbd{};
							core::bytebuffer::ByteBuffer* buffsrc{};
							int64_t size{};
							byte* destWindow{};
						} ffDecode{};

						ffDecode.buffbd = &buffbd;
						ffDecode.buffsrc = &buff;
						ffDecode.size = 0;
						ffDecode.destWindow = uncompress.get() + 0x80000 + 0x80000;

						if (!bo4.bdiff(&state,
							[](uint64_t offset, uint64_t size) -> byte* {
								// srcDataCallback
								ffDecode.buffsrc->Goto(offset);
								return ffDecode.buffbd->ReadPtr<byte>(size);
							}, 
							[](uint64_t offset, uint64_t size, uint64_t* pOffset) -> byte* {
								// patchDataCB
								if (offset) {
									ffDecode.buffbd->Goto(offset);
								}
								byte* ret{ ffDecode.buffbd->ReadPtr<byte>(size) };
								if (pOffset) *pOffset = ffDecode.buffbd->Loc();
								return ret;
							},
							[](uint64_t size) -> byte* {
								// destDataCB
								return ffDecode.destWindow;
							} 
							)) {
							LOG_ERROR("Error when bdiff");
						}
					}

				}
				// search gscobj
				{
					/*
					 The pattern is :
					 align<8>
					 ScriptParseTree {
					    u64 name;
						u64 namepad?; // maybe not there
						void* buffer = -1;
						u32 size;
						u32 unk;
					 }
					 align<32>
					 GSC_OBJ {
					    u64 magic = 0x??00a0d43534780
					 }
					
					*/
					uint64_t magic{ 0x00a0d43534780 };
					uint64_t magicMask{ 0xFFFFFFFFFFFFF };

					size_t loc{};
					std::filesystem::path out{ "scriptparsetree_t8ff" };
					buff.Goto(0);
					byte* start{ buff.Ptr<byte>() };
					while (true) {
						loc = buff.FindMasked((byte*)&magic, (byte*)&magicMask, sizeof(magic));
						if (loc == std::string::npos) break;
						struct T8GSCOBJ {
							byte magic[8];
							int32_t crc;
							int32_t pad;
							uint64_t name;
						};

						buff.Goto(loc);

						if (!buff.CanRead(sizeof(T8GSCOBJ))) {
							break; // can't read buffer
						}

						byte* sptCan{ buff.Ptr<byte>() - 0x18 }; // 0x18 is the minimum size to lookup

						// we are 32 bytes aligned
						T8GSCOBJ* obj{ buff.ReadPtr<T8GSCOBJ>() };

						uint64_t name{ obj->name };

						while (*(uint64_t*)sptCan != name) {
							sptCan--;
							if (start == sptCan) {
								break;
							}
						}
						if (start == sptCan) {
							loc++;
							continue;
						}


						uint64_t smagic{ *reinterpret_cast<uint64_t*>(obj) };

						size_t size;
						struct T8SPT {
							uint64_t name;
							uint64_t pad0;
							uintptr_t buffer;
							uint32_t size;
							uint32_t pad02;
						};
						struct T8SPTOld {
							uint64_t name;
							uintptr_t buffer;
							uint32_t size;
							uint32_t pad02;
						};

						// Depending on how old the ff is, we might use a XHash of 0x10 or 8 bytes. The pointer
						// to the buffer will constantly be -1 because it is not linked yet
						if (((T8SPT*)sptCan)->buffer == 0xFFFFFFFFFFFFFFFF) {
							size = ((T8SPT*)sptCan)->size;
						}
						else if (((T8SPTOld*)sptCan)->buffer == 0xFFFFFFFFFFFFFFFF) {
							size = ((T8SPTOld*)sptCan)->size;
						}
						else {
							LOG_ERROR("Can't get size 0x{:x} for loc 0x{:x}", smagic, loc);
							loc++;
							continue;
						}

						LOG_TRACE("gsc: 0x{:x} 0x{:x} 0x{:x}: {}", smagic, loc, size, hashutils::ExtractTmpScript(obj->name));


						if (!buff.CanRead(size)) {
							loc++;
							LOG_ERROR("Bad size 0x{:x} 0x{:x} for loc 0x{:x}", smagic, size, loc);
							continue;
						}
						buff.Skip(size);

						std::filesystem::path outFile{ out / std::format("vm_{:x}/script_{:x}.gscc", smagic, obj->name) };
						std::filesystem::create_directories(outFile.parent_path());

						if (!utils::WriteFile(outFile, obj->magic, size)) {
							LOG_ERROR("Can't write {}", outFile.string());
						}
						else {
							LOG_INFO("Dump {}", outFile.string());
						}

						loc++;
					}

				}
				LOG_INFO("Done");
			}
		}


		return tool::OK;
	}

	struct {
		core::bytebuffer::ByteBuffer* buffer{};

		void CheckValid() {
			if (!buffer) throw std::runtime_error("no buffer");
		}
	} streamData;

	bool Load_Stream(bool atStreamStart, void* ptr, size_t size) {
		if (!atStreamStart || !size) return true; // no read

		streamData.CheckValid();

		byte* data{ streamData.buffer->ReadPtr<byte>(size) };

		std::memmove(ptr, data, size);

		return true;
	}
	byte* DB_AllocStreamPos(int alignment) {
		streamData.CheckValid();
		
		streamData.buffer->Align(alignment + 1);
		return streamData.buffer->Ptr();
	}

	void** DB_InsertPointer() {
		streamData.CheckValid();
		streamData.buffer->Align<void*>();
		return streamData.buffer->Ptr<void*>();
	}

	void Load_SimpleAsset_Internal(void** data, games::bo4::pool::XAssetType type) {

	}

	void EmptyFunc() {}

	int ffexe(int argc, const char* argv[]) {
		FastFileOption opt{};

		if (!opt.Compute(argv, 2, argc) || opt.m_help) {
			opt.PrintHelp();
			return opt.m_help ? tool::OK : tool::BASIC_ERROR;
		}

		static hook::module_mapper::Module game{ true };

		if (!opt.game) {
			LOG_ERROR("No game set, please use -g [exe]");
			return tool::BAD_USAGE;
		}

		if (!game && !game.Load(opt.game)) {
			LOG_ERROR("Can't map {}", opt.game);
			return tool::BASIC_ERROR;
		}
		hook::memory::RedirectJmp((*game)[0x2EBC050], Load_Stream);
		hook::memory::RedirectJmp((*game)[0x2EBBBA0], DB_AllocStreamPos);
		hook::memory::RedirectJmp((*game)[0x2EBBCC0], DB_InsertPointer);
		hook::memory::RedirectJmp((*game)[0x2EBB6F0], Load_SimpleAsset_Internal);
		hook::memory::RedirectJmp((*game)[0x2EBBFF0], EmptyFunc); // DB_ConvertOffsetToPointer
		hook::memory::RedirectJmp((*game)[0x2EBC110], EmptyFunc); // unk
		hook::memory::RedirectJmp((*game)[0x2EBBFB0], EmptyFunc); // unk
		hook::memory::RedirectJmp((*game)[0x2EBBEA0], EmptyFunc); // DB_PushStreamPos
		hook::memory::RedirectJmp((*game)[0x2EBBE20], EmptyFunc); // DB_PopStreamPos
		hook::memory::RedirectJmp((*game)[0x35BA450], EmptyFunc); // Load_GfxImageAdapter

		deps::oodle::Oodle oodle{};

		if (!oodle.LoadOodle(deps::oodle::OO2CORE_7) || !oodle) return tool::BASIC_ERROR;

		std::vector<byte> fileBuff{};
		std::vector<byte> fileFDBuff{};
		for (const char* f : opt.files) {
			if (!opt.ReadFile(f, fileBuff)) {
				LOG_ERROR("Can't read file {}", f);
				return tool::BASIC_ERROR;
			}
			std::filesystem::path fffile{ f };
			std::filesystem::path fdfile{ f };

			fdfile.replace_extension(".fd");

			LOG_INFO("Reading {}", fffile.string());

			core::bytebuffer::ByteBuffer reader{ fileBuff };

			int ret{};
			auto PrintXFile = [&ret, &reader](XFile* buffer) {
				if (*reinterpret_cast<uint64_t*>(buffer->magic) != 0x3030303066664154) {
					LOG_ERROR("Invalid FF magic: {:x}", *reinterpret_cast<uint64_t*>(buffer));
					ret = tool::BASIC_ERROR;
					return;
				}

				if (buffer->version != 0x27F) {
					LOG_ERROR("Not a T8 FF, version: 0x{:x} != 0x27F", buffer->version);
					ret = tool::BASIC_ERROR;
					return;
				}


				if (buffer->encrypted) {
					auto checksum32 = fastfiles::ComputeChecksum32(buffer->key, (unsigned int)strlen(buffer->key), 0);
					LOG_INFO("checksum32 . {}", checksum32);
				}

				if (buffer->compression >= XFILE_COMPRESSION_COUNT) {
					LOG_ERROR("Not implemented for this compression type: {}", (int)buffer->compression);
					ret = tool::BASIC_ERROR;
					return;
				}

				LOG_INFO("server ............. {}", (buffer->server ? "true" : "false"));
				LOG_INFO("encrypted .......... {}", (buffer->encrypted ? "true" : "false"));
				LOG_INFO("compress ........... {} (0x{:x})", CompressionNames[buffer->compression], (int)buffer->compression);
				LOG_INFO("platform ........... {}", (int)buffer->platform);
				LOG_INFO("builder ............ {}", buffer->builder);
				LOG_INFO("timestamp .......... {}", buffer->timestamp);

				for (size_t i = 0; i < 4; i++) {
					LOG_INFO("archiveChecksum[{}] . {:x}", i, buffer->archiveChecksum[i]);
				}

				LOG_INFO("key ................ {}", buffer->key);
				};

			LOG_INFO("size ............... {}B (0x{:x})", utils::FancyNumber(fileBuff.size()), fileBuff.size());
			XFile* buffer = reader.ReadPtr<XFile>();
			PrintXFile(buffer);
			if (ret) return ret;

			size_t idx{};

			std::vector<byte> ffdata{};
			while (true) {
				size_t loc{ reader.Loc() };

				DBStreamHeader* block{ reader.ReadPtr<DBStreamHeader>() };
				if (!block->alignedSize) break; // end

				if (block->offset != loc) {
					LOG_ERROR("bad block position: 0x{:x} != 0x{:x}", loc, block->offset);
					break;
				}


				if (!block->uncompressedSize) {
					reader.Align(0x800000);
					continue;
				}

				byte* blockBuff{ reader.ReadPtr<byte>(block->alignedSize) };


				LOG_TRACE("Decompressing block 0x{:x} (0x{:x}/0x{:x} -> 0x{:x})", loc, block->compressedSize, block->alignedSize, block->uncompressedSize);

				size_t decompressedBlock{ utils::Allocate(ffdata, block->uncompressedSize) };
				byte* decompressed{ &ffdata[decompressedBlock] };

				switch (buffer->compression) {
				case XFILE_UNCOMPRESSED:
					if (block->uncompressedSize > block->compressedSize) {
						throw std::runtime_error(std::format("Can't decompress block, decompressed size isn't big enough: 0x{:x} != 0x{:x}", block->compressedSize, block->uncompressedSize));
					}
					memcpy(decompressed, blockBuff, block->uncompressedSize);
					break;
				case XFILE_ZLIB:
				case XFILE_ZLIB_HC: {

					uLongf sizef = (uLongf)block->uncompressedSize;
					uLongf sizef2{ (uLongf)block->compressedSize };
					int ret;
					if (ret = uncompress2(decompressed, &sizef, reader.Ptr<const Bytef>(), &sizef2) < 0) {
						throw std::runtime_error(std::format("error when decompressing {}", zError(ret)));
					}
					break;
				}
				case XFILE_OODLE_KRAKEN:
				case XFILE_OODLE_MERMAID:
				case XFILE_OODLE_SELKIE:
				case XFILE_OODLE_LZNA: {
					int ret{ oodle.Decompress(blockBuff, block->compressedSize, decompressed, block->uncompressedSize, deps::oodle::OODLE_FS_YES) };

					if (ret != block->uncompressedSize) {
						throw std::runtime_error(std::format("Can't decompress block, returned size isn't the expected one: 0x{:x} != 0x{:x}", ret, block->uncompressedSize));
					}
					break;
				}
				default:
					throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)buffer->compression));
				}
			}

			LOG_INFO("Decompressed 0x{:x} byte(s)", ffdata.size());

			core::bytebuffer::ByteBuffer buff{ ffdata };

			streamData.buffer = &buff;

			// assets are aligned
			buff.Align<void*>();
			struct ScriptStringList {
				int count;
				char** strings;
			};
			struct XAsset_0 {
				games::bo4::pool::XAssetType type;
				void* header;
			}; static_assert(sizeof(XAsset_0) == 0x10);


			struct XAssetList_0 {
				ScriptStringList stringList;
				int assetCount;
				XAsset_0* assets;
			};

			XAssetList_0& assetList{ *buff.ReadPtr<XAssetList_0>() };

			// load strings
			buff.Align<void*>();
			assetList.stringList.strings = buff.ReadPtr<char*>(assetList.stringList.count);

			for (size_t i = 0; i < assetList.stringList.count; i++) {
				if (assetList.stringList.strings[i] == reinterpret_cast<char*>(0xFFFFFFFFFFFFFFFF)) {
					assetList.stringList.strings[i] = buff.ReadString();
				}
				else {
					LOG_ERROR("Bad str {}", reinterpret_cast<int64_t>(assetList.stringList.strings[i]) );
				}
			}


			LOG_INFO("Done");
		}


		return tool::OK;
	}


	int ffdaes(Process& proc, int argc, const char* argv[]) {

		aesKey_t keys[64];

		if (!proc.ReadMemory(keys, proc[0xDDCC3A0], sizeof(keys))) {
			LOG_ERROR("Can't read aes keys");
			return tool::BASIC_ERROR;
		}

		std::cout
			<< "struct aesKey_t {\n"
			<< "    int fileNameHash;\n"
			<< "    int version; \n"
			<< "    byte key[32];\n"
			<< "}\n"
			<< "aesKey_t keys[64] = {"
			;
		for (size_t i = 0; i < 64; i++) {
			auto& key = keys[i];

			if (!key.fileNameHash) continue;

			std::cout
				<< "\n    {"
				<< " 0x" << std::hex << key.fileNameHash << ", 0x" << key.version << ", { ";

			for (size_t j = 0; j < sizeof(key.key); j++) {
				if (j) std::cout << ", ";
				std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << (int)key.key[j];
			}
			std::cout << " } },";
		}
		std::cout << "\n};";

		return tool::OK;
	}

	int CascTestOpt(HANDLE storage) {

		CASC_FIND_DATA data;

		auto firstFileHandle = CascFindFirstFile(storage, "*", &data, NULL);

		if (!firstFileHandle) {
			LOG_ERROR("Can't find first file : 0x{:x}", GetLastError());
			return tool::BASIC_ERROR;
		}

		do {
			if (data.bFileAvailable) {
				LOG_INFO("file: {} / {}", data.szFileName, (int)data.NameType);
			}
		} while (CascFindNextFile(firstFileHandle, &data));

		CascFindClose(firstFileHandle);

		return tool::OK;
	}

	int casctest(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		LOG_INFO("{}", argv[2]);

		HANDLE storage{};

		if (!CascOpenStorage(argv[2], 0, &storage)) {
			LOG_ERROR("Can't open local path {}", argv[2]);
			return tool::BASIC_ERROR;
		}

		auto res = CascTestOpt(storage);

		CascCloseStorage(storage);

		return res;
	}

	int h32ck(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		for (size_t i = 2; i < argc; i++) {
			auto chk = fastfiles::ComputeChecksum32(argv[i], (unsigned int)strlen(argv[i]), 0);

			LOG_INFO("{} 0x{:x}", argv[i], chk);
		}

		return tool::OK;
	}

	int daes_local(Process& proc, int argc, const char* argv[]) {

		for (size_t i = 0; i < sizeof(keys) / sizeof(*keys); i++) {
			auto& k = keys[i];

			if (!k.fileNameHash) {
				continue;
			}

			LOG_INFO("{}, {:x}, 0x{:x}, {}",
				k.fileNameHash, 
				fastfiles::ComputeChecksum32(k.fileNameHash, (unsigned int)strlen(k.fileNameHash), 0), 
				k.version, 
				base64_encode(k.key, sizeof(k.key), false));
		}

		return tool::OK;
	}
}

ADD_TOOL(fftest, "fastfile", " [ff]", "test fastfile", fftest);
ADD_TOOL(ffexe, "fastfile", " [ff]", "test fastfile", ffexe);
ADD_TOOL(casctest, "fastfile", " [path]", "test casc", nullptr, casctest);
ADD_TOOL(h32ck, "fastfile", " [path]", "hash checksum", nullptr, h32ck);
ADD_TOOL(daes, "bo4", "", "dump AES keys", L"BlackOps4.exe", ffdaes);
ADD_TOOL(daes_local, "bo4", "", "dump AES acts keys", nullptr, daes_local);
