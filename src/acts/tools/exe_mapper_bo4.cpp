#include <includes.hpp>
#include <hook/module_mapper.hpp>
#include <hook/error.hpp>
#include <deps/oodle.hpp>

namespace {
	struct {
		char locationDir[MAX_PATH + 1]{};
	} bo4;

	hook::library::Detour Com_Error_Detour;
	hook::library::Detour Sys_GetAbsZoneDir_Detour;

	void Com_Error_Stub(uint32_t code) {
		throw std::runtime_error(utils::va("Com_Error(0x%x)", code));
	}

	const char* Sys_GetAbsZoneDir_Stub() {
		return utils::va("%s/zone", bo4.locationDir);
	}
	
	enum PMemStack : __int32
	{
		PMEM_STACK_DB = 0x0,
		PMEM_STACK_DB2 = 0x1,
		PMEM_STACK_GAME = 0x2,
		PMEM_STACK_SERVER = 0x3,
		PMEM_STACK_HOT = 0x4,
		PMEM_STACK_CINEMATICS = 0x5,
		PMEM_STACK_DYNAMIC_IMAGES = 0x6,
		PMEM_STACK_LIGHTING = 0x7,
		PMEM_STACK_BINARY_PATCH = 0x8,
		PMEM_STACK_DEMO_DOWNLOAD = 0x9,
		PMEM_STACK_MOTION_MATCHING_KEYFRAMES = 0xA,
		PMEM_STACK_COUNT = 0xB,
		PMEM_STACK_INVALID = -1,
		PHYS_ALLOC_LOW = 0x0,
		PHYS_ALLOC_HIGH = 0x2,
		PHYS_ALLOC_COUNT = 0xB,
	};
	typedef void DB_Interrupt(void);



	int bo4_exe_mapper(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

		hook::module_mapper::Module mod{ true };
		if (!mod.Load(argv[2])) {
			LOG_ERROR("Can't map module {}", argv[2]);
			return tool::BASIC_ERROR;
		}

		strcpy_s(bo4.locationDir, argv[3]);

		LOG_INFO("Module loaded");

		deps::oodle::Oodle oodle{};

		if (!oodle.LoadOodleFromGame(*mod)) {
			LOG_ERROR("Can't load oodle");
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Deps loaded");

		Com_Error_Detour.Create((*mod)[0x288B110], Com_Error_Stub);
		Sys_GetAbsZoneDir_Detour.Create((*mod)[0x2895E40], Sys_GetAbsZoneDir_Stub);

		LOG_INFO("Hooks loaded");

		//DB_LoadXFile(char const*, int, XZoneBuffer*, char const*, XAssetList*, XBlock*, void (*)(void), uchar*, PMemStack, int).text	0000000002E0CC10	00001573	00000808	0000004C	R	.	.	..B	T	.

		void (*DB_LoadXFile)(char const* path, int file, void* buffer, char const* filename, void* assetlist, void* xblock, DB_Interrupt inter, byte * buf, PMemStack side, int flags)
			= (decltype(DB_LoadXFile))(*mod)[0x2E0CC1];

		return tool::OK;
	}

	ADD_TOOL(bo4_exe_mapper, "bo4", "[exe] [gamedir]", "test bo4 mapping", bo4_exe_mapper);
}