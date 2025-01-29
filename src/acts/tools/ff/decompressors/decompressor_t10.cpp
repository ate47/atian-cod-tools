#include <includes.hpp>
#include <deps/oodle.hpp>
#include <tools/fastfile.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <games/bo4/pool.hpp>

namespace {
	static deps::oodle::Oodle oodle{};

	class T10FFDecompressor : public fastfile::FFDecompressor {
	public:
		T10FFDecompressor() : fastfile::FFDecompressor("Black Ops 6", 0x3030316166665749, fastfile::MASK64) {}

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

			if (opt.m_header) {
				LOG_INFO("version: 0x{:x} / 0x{:x}", header->headerVersion, header->xfileVersion);
				LOG_INFO("flags: 0x{:x}", header->flags);
			}

			if (header->headerVersion != 0x19) {
				throw std::runtime_error(std::format("Unknown version 0x{:x}", header->headerVersion));
			}

			throw std::runtime_error("Not yet fully implemented");
		}

	};
	utils::ArrayAdder<T10FFDecompressor, fastfile::FFDecompressor> arr{ fastfile::GetDecompressors() };
}