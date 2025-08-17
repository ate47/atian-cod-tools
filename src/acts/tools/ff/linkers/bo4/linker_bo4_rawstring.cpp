#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {
	class RawStringWorker : public LinkerWorker {
	public:
		RawStringWorker() : LinkerWorker("RawString") {}

		void Compute(BO4LinkContext& ctx) override {
			for (fastfile::zone::AssetData& assval : ctx.linkCtx.zone.assets["rawstring"]) {
				assval.handled = true;
				struct RawString {
					XHash name;
					const char* str;
				}; static_assert(sizeof(RawString) == 0x18);

				ctx.mainFF.data.AddAsset(games::bo4::pool::ASSET_TYPE_RAWSTRING, fastfile::linker::data::POINTER_NEXT);

				ctx.mainFF.data.PushStream(XFILE_BLOCK_TEMP);
				RawString rf{};

				rf.name.name = ctx.HashPathName(assval.value);
				rf.str = (const char*)fastfile::linker::data::POINTER_NEXT;
				ctx.mainFF.data.WriteData(rf);

				ctx.mainFF.data.PushStream(XFILE_BLOCK_VIRTUAL);
				ctx.mainFF.data.WriteData(assval.value, std::strlen(assval.value) + 1);
				ctx.mainFF.data.PopStream();

				ctx.mainFF.data.PopStream();

				LOG_INFO("Added asset rawstring {} (hash_{:x})", assval.value, rf.name.name);
			}
		}
	};

	utils::ArrayAdder<RawStringWorker, LinkerWorker> impl{ GetWorkers() };
}