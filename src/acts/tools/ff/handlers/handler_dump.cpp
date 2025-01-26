#include <includes.hpp>
#include <tools/ff/fastfile_handlers.hpp>

namespace {

	class DumpFFHandler : public fastfile::FFHandler {
	public:
		DumpFFHandler() : fastfile::FFHandler("Dump", "Dump decompressed files") {
		}

		void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
			std::filesystem::path of{ ctx.file };
			std::filesystem::path decfile{ opt.m_output / ctx.ffname };

			decfile.replace_extension(".ff.dec");

			std::filesystem::create_directories(decfile.parent_path());
			if (!utils::WriteFile(decfile, reader.Ptr(), reader.Length())) {
				LOG_ERROR("Can't dump {}", decfile.string());
			}
			else {
				LOG_INFO("Dump into {}", decfile.string());
			}
		}
	};

	utils::ArrayAdder<DumpFFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
}
