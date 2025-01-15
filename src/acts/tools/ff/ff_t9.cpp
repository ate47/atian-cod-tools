#include <includes.hpp>
#include <deps/oodle.hpp>
#include <tools/fastfile.hpp>
#include <tools/fastfile_handlers.hpp>
#include <games/bo4/pool.hpp>

namespace {
	static deps::oodle::Oodle oodle{};

	class T9FFHandler : public fastfile::FFHandler {
	public:
		T9FFHandler() : fastfile::FFHandler("Black Ops Cold War", 0x46464154, fastfile::MASK32) {}

		void LoadFastFile(fastfile::FFAssetPool& pool, fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, const char* file) {
			if (!reader.CanRead(sizeof(fastfile::TXFileHeader))) {
				throw std::runtime_error("Can't read XFile header");
			}

			fastfile::TXFileHeader* header{ reader.Ptr<fastfile::TXFileHeader>() };

			if (header->encrypted) {
				throw std::runtime_error("Can't read encrypted header");
			}

			// todo: read buffer
		}
	};

	utils::ArrayAdder<T9FFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
}