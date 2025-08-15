#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>
#include <compiler/preprocessor.hpp>

namespace fastfile::linker::bo4 {
	std::vector<LinkerWorker*>& GetWorkers() {
		static std::vector<LinkerWorker*> workers{};
		return workers;
	}

	uint32_t BO4LinkContext::HashScr(const char* str) {
		uint64_t r;
		if (hash::TryHashPattern(str, r)) {
			return (uint32_t)r; // nothing to add
		}

		r = hash::HashT89Scr(str);

		linkCtx.RegisterHash(r, str);

		return (uint32_t)r;
	}

	uint64_t BO4LinkContext::HashXHash(const char* str, bool ignoreTop) {
		if (ignoreTop && *str == '#') {
			str++;
		}
		uint64_t r;
		if (hash::TryHashPattern(str, r)) {
			return r; // nothing to add
		}

		r = hash::Hash64(str);

		linkCtx.RegisterHash(r, str);

		return r;
	}

	uint64_t BO4LinkContext::HashPathName(const std::filesystem::path& path) {
		std::filesystem::path p{ path };
		if (p.has_extension()) {
			p.replace_extension();
		}
		std::string fn{ p.filename().string() };
		uint64_t r;
		if (!hash::TryHashPattern(fn.data(), r)) {
			fn = path.string();
			r = HashXHash(fn.data());
		}
		LOG_TRACE("Hash path {} -> 0x{:x}", path.string(), r);
		return r;

	}

	class FFLinkerBO4 : public FFLinker {
	public:
		FFLinkerBO4() : FFLinker("Bo4", "Bo4 fastfile linker") {
		}

		void Init(FastFileLinkerOption& opt) override {
			std::vector<LinkerWorker*>& w{ GetWorkers() };

			std::sort(w.begin(), w.end(), [](LinkerWorker* a, LinkerWorker* b) { return a->priority > b->priority; });
		}

		void Link(FastFileLinkerContext& ctx) override {
			BO4LinkContext bo4ctx{ ctx };
			bo4ctx.ffnameHash = hash::Hash64Pattern(ctx.ffname);
			ctx.RegisterHash(bo4ctx.ffnameHash, ctx.ffname);


			// load files into bo4ctx.assetData

			bo4ctx.data.SetMode(fastfile::linker::data::LM_DATA);

			for (LinkerWorker* w : GetWorkers()) {
				LOG_DEBUG("Compute '{}'...", w->id);
				w->Compute(bo4ctx);
			}
			
			if (bo4ctx.error) {
				throw std::runtime_error("Error when linking fast file data");
			}
				
			bo4ctx.linkCtx.zone.AssertAllHandled(true);

			bo4ctx.data.SetMode(fastfile::linker::data::LM_HEADER);

			// add to ctx.data the assets/strings headers
			XAssetList assetlist{};
			
			// write header
			assetlist.stringList.count = (int)bo4ctx.data.strings.size() + 1; // +1 for the null
			assetlist.stringList.strings = fastfile::linker::data::POINTER_NEXT;

			if (bo4ctx.data.assets.size()) {
				assetlist.assetCount = (int)bo4ctx.data.assets.size();
				assetlist.assets = fastfile::linker::data::POINTER_NEXT;
			}
			bo4ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);

			bo4ctx.data.WriteData(assetlist);

			bo4ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
			// write string ref array
			bo4ctx.data.Align<void*>();

			bo4ctx.data.WriteData<void*>(nullptr); // empty str
			for (size_t i = 0; i < bo4ctx.data.strings.size(); i++) {
				bo4ctx.data.WriteData<void*>((void*)fastfile::linker::data::POINTER_NEXT);
			}

			// write strings
			for (const char* str : bo4ctx.data.strings) {
				bo4ctx.data.Align<char>();
				bo4ctx.data.WriteData(str);
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

			LOG_INFO("Fastfile data linked with {} asset(s) and {} string(s)", assetlist.assetCount, assetlist.stringList.count);
		}

	};

	utils::ArrayAdder<FFLinkerBO4, FFLinker> impl{ GetLinkers() };
}