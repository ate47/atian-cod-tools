#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {
	std::vector<LinkerWorker*>& GetWorkers() {
		static std::vector<LinkerWorker*> workers{};
		return workers;
	}

	uint64_t HashPathName(const std::filesystem::path& path) {
		std::filesystem::path p{ path };
		if (p.has_extension()) {
			p.replace_extension();
		}
		std::string fn{ p.filename().string() };
		uint64_t r;
		if (!hash::TryHashPattern(fn.data(), r)) {
			fn = path.string();
			r = hash::Hash64(fn.data());
		}
		LOG_TRACE("Hash path {} -> 0x{:x}", path.string(), r);
		return r;

	}

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
			cfg.SyncConfig(false);

			std::string ffName{ cfg.GetString("data.name") };

			if (!ffName.empty()) {
				ctx.ffname = ctx.strs.CloneStr(ffName);
			}
			
			BO4LinkContext bo4ctx{ ctx, cfg };
			bo4ctx.ffnameHash = hash::Hash64Pattern(ctx.ffname);


			// load files into bo4ctx.assetData

			for (LinkerWorker* w : GetWorkers()) {
				LOG_DEBUG("Compute '{}'...", w->id);
				w->Compute(bo4ctx);
			}
			
			if (bo4ctx.error) {
				throw std::runtime_error("Error when linking fast file data");
			}

			// add to ctx.data the assets/strings headers
			XAssetList& assetlist{ utils::Allocate<XAssetList>(ctx.data) };

			// write header
			if (bo4ctx.strings.size()) {
				assetlist.stringList.count = (int)bo4ctx.strings.size();
				assetlist.stringList.strings = fastfile::ALLOC_PTR;
			}

			if (bo4ctx.assets.size()) {
				assetlist.assetCount = (int)bo4ctx.assets.size();
				assetlist.assets = fastfile::ALLOC_PTR;
			}

			if (bo4ctx.strings.size()) {
				// write string ref array
				uintptr_t* ptrs{ utils::AllocateArray<uintptr_t>(ctx.data, bo4ctx.strings.size()) };
				for (size_t i = 0; i < bo4ctx.strings.size(); i++) {
					ptrs[i] = fastfile::ALLOC_PTR;
				}

				// write strings
				for (const char* str : bo4ctx.strings) {
					utils::WriteString(ctx.data, str);
				}
			}
			if (bo4ctx.assets.size()) {
				// write asset array
				utils::WriteValue(ctx.data, bo4ctx.assets.data(), bo4ctx.assets.size() * sizeof(bo4ctx.assets[0]));
			}

			// write asset data
			utils::WriteValue(ctx.data, bo4ctx.assetData.data(), bo4ctx.assetData.size());
		}

	};

	utils::ArrayAdder<FFLinkerBO4, FFLinker> impl{ GetLinkers() };
}