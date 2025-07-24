#pragma once
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/cw/cw.hpp>

namespace fastfile::handlers::cw {
	typedef uint64_t CWXHash;
	using namespace ::cw;


	struct ScrString_t {
		uint32_t id;

		operator uint32_t() {
			return id;
		}
	};

	typedef float vec2_t[2];
	typedef float vec3_t[3];
	typedef float vec4_t[4];
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
		XFILE_BLOCK_CPU_GPU_SHARE = 0x5,
		XFILE_BLOCK_PHYSICAL = 0x6,
		XFILE_BLOCK_STREAMER = 0x7,
		XFILE_BLOCK_STREAMER_CPU = 0x8,
		XFILE_BLOCK_MEMMAPPED = 0x9,
		XFILE_BLOCK_LOAD_TIME = 0xA,
		XFILE_BLOCK_TEMP_PRELOAD_SAVED_PTR = 0xB,
		XFILE_BLOCK_MESH = 0xC,
		XFILE_BLOCK_COUNT = 0xD,
	};

	struct XAsset_0 {
		XAssetType type;
		void* header;
	}; static_assert(sizeof(XAsset_0) == 0x10);

	struct XAssetList_0 {
		int stringsCount;
		char** strings;
		int* unk10;
		int assetCount;
		XAsset_0* assets;
	}; static_assert(sizeof(XAssetList_0) == 0x28);

	class Worker {
	public:
		const bool ignoreFull;
		Worker(bool ignoreFull = false) : ignoreFull(ignoreFull) {}
		virtual void Unlink(fastfile::FastFileOption& opt, void* ptr) = 0;
		virtual void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) {}
		virtual void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) {}
	};

	std::unordered_map<XAssetType, Worker*>& GetWorkers();

	const char* GetScrString(ScrString_t id);
	const char* XFileBlockName(XFileBlock id);
	bool IsValidHandle(const void* handle);
	const char* GetValidString(const char* handle, const char* defaultVal = nullptr);

	class BOCWJsonWriter : public utils::raw_file_extractor::JsonWriter {
	public:
		using utils::raw_file_extractor::JsonWriter::JsonWriter;

		void WriteFieldValueScrString(const char* name, ScrString_t val) {
			if (!val) return;
			JsonWriter::WriteFieldValueString(name, GetScrString(val));
		}

		void WriteFieldValueScrString(uint64_t hash, ScrString_t val) {
			if (!val) return;
			JsonWriter::WriteFieldValueString(hash, GetScrString(val));
		}

		void WriteFieldValueScrStringArray(const char* name, size_t count, ScrString_t* val, bool ignoreEmpty = true) {
			if (ignoreEmpty && (!count || !*val)) return;
			JsonWriter::WriteFieldNameString(name);
			JsonWriter::BeginArray();
			for (size_t i = 0; i < count; i++) {
				if (!val[i]) break;
				JsonWriter::WriteValueString(GetScrString(val[i]));
			}
			JsonWriter::EndArray();
		}

		void WriteFieldValueScrStringArray(uint64_t hash, size_t count, ScrString_t* val, bool ignoreEmpty = true) {
			if (ignoreEmpty && (!count || !*val)) return;
			JsonWriter::WriterFieldNameHash(hash);
			JsonWriter::BeginArray();
			for (size_t i = 0; i < count; i++) {
				if (!val[i]) break;
				JsonWriter::WriteValueString(GetScrString(val[i]));
			}
			JsonWriter::EndArray();
		}

		/*
		void WriteFieldValueXAsset(const char* name, games::bo4::pool::XAssetType type, void* val) {
			if (!val) return;
			XHash* hname{ games::bo4::pool::GetAssetName(type, val) };
			if (*hname) {
				WriteFieldValueXHash(name, *hname);
			}
		}

		void WriteFieldValueXAsset(uint64_t hash, games::bo4::pool::XAssetType type, void* val) {
			if (!val) return;
			XHash* hname{ games::bo4::pool::GetAssetName(type, val) };
			if (*hname) {
				WriteFieldValueXHash(hash, *hname);
			}
		}
		void WriteFieldValueXAssetArray(const char* name, games::bo4::pool::XAssetType type, size_t count, void* handle, bool ignoreEmpty = true) {
			void** val{ (void**)handle };
			if (ignoreEmpty && (!count || !*val)) return;
			size_t off{ games::bo4::pool::GetAssetNameOffset(type) };

			JsonWriter::WriteFieldNameString(name);
			JsonWriter::BeginArray();

			for (size_t i = 0; i < count; i++) {
				if (!val[i]) break;
				XHash* hname{ (XHash*)((byte*)val[i] + off) };
				JsonWriter::WriteValueHash(*hname);
			}
			JsonWriter::EndArray();
		}

		void WriteFieldValueXAssetArray(uint64_t hash, games::bo4::pool::XAssetType type, size_t count, void* handle, bool ignoreEmpty = true) {
			void** val{ (void**)handle };
			if (ignoreEmpty && (!count || !*val)) return;
			size_t off{ games::bo4::pool::GetAssetNameOffset(type) };

			JsonWriter::WriterFieldNameHash(hash);
			JsonWriter::BeginArray();

			for (size_t i = 0; i < count; i++) {
				if (!val[i]) break;
				XHash* hname{ (XHash*)((byte*)val[i] + off) };
				JsonWriter::WriteValueHash(*hname);
			}
			JsonWriter::EndArray();
		}
		*/
	};
}
