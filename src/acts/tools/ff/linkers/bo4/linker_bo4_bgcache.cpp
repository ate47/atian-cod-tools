#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {
	class BGCacheWorker : public LinkerWorker {
	public:
		// force the priority to last so the other tools can add entries
		BGCacheWorker() : LinkerWorker("BGCache", MININT32) {}

		void Compute(BO4LinkContext& ctx) override {

			size_t entries{};
			for (auto& [_, v] : ctx.bgcache) {
				for (auto& vv : v) {
					entries++;
				}
			}

			if (!entries) {
				return; // nothing to build
			}

			struct BGCacheInfoDef {
				games::bo4::pool::BGCacheTypes type;
				XHash name;
				uint64_t asset;
			}; static_assert(sizeof(BGCacheInfoDef) == 0x20);

			struct BGCacheInfo {
				XHash name;
				BGCacheInfoDef* def;
				int defCount;
			}; static_assert(sizeof(BGCacheInfo) == 0x20);

			ctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_BG_CACHE, fastfile::linker::data::POINTER_NEXT);

			ctx.data.PushStream(XFILE_BLOCK_TEMP);
			BGCacheInfo bg{};

			bg.name.name = ctx.ffnameHash;
			bg.def = (BGCacheInfoDef*)fastfile::linker::data::POINTER_NEXT;
			bg.defCount = (uint32_t)entries;
			ctx.data.WriteData(bg);

			ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
			ctx.data.Align(8);
			BGCacheInfoDef* defs{ ctx.data.AllocDataPtr<BGCacheInfoDef>(sizeof(BGCacheInfoDef) * entries) };
			
			for (auto& [type, v] : ctx.bgcache) {
				for (auto& name : v) {
					defs->name.name = name;
					defs->type = type;
					defs++;
				}
			}
			ctx.data.PopStream();

			ctx.data.PopStream();

			LOG_INFO("Added asset bgcache {} (hash_{:x})", ctx.linkCtx.ffname, ctx.ffnameHash);
		}
	};

	utils::ArrayAdder<BGCacheWorker, LinkerWorker> impl{ GetWorkers() };
}