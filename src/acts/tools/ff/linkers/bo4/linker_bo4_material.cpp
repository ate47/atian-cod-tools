#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

namespace fastfile::linker::bo4 {
	class MaterialWorker : public LinkerWorker {
	public:
		MaterialWorker() : LinkerWorker("Material", -2) {}

		void Compute(BO4LinkContext& ctx) override {
			std::vector<std::filesystem::path> images{};
			std::filesystem::path imagesDir{ ctx.linkCtx.input / "materials" };


			// todo
		}
	};

	//utils::ArrayAdder<MaterialWorker, LinkerWorker> impl{ GetWorkers() };
}