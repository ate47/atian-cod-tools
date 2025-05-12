#pragma once
#include <tools/ff/fastfile_handlers.hpp>
#include <games/bo4/pool.hpp>

namespace fastfile::handlers::bo4 {
	struct ScrString_t {
		uint32_t id;

		operator uint32_t() {
			return id;
		}
	};
	typedef float vec3_t[3];
	typedef uint64_t ID64Metatable;
	typedef const char* XString;
	struct WeaponDef;
	struct ScriptBundle;
	struct Character;
	struct FootstepTableDef;
	struct SurfaceFXTableDef;
	struct AnimMappingTable;
	struct AimTable;
	struct ShootTable;
	struct BehaviorTree;
	struct AnimStateMachine;
	struct AnimSelectorTableSet;
	struct GfxImage;
	struct Material;
	struct Gesture;
	struct SurfaceSoundDef;
	struct SurfaceFXTableDef;
	struct DestructibleDef;
	struct RumbleInfo;
	struct FxEffectDef;
	struct VehicleFxDef;
	struct XModel;
	struct VehicleSoundDef;
	typedef FxEffectDef* FxEffectDefHandle;
	typedef Material* MaterialHandle;

	enum XFileBlock : uint32_t {
		XFILE_BLOCK_TEMP = 0x0,
		XFILE_BLOCK_TEMP_PRELOAD = 0x1,
		XFILE_BLOCK_RUNTIME_VIRTUAL = 0x2,
		XFILE_BLOCK_RUNTIME_PHYSICAL = 0x3,
		XFILE_BLOCK_VIRTUAL = 0x4,
		XFILE_BLOCK_PHYSICAL = 0x5,
		XFILE_BLOCK_STREAMER = 0x6,
		XFILE_BLOCK_STREAMER_CPU = 0x7,
		XFILE_BLOCK_MEMMAPPED = 0x8,
		XFILE_BLOCK_COUNT = 0x9,
		XFILE_BLOCK_MEMMAPPED2 = 0xA,
		XFILE_BLOCK_COUNT2 = 0xB,
	};

	enum XFileBlockMemLocation : int {
		XFILE_BLOCK_LOC_VIRTUAL = 0,
		XFILE_BLOCK_LOC_PHYSICAL = 1,
		XFILE_BLOCK_LOC_STREAM = 2,
		XFILE_BLOCK_LOC_STREAM_CPU = 3,
		XFILE_BLOCK_LOC_STREAM_DEV = 4, // generate 1777362536 by default, should be 0
		XFILE_BLOCK_LOC_STREAM_MMAP = 5,
	};

	struct XAsset_0 {
		games::bo4::pool::XAssetType type;
		void* header;
	}; static_assert(sizeof(XAsset_0) == 0x10);

	struct XAssetList_0 {
		int stringsCount;
		char** strings;
		int assetCount;
		XAsset_0* assets;
	}; static_assert(sizeof(XAssetList_0) == 0x20);

	class Worker {
	public:
		virtual void Unlink(fastfile::FastFileOption& opt, void* ptr) = 0;
		virtual void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) {}
		virtual void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) {}
	};

	std::unordered_map<games::bo4::pool::XAssetType, Worker*>& GetWorkers();
	const char* GetScrString(ScrString_t id);
	void SetAssetList(XAssetList_0* assetList);
	const char* XFileBlockName(XFileBlock id);

	XFileBlockMemLocation GetXFileBlockMemLocation(XFileBlock id);

	bool IsValidHandle(const void* handle);

	const char* GetValidString(const char* handle, const char* defaultVal = nullptr);

}

std::ostream& operator<<(std::ostream& os, const fastfile::handlers::bo4::ScrString_t& scr);
template<>
struct std::formatter<fastfile::handlers::bo4::ScrString_t, char> : utils::BasicFormatter<fastfile::handlers::bo4::ScrString_t> {};