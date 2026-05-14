#include <includes.hpp>
#include <tools/fastfile/linkers/linker_bo4.hpp>
#include <core/preprocessor.hpp>

namespace fastfile::linker::bo4 {
	std::unordered_map<XAssetType, XAssetLinker*>& GetWorkers() {
		// could be an array like in bo3 linker
		static std::unordered_map<XAssetType, XAssetLinker*> workers{};
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

	void BO4LinkContext::LinkEmptyAsset(BO4FFContext& ff, XAssetType type, uint64_t name) {
		ff.data.PushStream(XFILE_BLOCK_TEMP);

		size_t len{ games::bo4::pool::GetAssetSize(type) };

		if (!len) {
			LOG_WARNING("linking empty asset");
		}
		else {
			// alloc empty asset
			size_t off{ ff.data.AllocData(len) };

			// set the name
			size_t nameOff{ games::bo4::pool::GetAssetNameOffset(type) };
			ff.data.GetData<XHash>(off + nameOff)->name = name;
		}

		ff.data.PopStream();
	}

	class FFLinkerBO4 : public FFLinker {
	public:
		FFLinkerBO4() : FFLinker("Bo4", "Bo4 fastfile linker") {
		}

		void Link(FastFileLinkerContext& ctx) override {
			BO4LinkContext bo4ctx{ ctx };
			bo4ctx.mainFF.ffname = ctx.mainFFName;
			bo4ctx.mainFF.ffnameHash = bo4ctx.HashXHash(ctx.mainFFName);
			bo4ctx.mainFF.data.SetMode(fastfile::linker::data::LM_DATA);

			for (auto& [t, d] : ctx.zone.assets) {
				XAssetType type{ games::bo4::pool::XAssetIdFromName(t.data()) };
				if (type == XAssetType::ASSET_TYPE_COUNT) {
					bo4ctx.error = true;
					LOG_ERROR("Can't find asset type: {}", t);
					continue;
				}

				for (fastfile::zone::AssetData& assetData : d) {
					LinkAsset(type, bo4ctx, assetData.value, nullptr, true, &bo4ctx.mainFF);
					assetData.handled = true;
				}
			}

			// final linking for all the fastfiles
			for (auto& [type, linker] : GetWorkers()) {
				linker->ComputeFinal(bo4ctx, bo4ctx.mainFF);
				for (auto& [ffname, ff] : bo4ctx.ffs) {
					linker->ComputeFinal(bo4ctx, ff);
				}
			}

			if (bo4ctx.error) {
				throw std::runtime_error("Error when linking fast file data");
			}

			auto LinkFF = [&bo4ctx](BO4FFContext& ff, std::vector<byte>& data, size_t* blocks) {
				if (!ff.data.blockTypes.empty()) {
					throw std::runtime_error(std::format("LinkFF: ff {} is still in a stack, size: {}", ff.ffname, ff.data.blockTypes.size()));
				}

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

	void LinkAsset(XAssetType type, BO4LinkContext& ctx, const char* id, uint64_t* hashOut, bool addAsset, BO4FFContext* ff) {
		// ignore hash identifier
		if (*id == '#') {
			id++;
		}

		if (*id != ',') {
			std::unordered_map<XAssetType, XAssetLinker*>& workers{ GetWorkers() };
			auto it{ workers.find(type) };

			if (it == workers.end()) {
				LOG_ERROR("Can't find asset linker for type {}", games::bo4::pool::XAssetNameFromId(type));
				ctx.error = true;
				return;
			}

			// assign default ff
			if (!ff) {
				ff = &ctx.mainFF;
			}

			// add the asset to the list if required
			if (addAsset && !it->second->isGrouped) {
				ff->data.AddAsset(type, fastfile::linker::data::POINTER_NEXT);
			}

			it->second->Compute(ctx, id, hashOut, *ff);
			return;
		}
		id++; // ','

		// create empty asset

		size_t len{ games::bo4::pool::GetAssetSize(type) };

		if (!len) {
			LOG_ERROR("Can't link empty asset {}", games::bo4::pool::XAssetNameFromId(type));
			ctx.error = true;
			return;
		}

		if (addAsset) {
			ff->data.AddAsset(type, fastfile::linker::data::POINTER_NEXT);
		}
		ff->data.PushStream(XFILE_BLOCK_TEMP);

		void* emptyAsset{ ff->data.AllocDataPtr<void>(len) };
		XHash* h{ games::bo4::pool::GetAssetName(type, emptyAsset, len) };

		if (!h) {
			LOG_ERROR("Can't link asset {} with invalid name location", games::bo4::pool::XAssetNameFromId(type));
			ctx.error = true;
			return;
		}
		// we add the flag 63 to mark it as default
		h->name = ctx.HashPathName(id) | ~hash::MASK63;

		ff->data.PopStream();
	}

	utils::ArrayAdder<FFLinkerBO4, FFLinker> impl{ GetLinkers() };
}