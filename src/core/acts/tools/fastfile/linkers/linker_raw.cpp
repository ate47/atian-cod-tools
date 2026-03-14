#include <includes.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>

namespace {
	using namespace fastfile;

	class FFLinkerRaw : public FFLinker {
	public:
		FFLinkerRaw() : FFLinker("Raw", "raw fastfile linker") {
		}

		void Link(FastFileLinkerContext& ctx) override {
			if (!std::filesystem::is_regular_file(ctx.input)) {
				throw std::runtime_error("Raw linker can't link non regular file");
			}

			/*
			if (!utils::ReadFile(ctx.input, ctx.data)) {
				throw std::runtime_error(std::format("Can't read {}", ctx.input.string()));
			}

			LOG_INFO("Read 0x{:x} bytes of raw data", ctx.data.size());
			*/
		}

	};

	//utils::ArrayAdder<FFLinkerRaw, FFLinker> impl{ GetLinkers() };
}