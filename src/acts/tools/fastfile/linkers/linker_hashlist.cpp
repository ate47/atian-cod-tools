#include <includes.hpp>
#include <core/bytebuffer.hpp>
#include <tools/fastfile/linkers/linker_bo4.hpp>
#include <tools/fastfile/linkers/linker_cw.hpp>

namespace fastfile::linker {
	template<typename LinkerWorker, typename LinkContext>
	class ImplWorker : public LinkerWorker {
	public:
		ImplWorker() : LinkerWorker("HashList") {}

		void Compute(LinkContext& ctx) override {
			for (fastfile::zone::AssetData& assval : ctx.linkCtx.zone.assets["hashlist"]) {
				std::filesystem::path path{ ctx.linkCtx.input / assval.value };
				assval.handled = true;

				utils::InFileCE is{ path };
				if (!is) {
					LOG_ERROR("Can't open {}", path.string());
					ctx.error = true;
					continue;
				}

				std::string line;
				while (is->good() && std::getline(*is, line)) {
					if (line[0] == '#') {
						if (line[1]) {
							ctx.HashScr(&line[1]);
						}
					}
					else {
						if (line[0]) {
							ctx.HashXHash(&line[0]);
						}
					}
				}

				LOG_INFO("Added hashlist {}", path.string());
			}
		}
	};
	utils::ArrayAdder<ImplWorker<bo4::LinkerWorker, bo4::BO4LinkContext>, bo4::LinkerWorker> implbo4{ bo4::GetWorkers() };
	utils::ArrayAdder<ImplWorker<cw::LinkerWorker, cw::BOCWLinkContext>, cw::LinkerWorker> implcw{ cw::GetWorkers() };
}