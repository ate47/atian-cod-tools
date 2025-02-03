#include <includes.hpp>
#include <hook/memory.hpp>
#include <tools/fastfile.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <games/bo4/pool.hpp>
#include <tools/utils/compress_utils.hpp>

namespace {
	enum IWFFVersion {
		IWFV_MW19 = 0x0B,
		IWFV_MW23 = 0x18,
		IWFV_BO6 = 0x19,
	};

	// bo6  "48 83 EC 28 FF C9 83"
	// mw23 "48 83 EC 28 FF C9 83 F9 10 77"
	void DecompressBlock(fastfile::FastFileIWCompression type, const void* src, void* dest, unsigned int srcLen, unsigned int destLen) {
		utils::compress::CompressionAlgorithm alg{ fastfile::GetFastFileCompressionAlgorithm(type) };
		if (!utils::compress::Decompress(alg, dest, destLen, src, srcLen)) {
			throw std::runtime_error(std::format("Can't decompress {} data", fastfile::GetFastFileCompressionName(type)));
		}
	}

	class IWFFDecompressor : public fastfile::FFDecompressor {
		hook::module_mapper::Module game{};
	public:
		IWFFDecompressor() : fastfile::FFDecompressor("IW", 0x3030317066665749, 0xFFFFFF00FFFFFFFF) {}

		void Init(fastfile::FastFileOption& opt) override {
			if (!opt.game) {
				return; // no game
			}
			if (!game.Load(opt.game)) {
				throw std::runtime_error(std::format("Can't load {}", opt.game));
			}
		}

		void LoadFastFile(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx, std::vector<byte>& ffdata) {
			struct DB_FFHeader
			{
				byte magic[8];
				unsigned int headerVersion;
				unsigned int xfileVersion;
				uint32_t flags;
				uint32_t unk14;
				uint64_t unk18;
				uint64_t unk20;
			};

			if (!reader.CanRead(sizeof(DB_FFHeader))) {
				throw std::runtime_error("Can't read XFile header");
			}

			DB_FFHeader* header{ reader.Ptr<DB_FFHeader>() };

			char platform{ (char)header->magic[4] };

			hook::library::Library lib{ *game };
			if (opt.m_header) {
				LOG_INFO("version: 0x{:x} / 0x{:x}", header->headerVersion, header->xfileVersion);
				LOG_INFO("flags: 0x{:x}, plt: {}", header->flags, platform);
			}
			switch (header->headerVersion) {
			case IWFV_BO6: {
				reader.Goto(0xe0);
				byte* compHeader{ reader.Ptr() };
				// todo
				throw std::runtime_error("bo6 header decrypter not implemented");
			}
			case IWFV_MW23: {
				reader.Goto(0xe0);
				byte* compHeader{ reader.Ptr() };
				// todo
				throw std::runtime_error("mw23 header decrypter not implemented");
			}
				break;
			default:
				throw std::runtime_error(std::format("version not supported 0x{:x}", header->headerVersion));
			}
		}

	};
	utils::ArrayAdder<IWFFDecompressor, fastfile::FFDecompressor> arr{ fastfile::GetDecompressors() };
}