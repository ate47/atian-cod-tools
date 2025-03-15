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

			bo4ctx.data.SetMode(fastfile::linker::data::LM_DATA);

			for (LinkerWorker* w : GetWorkers()) {
				LOG_DEBUG("Compute '{}'...", w->id);
				w->Compute(bo4ctx);
			}
			
			if (bo4ctx.error) {
				throw std::runtime_error("Error when linking fast file data");
			}

			bo4ctx.data.SetMode(fastfile::linker::data::LM_HEADER);

			// add to ctx.data the assets/strings headers
			XAssetList assetlist{};
			
			// write header
			if (bo4ctx.data.strings.size()) {
				assetlist.stringList.count = (int)bo4ctx.data.strings.size();
				assetlist.stringList.strings = fastfile::linker::data::POINTER_NEXT;
			}

			if (bo4ctx.data.assets.size()) {
				assetlist.assetCount = (int)bo4ctx.data.assets.size();
				assetlist.assets = fastfile::linker::data::POINTER_NEXT;
			}
			bo4ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);

			bo4ctx.data.WriteData(assetlist);

			bo4ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
			if (bo4ctx.data.strings.size()) {
				// write string ref array
				bo4ctx.data.Align<void*>();

				for (size_t i = 0; i < bo4ctx.data.strings.size(); i++) {
					bo4ctx.data.WriteData<void*>((void*)fastfile::linker::data::POINTER_NEXT);
				}

				// write strings
				for (const char* str : bo4ctx.data.strings) {
					bo4ctx.data.Align<char>();
					bo4ctx.data.WriteData(str);
				}
			}
			bo4ctx.data.PopStream();

			if (bo4ctx.data.assets.size()) {
				bo4ctx.data.Align(8); // GetAlignment_XAsset

				struct XAsset {
					games::bo4::pool::XAssetType type;
					uintptr_t header; // XAssetHeader
				};
				// write asset array
				
				XAsset tmpAsset{};
				for (const fastfile::linker::data::AssetData& asset : bo4ctx.data.assets) {
					tmpAsset.type = (games::bo4::pool::XAssetType)asset.type;
					tmpAsset.header = (uintptr_t)asset.header;
					bo4ctx.data.WriteData(tmpAsset);
				}
			}
			bo4ctx.data.PopStream();

			bo4ctx.data.Link(ctx.linkedData, ctx.blockSizes);
		}

	};

	utils::ArrayAdder<FFLinkerBO4, FFLinker> impl{ GetLinkers() };
}