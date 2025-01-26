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
			if (!reader.CanRead(sizeof(fastfile::TXFileHeader))) {
				throw std::runtime_error("Can't read XFile header");
			}

			fastfile::TXFileHeader* header{ reader.Ptr<fastfile::TXFileHeader>() };

			if (header->encrypted) {
				throw std::runtime_error("Can't read encrypted header");
			}

		}

	};
	utils::ArrayAdder<T10FFDecompressor, fastfile::FFDecompressor> arr{ fastfile::GetDecompressors() };
}