#pragma once
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <games/bo4/pool.hpp>

namespace fastfile::handlers::bo4 {
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
	struct PlayerSoundsTable;
	struct PlayerFXTable;
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
	struct WeaponCamo;
	struct WeaponAttachment;
	struct WeaponAttachmentUnique;
	struct WeaponTunables;
	struct SharedWeaponSounds;
	struct XAnim;
	struct TagFxSet;
	struct XCam;
	struct LaserDef;
	struct BeamDef;
	struct FlameTable;
	struct BallisticDesc;
	struct Objective;
	struct FxImpactTable;
	struct SoundsImpactTable;
	struct XModelAlias;
	struct RagdollDef;
	struct GestureTable;
	typedef FxEffectDef* FxEffectDefHandle;
	typedef Material* MaterialHandle;

	enum eModes : int32_t {
		MODE_ZOMBIES = 0x0,
		MODE_MULTIPLAYER = 0x1,
		MODE_CAMPAIGN = 0x2,
		MODE_WARZONE = 0x3,
		MODE_COUNT = 0x4,
		MODE_INVALID = 0x4,
		MODE_FIRST = 0x0,
	};

	class Worker {
	public:
		const bool ignoreFull;
		Worker(bool ignoreFull = false) : ignoreFull(ignoreFull) {}
		virtual void Unlink(fastfile::FastFileOption& opt, void* ptr) = 0;
		virtual void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) {}
		virtual void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) {}
	};

	std::unordered_map<games::bo4::pool::XAssetType, Worker*>& GetWorkers();
	fastfile::FastFileContext* GetFFContext();
	void SetFFContext(fastfile::FastFileContext* ctx);
	const char* GetScrString(ScrString_t id);
	const char* GetEModeName(eModes mode);

	bool IsValidHandle(const void* handle);

	const char* GetValidString(const char* handle, const char* defaultVal = nullptr);

	class BO4JsonWriter : public utils::raw_file_extractor::JsonWriter {
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

		void WriteFieldValueXHash(const char* name, XHash& val) {
			if (!val) return;
			JsonWriter::WriteFieldValueHash(name, val);
		}

		void WriteFieldValueXHash(uint64_t hash, XHash& val) {
			if (!val) return;
			JsonWriter::WriteFieldValueHash(hash, val);
		}

		void WriteFieldValueXString(const char* name, const char* val) {
			if (!val) return;
			JsonWriter::WriteFieldValueString(name, val);
		}

		void WriteFieldValueXString(uint64_t hash, const char* val) {
			if (!val) return;
			JsonWriter::WriteFieldValueString(hash, val);
		}

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

		void WriteValueXAsset(games::bo4::pool::XAssetType type, void* val) {
			if (!val) {
				WriteValueNull();
				return;
			}
			XHash* hname{ games::bo4::pool::GetAssetName(type, val) };
			WriteValueHash(*hname);
		}

		void WriteFieldValueScrStringArray(const char* name, size_t count, ScrString_t* val, bool ignoreEmpty = true) {
			if (ignoreEmpty && (!count || !val || !*val)) return;
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

		void WriteFieldValueXHashArray(const char* name, size_t count, XHash* val, bool ignoreEmpty = true) {
			if (ignoreEmpty && (!count || !*val)) return;
			JsonWriter::WriteFieldNameString(name);
			JsonWriter::BeginArray();
			for (size_t i = 0; i < count; i++) {
				if (!val[i]) break;
				JsonWriter::WriteValueHash(val[i]);
			}
			JsonWriter::EndArray();
		}

		void WriteFieldValueXHashArray(uint64_t hash, size_t count, XHash* val, bool ignoreEmpty = true) {
			if (ignoreEmpty && (!count || !*val)) return;
			JsonWriter::WriterFieldNameHash(hash);
			JsonWriter::BeginArray();
			for (size_t i = 0; i < count; i++) {
				if (!val[i]) break;
				JsonWriter::WriteValueHash(val[i]);
			}
			JsonWriter::EndArray();
		}

		void WriteFieldValueXAssetArray(const char* name, games::bo4::pool::XAssetType type, size_t count, void* handle, bool ignoreEmpty = true) {
			void** val{ (void**)handle };
			if (ignoreEmpty && (!count || !val || !*val)) return;
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
	};
}

std::ostream& operator<<(std::ostream& os, const fastfile::handlers::bo4::ScrString_t& scr);
template<>
struct std::formatter<fastfile::handlers::bo4::ScrString_t, char> : utils::BasicFormatter<fastfile::handlers::bo4::ScrString_t> {};