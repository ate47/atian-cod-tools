#pragma once
#include <games/bo4/pool.hpp>

namespace fastfile::handlers::bo4 {
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

	enum XFileBlockMemLocation : uint32_t {
		XFILE_BLOCK_LOC_VIRTUAL = 0,
		XFILE_BLOCK_LOC_PHYSICAL = 1,
		XFILE_BLOCK_LOC_STREAM = 2,
		XFILE_BLOCK_LOC_STREAM_CPU = 3,
		XFILE_BLOCK_LOC_STREAM_DEV = 4, // generate 1777362536 by default, should be 0
		XFILE_BLOCK_LOC_STREAM_MMAP = 5,
	};

	struct XBlock {
		byte* data;
		uint64_t size;
		uint64_t unk10;
	};

	const char* XFileBlockName(XFileBlock id);
	XFileBlockMemLocation GetXFileBlockMemLocation(XFileBlock id);
	void DB_AllocXBlocks(size_t* blockSize, XBlock* blocks);
	void DB_InitStreams(XBlock* blocks);
	void DB_LoadXFileData(void* pos, size_t size);
	void DB_IncStreamPos(size_t size);
	bool Load_Stream(bool atStreamStart, void* ptr, size_t size);

	void* DB_AllocStreamPos(int align);

	template<typename T = void, size_t align = sizeof(T) - 1>
	T* AllocStreamPos() {
		return (T*)DB_AllocStreamPos(align);
	}
	void DB_PopStreamPos();
	bool DB_EnableMemMappedData();
	void DB_PushStreamPos(XFileBlock index);
	const void** DB_InsertPointer();
	void* DB_GetOffsetData(uint64_t ref);
	void DB_ConvertOffsetToPointer(void** data);
	void DB_ConvertOffsetToAlias(void** data);
	void Load_XStringCustom(char** str);
	void Load_String(bool atStreamStart, char** handle);

	inline void Load_String(bool atStreamStart, const char** handle) {
		Load_String(atStreamStart, (char**)handle);
	}
	void Load_SimpleAsset_Internal(void** header, games::bo4::pool::XAssetType type);
	XAsset_0* DB_LinkXAssetEntry(XAsset_0* xasset, bool allowOverride);
}