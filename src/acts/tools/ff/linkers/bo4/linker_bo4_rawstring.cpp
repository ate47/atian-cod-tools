#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {
	class RawStringWorker : public LinkerWorker {
	public:
		RawStringWorker() : LinkerWorker("RawString") {}

		void Compute(BO4LinkContext& ctx) override {
			for (const char*& value : ctx.linkCtx.zone.assets["rawstring"]) {
				struct RawString {
					XHash name;
					const char* str;
				}; static_assert(sizeof(RawString) == 0x18);

				ctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_RAWSTRING, fastfile::linker::data::POINTER_NEXT);

				ctx.data.PushStream(XFILE_BLOCK_TEMP);
				RawString rf{};

				rf.name.name = ctx.HashPathName(value);
				rf.str = (const char*)fastfile::linker::data::POINTER_NEXT;
				ctx.data.WriteData(rf);

				ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
				ctx.data.WriteData(value, std::strlen(value) + 1);
				ctx.data.PopStream();

				ctx.data.PopStream();

				LOG_INFO("Added asset rawstring {} (hash_{:x})", value, rf.name.name);
			}
		}
	};

	utils::ArrayAdder<RawStringWorker, LinkerWorker> impl{ GetWorkers() };
}