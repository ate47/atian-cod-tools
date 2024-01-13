#include <includes.hpp>

#pragma warning(push)
#pragma warning(disable:4996)
#include <CascLib.h>
#include <CascCommon.h>
#pragma warning(pop)

namespace {

	void WriteHex(std::ostream& out, uintptr_t base, BYTE* buff, SIZE_T size) {
		for (size_t j = 0; j < size; j++) {
			if (j % 8 == 0) {
				if (base) {
					out << std::hex << std::setw(16) << std::setfill('0') << (base + j) << "~";
				}
				out << std::hex << std::setw(3) << std::setfill('0') << j << "|";
				if (j + 7 < size) {
					out << std::hex << std::setw(16) << std::setfill('0') << *reinterpret_cast<UINT64*>(&buff[j]);
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
					auto val = *reinterpret_cast<UINT64*>(&buff[j - 7]);
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
	struct XFile {
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
	};
	struct aesKey_t {
		UINT32 fileNameHash;
		UINT32 version;
		byte key[32];
	};

	aesKey_t keys[64] = {
		{ 0x7c7b91e, 0x1, { 0x2c, 0xa0, 0x5c, 0x8c, 0x0b, 0x4b, 0x16, 0xda, 0xe1, 0x19, 0x0a, 0x97, 0x64, 0xfd, 0xc6, 0x40, 0xdd, 0x8c, 0x5d, 0xb1, 0x9e, 0xe3, 0x20, 0x40, 0xe0, 0x22, 0x7c, 0x28, 0xaf, 0x93, 0x9b, 0x2c } },
		{ 0xa7136b28, 0x1, { 0xd5, 0x79, 0xac, 0xf1, 0xb8, 0xaf, 0x0e, 0x7b, 0xed, 0x95, 0xef, 0x46, 0x94, 0xdc, 0xb7, 0x20, 0x7b, 0x03, 0x05, 0x88, 0xb9, 0xab, 0xa9, 0x12, 0x25, 0x3a, 0x8c, 0x25, 0x46, 0x5b, 0x50, 0x1a } },
		{ 0x1f5c5dbd, 0x1, { 0xb9, 0x59, 0x31, 0x39, 0x3c, 0x4d, 0xda, 0xf2, 0x82, 0x24, 0x7b, 0x76, 0xa6, 0x26, 0x39, 0xb4, 0x6a, 0xb2, 0xdf, 0x72, 0xca, 0x67, 0x47, 0x4a, 0x94, 0xa8, 0xb3, 0xba, 0xd6, 0x62, 0x7e, 0xe2 } },
		{ 0x700dd1df, 0x1, { 0xed, 0xee, 0x8a, 0x02, 0x6a, 0xb1, 0x08, 0xad, 0xe7, 0x37, 0x75, 0xbb, 0xd6, 0xb8, 0xd9, 0x1e, 0x97, 0x45, 0x6a, 0xa0, 0x12, 0x78, 0x12, 0x6b, 0x28, 0x95, 0xf7, 0x30, 0xb9, 0xfa, 0x4e, 0x00 } },
		{ 0xccbd3ea9, 0x1, { 0xa9, 0xe8, 0x1c, 0x1d, 0x3f, 0x48, 0x4f, 0xe3, 0xa3, 0x3d, 0xe0, 0x75, 0x84, 0x3f, 0x0a, 0x75, 0x0d, 0x4b, 0xcb, 0xe9, 0xa3, 0x6e, 0x47, 0x35, 0xeb, 0x9c, 0xb6, 0xa3, 0x22, 0x6b, 0x2a, 0x31 } },
		{ 0xe6570b7, 0x1, { 0x19, 0x43, 0xc6, 0xb6, 0xb3, 0x64, 0x74, 0xa0, 0xdb, 0x86, 0xff, 0x41, 0xb1, 0xe4, 0x45, 0x88, 0x15, 0xac, 0xef, 0x93, 0xea, 0xcb, 0x0b, 0xea, 0x0f, 0xb6, 0xb7, 0xa8, 0xe4, 0x59, 0x33, 0x3e } },
		{ 0xa8453fab, 0x1, { 0xa9, 0x83, 0xb0, 0xdd, 0x11, 0xe9, 0x7f, 0xfe, 0x0b, 0xa0, 0xda, 0xa1, 0x6b, 0xb0, 0xfc, 0xc3, 0x49, 0x75, 0xb8, 0x9b, 0x35, 0x6b, 0xc7, 0xf5, 0x19, 0x7b, 0xf3, 0xf5, 0x14, 0xa5, 0x72, 0x97 } },
		{ 0x849d20a0, 0x1, { 0x83, 0x10, 0xd0, 0x8b, 0x91, 0xd1, 0x79, 0x0f, 0xa3, 0x55, 0xeb, 0x8c, 0xd9, 0x58, 0x9d, 0x19, 0xf6, 0x0f, 0x1b, 0xfb, 0x48, 0x86, 0xc0, 0x82, 0x1f, 0x9c, 0xde, 0x16, 0x2f, 0x84, 0x7f, 0x8d } },
		{ 0xb3a23baa, 0x1, { 0xf2, 0x2b, 0x37, 0x58, 0xd9, 0xec, 0xdf, 0x05, 0x33, 0xfb, 0xf8, 0x8f, 0xfa, 0x02, 0xea, 0x38, 0x4e, 0xcf, 0xbd, 0x4a, 0xc4, 0x8c, 0x09, 0x42, 0xeb, 0x9e, 0x2e, 0x48, 0x25, 0x6a, 0x45, 0xf7 } },
		{ 0xfa2808eb, 0x1, { 0x35, 0xce, 0x15, 0x0d, 0xe2, 0xdb, 0x3d, 0x7f, 0xee, 0x4d, 0x51, 0x6d, 0xf1, 0xd9, 0xad, 0x37, 0x38, 0x02, 0xc6, 0xb8, 0xa6, 0x40, 0xdf, 0x19, 0x64, 0x39, 0xee, 0x84, 0x8c, 0x38, 0x4f, 0x81 } },
		{ 0x245d3f4a, 0x1, { 0x07, 0x6d, 0xff, 0x8e, 0x01, 0x88, 0x93, 0xf6, 0x4d, 0x99, 0x09, 0x12, 0x0b, 0xdf, 0x20, 0x21, 0x3a, 0x5e, 0x36, 0x8a, 0xc1, 0x90, 0xb7, 0x55, 0x17, 0xaf, 0x3e, 0x28, 0xf4, 0xa9, 0x11, 0xc2 } },
		{ 0xada6316, 0x1, { 0x69, 0xdd, 0xbf, 0x42, 0xd8, 0x00, 0xfc, 0xce, 0xad, 0x31, 0x4b, 0x34, 0x81, 0x31, 0xa5, 0x14, 0xa1, 0xc0, 0x28, 0x18, 0xea, 0xaf, 0xf6, 0x83, 0xe0, 0x2e, 0xd3, 0xc1, 0x07, 0xd5, 0x00, 0x3d } },
		{ 0xba0bcfbd, 0x1, { 0x1c, 0x17, 0x3a, 0xe6, 0xa0, 0x0a, 0x0e, 0xe5, 0x7b, 0x20, 0xea, 0xa2, 0x1c, 0x62, 0xba, 0x30, 0xa8, 0x67, 0xaa, 0x04, 0x2a, 0x12, 0xe1, 0xab, 0x5c, 0x4e, 0x34, 0xa3, 0xf3, 0xc4, 0xa3, 0x9b } },
		{ 0x6f840221, 0x1, { 0xd6, 0xf6, 0xea, 0x66, 0x1c, 0x6f, 0x17, 0xad, 0x86, 0x06, 0xf6, 0x34, 0xea, 0x7e, 0x91, 0x63, 0x56, 0x0a, 0x34, 0x2d, 0xeb, 0x5b, 0x0b, 0x3c, 0x12, 0x91, 0xff, 0xc9, 0x6b, 0x3b, 0xc8, 0x35 } },
		{ 0xda7e7e8f, 0x1, { 0x4c, 0x90, 0x93, 0x14, 0x76, 0x54, 0xce, 0x47, 0x01, 0xe7, 0x1d, 0x29, 0x06, 0x15, 0x93, 0x21, 0xfc, 0x45, 0x9a, 0x74, 0xda, 0x3d, 0x1d, 0x42, 0x96, 0xbd, 0x29, 0x48, 0xc4, 0x82, 0x12, 0xcc } },
		{ 0x2e39076, 0x1, { 0x88, 0x85, 0x7a, 0xe5, 0x22, 0x0a, 0x36, 0xf9, 0x76, 0xe2, 0x18, 0xe8, 0x7b, 0x7b, 0xbc, 0x81, 0xeb, 0x8f, 0x5d, 0xa2, 0x7c, 0xad, 0x1d, 0x7c, 0x77, 0x2e, 0xc6, 0xbb, 0x12, 0xbd, 0xd7, 0xa0 } },
	};

	int __fastcall ComputeChecksum32(const char* buffer, unsigned int len, int start)
	{
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

	int HandleFF(XFile* buffer, size_t size) {
		if (size < sizeof(*buffer)) {
			std::cerr << "FF too small for header\n";
			return tool::BASIC_ERROR;
		}
		if (*reinterpret_cast<UINT64*>(buffer->magic) != 0x3030303066664154) {
			std::cerr << "Invalid FF magic: " << std::hex << *reinterpret_cast<UINT64*>(buffer) << "\n";
			return tool::BASIC_ERROR;
		}

		if (buffer->version != 0x27F) {
			std::cerr << "Not a T8 FF\n";
			return tool::BASIC_ERROR;
		}

		//std::cout << "version: " << std::dec << buffer->version << "\n";

		std::cout << "server ..... " << (buffer->server ? "true" : "false") << "\n";
		std::cout << "encrypted .. " << (buffer->encrypted ? "true" : "false") << "\n";
		std::cout << "platform ... " << std::dec << (int)buffer->platform << "\n";

		for (size_t i = 0; i < 4; i++) {
			std::cout << "archiveChecksum[" << i << "] = " << std::hex << buffer->archiveChecksum[i] << "\n";
		}

		std::cout << "key: ....... " << buffer->key << "\n";
		std::cout << "compress: .. " << (int)buffer->compression << "\n";

		// 6 = inflate?
		// 7 = lzma?
		// 8 = ?


		if (buffer->encrypted) {
			auto checksum32 = ComputeChecksum32(buffer->key, (unsigned int)strlen(buffer->key), 0);
			std::cout << "checksum32 . " << checksum32 << "\n";
		}

		WriteHex(std::cout, 0, reinterpret_cast<BYTE*>(buffer), sizeof(*buffer));

		std::cout << "-----------------------------\n";

		WriteHex(std::cout, reinterpret_cast<uintptr_t>(buffer + 1) - reinterpret_cast<uintptr_t>(buffer), reinterpret_cast<BYTE*>(buffer + 1), size - sizeof(*buffer));



		return tool::OK;
	}

	int fftest(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		LPVOID buffer{};
		size_t size{};

		if (!utils::ReadFileNotAlign(argv[2], buffer, size, false)) {
			std::cerr << "Can't read file " << argv[2] << "\n";
			return tool::BASIC_ERROR;
		}

		auto res = HandleFF(reinterpret_cast<XFile*>(buffer), size);

		std::free(buffer);

		return res;
	}
	int ffdaes(Process& proc, int argc, const char* argv[]) {

		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		LPVOID buffer{};
		size_t size{};

		if (!utils::ReadFileNotAlign(argv[2], buffer, size, false)) {
			std::cerr << "Can't read file " << argv[2] << "\n";
			return tool::BASIC_ERROR;
		}

		if (size >= sizeof(aesKey_t[64])) {
			std::cout
				<< "struct aesKey_t {\n"
				<< "    int fileNameHash;\n"
				<< "    int version; \n"
				<< "    byte key[32];\n"
				<< "}\n"
				<< "aesKey_t keys[64] = {"
				;
			auto* keys = reinterpret_cast<aesKey_t*>(buffer);
			for (size_t i = 0; i < 64; i++) {
				auto& key = keys[i];

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
		}
		else {
			std::cerr << "Bad file\n";
		}

		std::free(buffer);

		return tool::OK;
	}

	int CascTestOpt(HANDLE storage) {

		CASC_FIND_DATA data;

		auto firstFileHandle = CascFindFirstFile(storage, "*", &data, NULL);

		if (!firstFileHandle) {
			auto err = GetLastError();
			std::cerr << "Can't find first file : 0x" << std::hex << err << "\n";
			return tool::BASIC_ERROR;
		}

		do {
			if (data.bFileAvailable) {
				std::cout << "file: " << data.szFileName << " / " << (int)data.NameType << "\n";
			}
		} while (CascFindNextFile(firstFileHandle, &data));

		CascFindClose(firstFileHandle);

		return tool::OK;
	}

	int casctest(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		std::wstring path = utils::StrToWStr(argv[2]);

		std::wcout << path << "\n";

		HANDLE storage{};

		if (!CascOpenStorage(path.c_str(), 0, &storage)) {
			std::cerr << "Can't open local path\n";
			return tool::BASIC_ERROR;
		}

		auto res = CascTestOpt(storage);

		CascCloseStorage(storage);

		return res;
	}
}
#ifndef CI_BUILD

ADD_TOOL("fftest", " [ff]", "test fastfile", nullptr, fftest);
ADD_TOOL("casctest", " [path]", "test casc", nullptr, casctest);
ADD_TOOL("ff_aes", " [aes]", "dump AES keys", nullptr, ffdaes);

#endif