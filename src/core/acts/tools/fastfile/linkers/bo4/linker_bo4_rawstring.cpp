#include <includes.hpp>
#include <tools/fastfile/linkers/linker_bo4.hpp>

namespace {
	using namespace fastfile::linker::bo4;
	struct RawString {
		XHash name;
		const char* str;
	};
	static_assert(sizeof(RawString) == 0x18);

	class XAssetLinkerImpl : public XAssetLinker {
	public:
		using XAssetLinker::XAssetLinker;

		void Compute(BO4LinkContext& ctx, const char* id, uint64_t* hashOut, BO4FFContext& ff) override {
			ff.data.PushStream(XFILE_BLOCK_TEMP);
			RawString& rf{ ff.data.AllocStreamRef< RawString>() };

			rf.name.name = ctx.HashPathName(id);
			rf.str = (const char*)fastfile::linker::memory::POINTER_NEXT;

			ff.data.PushStream(XFILE_BLOCK_VIRTUAL);
			ff.data.WriteStream(id);
			ff.data.PopStream();

			ff.data.PopStream();

			LOG_INFO("Added asset rawstring {} (hash_{:x})", id, rf.name.name);
		}
	};

	utils::MapAdder<XAssetLinkerImpl, XAssetType, XAssetLinker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_RAWSTRING };
}