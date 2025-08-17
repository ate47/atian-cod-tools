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
	BO4FFContext& BO4LinkContext::GetFFContext(const char* prefix) {
		BO4FFContext& sctx{ ffs[prefix] };
		if (!sctx.ffnameHash) {
			sctx.ffname = this->linkCtx.strs.CloneStr(utils::va("%s%s", prefix, mainFF.ffname));
			sctx.ffnameHash = HashXHash(sctx.ffname);
			sctx.data.SetMode(fastfile::linker::data::LM_DATA);
		}
		return sctx;
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
			bo4ctx.mainFF.ffname = ctx.mainFFName;
			bo4ctx.mainFF.ffnameHash = bo4ctx.HashXHash(ctx.mainFFName);
			bo4ctx.mainFF.data.SetMode(fastfile::linker::data::LM_DATA);

			for (LinkerWorker* w : GetWorkers()) {
				LOG_DEBUG("Compute '{}'...", w->id);
				w->Compute(bo4ctx);
			}
			
			if (bo4ctx.error) {
				throw std::runtime_error("Error when linking fast file data");
			}
				
			bo4ctx.linkCtx.zone.AssertAllHandled(true);

			auto LinkFF = [&bo4ctx](BO4FFContext& ff, std::vector<byte>& data, size_t* blocks) {

				ff.data.SetMode(fastfile::linker::data::LM_HEADER);

				// add to ctx.data the assets/strings headers
				XAssetList assetlist{};

				// write header
				assetlist.stringList.count = (int)ff.data.strings.size() + 1; // +1 for the null
				assetlist.stringList.strings = fastfile::linker::data::POINTER_NEXT;

				if (ff.data.assets.size()) {
					assetlist.assetCount = (int)ff.data.assets.size();
					assetlist.assets = fastfile::linker::data::POINTER_NEXT;
				}
				ff.data.PushStream(XFILE_BLOCK_VIRTUAL);

				ff.data.WriteData(assetlist);

				ff.data.PushStream(XFILE_BLOCK_VIRTUAL);
				// write string ref array
				ff.data.Align<void*>();

				ff.data.WriteData<void*>(nullptr); // empty str
				for (size_t i = 0; i < ff.data.strings.size(); i++) {
					ff.data.WriteData<void*>((void*)fastfile::linker::data::POINTER_NEXT);
				}

				// write strings
				for (const char* str : ff.data.strings) {
					ff.data.Align<char>();
					ff.data.WriteData(str);
				}
				ff.data.PopStream();

				if (ff.data.assets.size()) {
					ff.data.Align(8); // GetAlignment_XAsset

					struct XAsset {
						games::bo4::pool::XAssetType type;
						uintptr_t header; // XAssetHeader
					};
					// write asset array

					XAsset tmpAsset{};
					for (const fastfile::linker::data::AssetData& asset : ff.data.assets) {
						tmpAsset.type = (games::bo4::pool::XAssetType)asset.type;
						tmpAsset.header = (uintptr_t)asset.header;
						ff.data.WriteData(tmpAsset);
					}
				}
				ff.data.PopStream();

				ff.data.Link(data, blocks);

				LOG_INFO("Fastfile {} data linked with {} asset(s) and {} string(s)", ff.ffname, assetlist.assetCount, assetlist.stringList.count);
			};

			fastfile::FastFile& mff{ ctx.fastfiles.emplace_back() };
			mff.ffname = ctx.mainFFName;
			LinkFF(bo4ctx.mainFF, mff.linkedData, mff.blockSizes);

			for (auto& [k, ff] : bo4ctx.ffs) {
				fastfile::FastFile& mff{ ctx.fastfiles.emplace_back() };
				mff.ffname = ff.ffname;
				LinkFF(ff, mff.linkedData, mff.blockSizes);
			}
		}

	};

	utils::ArrayAdder<FFLinkerBO4, FFLinker> impl{ GetLinkers() };
}