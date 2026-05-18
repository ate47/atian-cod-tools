#include <includes.hpp>
#include <tools/fastfile/linkers/linker_bo4.hpp>
#include <core/preprocessor.hpp>

namespace fastfile::linker::bo4 {
	BO4FFContext::BO4FFContext() : data{ XFILE_BLOCK_COUNT } {
		data.SetMode(fastfile::linker::memory::LM_DATA);
		data.SetBlockType(XFILE_BLOCK_TEMP, fastfile::linker::memory::BLOCKTYPE_TEMP);
		data.SetBlockType(XFILE_BLOCK_TEMP_PRELOAD, fastfile::linker::memory::BLOCKTYPE_TEMP);
		data.SetBlockType(XFILE_BLOCK_RUNTIME_VIRTUAL, fastfile::linker::memory::BLOCKTYPE_RUNTIME);
		data.SetBlockType(XFILE_BLOCK_RUNTIME_PHYSICAL, fastfile::linker::memory::BLOCKTYPE_RUNTIME);
		data.SetBlockType(XFILE_BLOCK_VIRTUAL, fastfile::linker::memory::BLOCKTYPE_VIRTUAL);
		data.SetBlockType(XFILE_BLOCK_PHYSICAL, fastfile::linker::memory::BLOCKTYPE_VIRTUAL);
		data.SetBlockType(XFILE_BLOCK_STREAMER, fastfile::linker::memory::BLOCKTYPE_RUNTIME);
		data.SetBlockType(XFILE_BLOCK_STREAMER_CPU, fastfile::linker::memory::BLOCKTYPE_RUNTIME);
		data.SetBlockType(XFILE_BLOCK_MEMMAPPED, fastfile::linker::memory::BLOCKTYPE_UNKNOWN);
	}
	std::unordered_map<XAssetType, XAssetLinker*>& GetWorkers() {
		// could be an array like in bo3 linker
		static std::unordered_map<XAssetType, XAssetLinker*> workers{};
		return workers;
	}

	uint32_t BO4LinkContext::HashScr(const char* str) {
		if (!str) {
			return 0;
		}
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
			sctx.data.SetMode(fastfile::linker::memory::LM_DATA);
		}
		return sctx;
	}

	uint64_t BO4LinkContext::HashXHash(const char* str, bool ignoreTop) {
		if (!str) {
			return 0;
		}
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

	static BO4LinkContext* linkContext{};

	BO4LinkContext& GetLinkContext() {
		if (!linkContext) {
			throw std::runtime_error("No BO4LinkContext");
		}
		return *linkContext;
	}

	class FFLinkerBO4 : public FFLinker {
	public:
		FFLinkerBO4() : FFLinker("Bo4", "Bo4 fastfile linker") {
		}

		void Link(FastFileLinkerContext& ctx) override {
			BO4LinkContext bo4ctx{ ctx };
			linkContext = &bo4ctx;
			utils::CloseEnd linkContextCE{ [] {linkContext = nullptr; } };
			bo4ctx.mainFF.ffname = ctx.mainFFName;
			bo4ctx.mainFF.ffnameHash = bo4ctx.HashXHash(ctx.mainFFName);

			for (auto& [t, d] : ctx.zone.assets) {
				XAssetType type{ games::bo4::pool::XAssetIdFromName(t.data()) };
				if (type == XAssetType::ASSET_TYPE_COUNT) {
					bo4ctx.error = true;
					LOG_ERROR("Can't find asset type: {}", t);
					continue;
				}

				for (fastfile::zone::AssetData& assetData : d) {
					void* ref{};
					bo4ctx.LinkAsset(type, assetData.value, ref, true, &bo4ctx.mainFF);
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
				ff.data.SetMode(fastfile::linker::memory::LM_HEADER);

				// add to ctx.data the assets/strings headers
				XAssetList assetlist{};

				// write header
				assetlist.stringList.count = (int)ff.data.scrStrings.size() + 1; // +1 for the null
				assetlist.stringList.strings = fastfile::linker::memory::POINTER_NEXT;

				if (ff.data.assets.size()) {
					assetlist.assetCount = (int)ff.data.assets.size();
					assetlist.assets = fastfile::linker::memory::POINTER_NEXT;
				}
				ff.data.PushStream(XFILE_BLOCK_VIRTUAL);

				ff.data.WriteStream(assetlist);

				ff.data.PushStream(XFILE_BLOCK_VIRTUAL);
				// write string ref array
				ff.data.Align<void*>();

				ff.data.WriteStream<void*>(nullptr); // empty str
				for (size_t i = 0; i < ff.data.scrStrings.size(); i++) {
					ff.data.WriteStream<void*>((void*)fastfile::linker::memory::POINTER_NEXT);
				}

				// write strings
				for (const char* str : ff.data.scrStrings) {
					ff.data.Align<char>();
					ff.data.WriteStream(str);
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
					for (const fastfile::linker::memory::AssetData* asset : ff.data.assets) {
						tmpAsset.type = (games::bo4::pool::XAssetType)asset->type;
						tmpAsset.header = (uintptr_t)asset->header;
						ff.data.WriteStream(tmpAsset);
					}
				}
				ff.data.PopStream();

				ff.data.Link(blocks);

				ff.data.WriteLinkedData(data);

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

	void BO4LinkContext::LinkAsset(XAssetType type, const char* id, void*& ref, bool addAsset, BO4FFContext* ff) {
		if (!id) {
			ref = nullptr;
			return; // empty id = no asset to link
		}

		// ignore hash identifier
		const char* real{ id };
		if (*id == '#') {
			id++;
		}
		bool defaultAsset{ *id == ',' };
		if (defaultAsset) {
			id++;
		}

		// assign default ff
		if (!ff) {
			ff = &mainFF;
		}

		// todo: we can probably check that we don't have this asset somewhere else
		ref = (void*)fastfile::linker::memory::POINTER_NEXT;

		fastfile::linker::memory::LinkerDataChunk* computedRef{};
		fastfile::linker::memory::AssetData* asset{};
		if (!defaultAsset) {
			std::unordered_map<XAssetType, XAssetLinker*>& workers{ GetWorkers() };
			auto it{ workers.find(type) };

			if (it == workers.end()) {
				LOG_ERROR("Can't find asset linker for type {}", games::bo4::pool::XAssetNameFromId(type));
				error = true;
				return;
			}

			// add the asset to the list if required
			if (addAsset && !it->second->isGrouped) {
				asset = ff->data.AddAsset(type);
			}
			try {
				it->second->Compute(*this, id, &computedRef, *ff);
			}
			catch (std::runtime_error& e) {
				error = true;
				LOG_ERROR("Can't link asset {},{} : {}", games::bo4::pool::XAssetNameFromId(type), id, e.what());
			}
		}
		else {
			// create empty asset

			size_t len{ games::bo4::pool::GetAssetSize(type) };

			if (!len) {
				LOG_ERROR("Can't link empty asset {}: '{}'", games::bo4::pool::XAssetNameFromId(type), real);
				error = true;
				return;
			}

			if (addAsset) {
				asset = ff->data.AddAsset(type);
			}
			ff->data.PushStream(XFILE_BLOCK_TEMP);

			computedRef = ff->data.AllocStream(len);
			void* emptyAsset{ computedRef->As<void>() };
			XHash* h{ games::bo4::pool::GetAssetName(type, emptyAsset, len) };

			if (!h) {
				LOG_ERROR("Can't link asset {} with invalid name location: '{}'", games::bo4::pool::XAssetNameFromId(type), real);
				error = true;
				return;
			}
			// we add the flag 63 to mark it as default
			h->name = HashPathName(id) | ~hash::MASK63;

			LOG_INFO("Add default asset {} {} (hash_{:x})", games::bo4::pool::XAssetNameFromId(type), id, h->name);

			ff->data.PopStream();
		}
		if (asset) {
			asset->chunk = computedRef;
		}
	}

	size_t BO4LinkContext::LinkAssetArray(XAssetType type, const char* id, core::config::RapidJsonGeneric& cfg, void** array, size_t count, BO4FFContext* ff) {
		if (cfg.IsNull()) {
			return 0;
		}
		if (!cfg.IsArray()) {
			LOG_ERROR("BO4LinkContext::LinkAssetArray: {} isn't an array", id);
			error = true;
			return 0;
		}
		auto arr{ cfg.GetArray() };
		if (arr.Size() > count) {
			LOG_ERROR("BO4LinkContext::LinkAssetArray: {} can't contain more than {} element(s)", id, count);
			error = true;
			return 0;
		}

		for (rapidjson::Value& v : arr) {
			if (!v.IsString()) {
				LOG_ERROR("BO4LinkContext::LinkAssetArray: {} can't contain a non string element", id);
				error = true;
				continue;
			}

			const char* str{ v.GetString() };
			LinkAsset(type, str, *array, false, ff);
			array++;
		}

		return arr.Size();
	}

	void BO4LinkContext::AddXHash(const char* val, XHash& value) {
		value.name = HashXHash(val, true);
	}

	size_t BO4LinkContext::AddXHashArray(const char* id, core::config::RapidJsonGeneric& cfg, XHash* array, size_t count) {
		if (cfg.IsNull()) {
			return 0;
		}
		if (!cfg.IsArray()) {
			LOG_ERROR("BO4LinkContext::AddXHashArray: {} isn't an array", id);
			error = true;
			return 0;
		}
		auto arr{ cfg.GetArray() };
		if (arr.Size() > count) {
			LOG_ERROR("BO4LinkContext::AddXHashArray: {} can't contain more than {} element(s)", id, count);
			error = true;
			return 0;
		}

		for (rapidjson::Value& v : arr) {
			if (!v.IsString()) {
				LOG_ERROR("BO4LinkContext::AddXHashArray: {} can't contain a non string element", id);
				error = true;
				continue;
			}

			const char* str{ v.GetString() };
			AddXHash(str, *array);
			array++;
		}

		return arr.Size();
	}

	void BO4LinkContext::AddScrString(const char* val, ScrString_t& value, BO4FFContext& ff) {
		value = (ScrString_t)ff.data.AddScrString(val);
	}

	size_t BO4LinkContext::AddScrStringArray(const char* id, core::config::RapidJsonGeneric& cfg, ScrString_t* array, size_t count, BO4FFContext& ff) {
		if (cfg.IsNull()) {
			return 0;
		}
		if (!cfg.IsArray()) {
			LOG_ERROR("BO4LinkContext::AddXHashArray: {} isn't an array", id);
			error = true;
			return 0;
		}
		auto arr{ cfg.GetArray() };
		if (arr.Size() > count) {
			LOG_ERROR("BO4LinkContext::AddXHashArray: {} can't contain more than {} element(s)", id, count);
			error = true;
			return 0;
		}

		for (rapidjson::Value& v : arr) {
			if (!v.IsString()) {
				LOG_ERROR("BO4LinkContext::AddXHashArray: {} can't contain a non string element", id);
				error = true;
				continue;
			}

			const char* str{ v.GetString() };
			AddScrString(str, *array, ff);
			array++;
		}

		return arr.Size();
	}

	bool BO4LinkContext::Assert(bool expr, const char* msg) {
		if (!expr) {
			LOG_ERROR("BO4LinkContext::Assert: {}", msg);
			error = true;
		}
		return expr;
	}

	utils::ArrayAdder<FFLinkerBO4, FFLinker> impl{ GetLinkers() };
}