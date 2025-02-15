#include <includes.hpp>
#include <core/config.hpp>
#include <tools/fastfile.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <games/bo4/pool.hpp>

namespace {
	using namespace fastfile;

	struct Asset {
		games::bo4::pool::XAssetType type;
		void* location;
	};

	struct BO4LinkContext {
		FastFileLinkerContext& linkCtx;
		std::vector<byte> assetData{};
		std::vector<const char*> strings{};
		std::vector<Asset> assets{};

	};

	class FFLinkerBO4 : public FFLinker {
	public:
		FFLinkerBO4() : FFLinker("Bo4", "Bo4 fastfile linker") {
		}

		void Link(FastFileLinkerContext& ctx) override {
			if (std::filesystem::is_regular_file(ctx.input)) {
				throw std::runtime_error("bo4 linker can't link regular file");
			}

			// load common configs
			core::config::Config cfg{ ctx.input / "config.json" };

			std::string ffName{ cfg.GetString("data.name") };

			if (!ffName.empty()) {
				ctx.ffname = ctx.strs.CloneStr(ffName);
			}
			
			BO4LinkContext bo4ctx{ ctx };


			// todo: load files into bo4ctx.assetData


			// todo: add to ctx.data the assets/strings headers
			
			throw std::runtime_error("not implemented");
		}

	};

	utils::ArrayAdder<FFLinkerBO4, FFLinker> impl{ GetLinkers() };
}