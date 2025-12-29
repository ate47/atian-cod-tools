#include <includes.hpp>
#include <deps/oodle.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/fastfile_data_tre.hpp>
#include <games/bo4/pool.hpp>
#include <Salsa20.h>

namespace {
	static byte key[]{ 
		0x64, 0x1D, 0x8A, 0x2F, 0xE3, 0x1D, 0x3A, 0xA6, 0x36, 0x22, 0xBB, 0xC9, 0xCE, 0x85, 0x87, 0x22, 
		0x9D, 0x42, 0xB0, 0xF8, 0xED, 0x9B, 0x92, 0x41, 0x30, 0xBF, 0x88, 0xB6, 0x5E, 0xDC, 0x50, 0xBE 
	};
	static deps::oodle::Oodle oodle{};

	void FillIV(byte* table, size_t len, const char* name) {
		size_t s{ std::strlen(name) };
		size_t s4{ s * 4 };
		int32_t addDiv{};
		for (size_t i = 0; i < len; i += s4) {
			for (size_t x = 0; x < s4; x += 4) {
				if ((i + addDiv) >= len || i + x >= len) {
					return;
				}
				if (x > 0) {
					addDiv = (int32_t)(x / 4);
				}
				else {
					addDiv = 0;
				}

				for (int y = 0; y < 4; y++) {
					table[i + x + y] = name[addDiv];
				}
			}
		}
	}

	class T6FFDecompressor : public fastfile::FFDecompressor {
	public:
		T6FFDecompressor() : fastfile::FFDecompressor("Black Ops 2", 0x3030313066664154, fastfile::MASK64) {}

		void LoadFastFile(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx, std::vector<byte>& ffdata) {
			struct T6FileHeader {
				uint8_t magic[8];
				uint32_t version;
				char builderName[0x0c];
				char ffname[0x20];
				byte iv[0x100];
			};
			static_assert(sizeof(T6FileHeader) == 0x138);

			if (!reader.CanRead(sizeof(T6FileHeader))) {
				throw std::runtime_error("Can't read header");
			}
			reader.Skip<uint64_t>(); // magic
			uint32_t version{ reader.Read<uint32_t>() };
			size_t fastFileSize;
			size_t decompressedSizeLoc{};
			size_t fastfileNameLoc;

			switch (version) {
			case 0x93:
				fastfileNameLoc = offsetof(T6FileHeader, ffname);
				fastFileSize = sizeof(T6FileHeader);
				break;
			default:
				throw std::runtime_error(std::format("Unknown version 0x{:x}", version));
			}

			reader.Goto(fastfileNameLoc);
			const char* ffname{ reader.ReadString() };
			sprintf_s(ctx.ffname, "%s", ffname);

			constexpr size_t tableLen = 16000;
			auto table{ std::make_unique<byte[]>(tableLen)};
			size_t counter[4]{ 1, 1, 1, 1 };
			byte iv[8]{};

			FillIV(table.get(), tableLen, ctx.ffname);

			ucstk::Salsa20 salsa20{ key };

			reader.Goto(fastFileSize);

			size_t blockId{};
			while (true) {
				int32_t size{ reader.Read<int32_t>() };

				if (!size) {
					break; // done
				}

				size_t ivLoc{ (blockId + 4 * (counter[blockId] - 1)) % 800 * 20 };
				byte iv[8];
				std::memcpy(iv, &table[ivLoc], sizeof(iv));
				salsa20.setIv(iv);

				size_t unloc{ utils::Allocate(ffdata, size) };
				byte* decompressed{ &ffdata[unloc] };
				//salsa20.

				blockId++;
			}
		}

	};
#ifndef CI_BUILD
	utils::ArrayAdder<T6FFDecompressor, fastfile::FFDecompressor> arr{ fastfile::GetDecompressors() };
#endif
}