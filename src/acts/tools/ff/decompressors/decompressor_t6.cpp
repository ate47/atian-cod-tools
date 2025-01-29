#include <includes.hpp>
#include <deps/oodle.hpp>
#include <tools/fastfile.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <games/bo4/pool.hpp>
#include <Salsa20.h>

namespace {
	static byte key[]{ 
		0x64, 0x1D, 0x8A, 0x2F, 0xE3, 0x1D, 0x3A, 0xA6, 0x36, 0x22, 0xBB, 0xC9, 0xCE, 0x85, 0x87, 0x22, 
		0x9D, 0x42, 0xB0, 0xF8, 0xED, 0x9B, 0x92, 0x41, 0x30, 0xBF, 0x88, 0xB6, 0x5E, 0xDC, 0x50, 0xBE 
	};
	static deps::oodle::Oodle oodle{};

	void fillIV(byte* table, size_t len, const char* name) {
		size_t s{ std::strlen(name) };
		size_t s4{ s * 4 };

		int32_t addDiv{};
		for (size_t i = 0; i < len; i += s4) {
			for (size_t x = 0; x < s4; x += 4) {
				if ((i + addDiv) >= len || i + x >= len) {
					return;
				}
				if (x > 0) {
					addDiv = x / 4;
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
			};

			if (!reader.CanRead(sizeof(T6FileHeader))) {
				throw std::runtime_error("Can't read header");
			}

			uint32_t version{ reader.ReadPtr<T6FileHeader>()->version };
			size_t fastFileSize;
			size_t decompressedSizeLoc{};
			size_t fastfileNameLoc;

			switch (version) {
			case 0x93:
				fastfileNameLoc = 0x18;
				fastFileSize = 0x138;
				break;
			default:
				throw std::runtime_error(std::format("Unknown version 0x{:x}", version));
			}

			reader.Goto(fastfileNameLoc);
			ctx.ffname = reader.ReadString();
			constexpr size_t tableLen = 16000;
			auto table{ std::make_unique<byte>(tableLen) };
			int32_t counter[4]{};

			fillIV(table.get(), tableLen, ctx.ffname);

			ucstk::Salsa20 salsa20{ key };
			salsa20.setIv(nullptr);

		}

	};
	utils::ArrayAdder<T6FFDecompressor, fastfile::FFDecompressor> arr{ fastfile::GetDecompressors() };
}