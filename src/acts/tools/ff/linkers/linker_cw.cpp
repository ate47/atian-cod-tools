#include <includes.hpp>
#include <tools/ff/linkers/linker_cw.hpp>
#include <compiler/preprocessor.hpp>

namespace fastfile::linker::cw {
	std::vector<LinkerWorker*>& GetWorkers() {
		static std::vector<LinkerWorker*> workers{};
		return workers;
	}

	uint32_t BOCWLinkContext::HashScr(const char* str) {
		uint64_t r;
		if (hash::TryHashPattern(str, r)) {
			return (uint32_t)r; // nothing to add
		}

		r = hash::HashT89Scr(str);

		linkCtx.RegisterHash(r, str);

		return (uint32_t)r;
	}

	uint64_t BOCWLinkContext::HashXHash(const char* str) {
		uint64_t r;
		if (hash::TryHashPattern(str, r)) {
			return r; // nothing to add
		}

		r = hash::Hash64(str);

		linkCtx.RegisterHash(r, str);

		return r;
	}

	uint64_t BOCWLinkContext::HashPathName(const std::filesystem::path& path) {
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

	class FFLinkerBOCW : public FFLinker {
	public:
		FFLinkerBOCW() : FFLinker("CW", "BoCW fastfile linker") {
		}

		void Init(FastFileLinkerOption& opt) override {
			std::vector<LinkerWorker*>& w{ GetWorkers() };

			std::sort(w.begin(), w.end(), [](LinkerWorker* a, LinkerWorker* b) { return a->priority > b->priority; });
		}

		void Link(FastFileLinkerContext& ctx) override {
			BOCWLinkContext bocwctx{ ctx };
			bocwctx.ffnameHash = hash::Hash64Pattern(ctx.ffname);
			ctx.RegisterHash(bocwctx.ffnameHash, ctx.ffname);


			// load files into bocwctx.assetData

			bocwctx.data.SetMode(fastfile::linker::data::LM_DATA);

			for (LinkerWorker* w : GetWorkers()) {
				LOG_DEBUG("Compute '{}'...", w->id);
				w->Compute(bocwctx);
			}

			if (bocwctx.error) {
				throw std::runtime_error("Error when linking fast file data");
			}

			bocwctx.data.SetMode(fastfile::linker::data::LM_HEADER);

			// add to ctx.data the assets/strings headers
			XAssetList assetlist{};

			// write header
			assetlist.stringList.count = (int)bocwctx.data.strings.size() + 1; // +1 for the null
			assetlist.stringList.strings = fastfile::linker::data::POINTER_NEXT;

			if (bocwctx.data.assets.size()) {
				assetlist.assetCount = (int)bocwctx.data.assets.size();
				assetlist.assets = fastfile::linker::data::POINTER_NEXT;
			}
			bocwctx.data.PushStream(XFILE_BLOCK_VIRTUAL);

			bocwctx.data.WriteData(assetlist);

			bocwctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
			// write string ref array
			bocwctx.data.Align<void*>();

			bocwctx.data.WriteData<void*>(nullptr); // empty str
			for (size_t i = 0; i < bocwctx.data.strings.size(); i++) {
				bocwctx.data.WriteData<void*>((void*)fastfile::linker::data::POINTER_NEXT);
			}

			// write strings
			for (const char* str : bocwctx.data.strings) {
				bocwctx.data.Align<char>();
				bocwctx.data.WriteData(str);
			}
			bocwctx.data.PopStream();

			if (bocwctx.data.assets.size()) {
				bocwctx.data.Align(8); // GetAlignment_XAsset

				struct XAsset {
					XAssetType type;
					uintptr_t header; // XAssetHeader
				};
				// write asset array

				XAsset tmpAsset{};
				for (const fastfile::linker::data::AssetData& asset : bocwctx.data.assets) {
					tmpAsset.type = (XAssetType)asset.type;
					tmpAsset.header = (uintptr_t)asset.header;
					bocwctx.data.WriteData(tmpAsset);
				}
			}
			bocwctx.data.PopStream();

			bocwctx.data.Link(ctx.linkedData, ctx.blockSizes);

			LOG_INFO("Fastfile data linked with {} asset(s) and {} string(s)", assetlist.assetCount, assetlist.stringList.count);
		}

	};

	utils::ArrayAdder<FFLinkerBOCW, FFLinker> impl{ GetLinkers() };
}