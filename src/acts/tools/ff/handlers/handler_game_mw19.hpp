#pragma once
#pragma once
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/mw19/mw19.hpp>

namespace fastfile::handlers::mw19 {
	// used for fast changes
	constexpr const char* gamePath = "mw19";
	constexpr const char* gameExe = "ModernWarfare_dump.exe";
	constexpr const char* gameDumpId = "mw19";
	constexpr const char* handlerId = "mw19";
	constexpr const char* handlerName = "Modern Warfare (2019)";

	using namespace tool::mw19;

	typedef IW8PoolType HandlerAssetType;
	typedef IW8HashAssetType HandlerHashedAssetType;

	struct GfxImage;
	struct MaterialAnimation;
	struct ParticleSystem;
	struct FootStepsFxTable;
	struct FoliagesFXTable;
	struct HandPlantsFXTable;
	struct AiImpactVFXTable;
	struct Dismemberment;
	struct CalloutMarkerPing;
	struct WeaponAccuracy;
	struct Character;
	struct XModel;
	struct XBoneSet;
	struct ReactiveAudioPackage;
	struct ReactiveVFXPackage;
	struct FootStepVFX;
	struct Camo;
	struct Material;
	struct ScriptBundle;
	struct PlayerSpawnSettings;

	typedef const char* XString;

	struct ScrString_t {
		uint32_t id;

		operator uint32_t() {
			return id;
		}
	};

	class Worker {
	public:
		bool requiresRelativeLoads;
		size_t assetSize;

		Worker(size_t assetSize, bool requiresRelativeLoads = false) : requiresRelativeLoads(requiresRelativeLoads), assetSize(assetSize) {}
		virtual void GenDefaultXHashes(fastfile::FastFileContext* ctx) {}
		virtual void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) = 0;
		virtual void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) {}
		virtual void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) {}
	};

	const char* GetPoolName(uint32_t hash);
	const char* GetScrString(ScrString_t id);
	HandlerHashedAssetType GetHashType(HandlerAssetType type);
	HandlerAssetType GetExePoolId(const char* name);
	HandlerAssetType GetExePoolId(HandlerHashedAssetType name);
	XString GetXAssetName(HandlerHashedAssetType type, void* handle);

	class HandlerJsonWriter : public utils::raw_file_extractor::JsonWriter {
	public:
		using utils::raw_file_extractor::JsonWriter::JsonWriter;

		void WriteFieldValueXString(const char* name, XString val) {
			if (!val) return;
			JsonWriter::WriteFieldValueString(name, val);
		}

		void WriteFieldValueScrString(const char* name, ScrString_t val) {
			if (!val) return;
			JsonWriter::WriteFieldValueString(name, GetScrString(val));
		}


		void WriteValueScrString(ScrString_t val) {
			if (!val) return;
			JsonWriter::WriteValueString(GetScrString(val));
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

		void WriteFieldValueXAsset(const char* name, HandlerHashedAssetType type, void* val) {
			if (!val) return;
			XString hname{ GetXAssetName(type, val) };
			if (hname) {
				WriteFieldValueXString(name, hname);
			}
		}

		void WriteFieldValueXAssetArray(const char* name, HandlerHashedAssetType type, size_t count, void* handle, bool ignoreEmpty = true) {
			void** val{ (void**)handle };
			if (ignoreEmpty && (!count || !*val)) return;

			JsonWriter::WriteFieldNameString(name);
			JsonWriter::BeginArray();

			for (size_t i = 0; i < count; i++) {
				if (!val[i]) break;
				JsonWriter::WriteValueString(GetXAssetName(type, val[i]));
			}
			JsonWriter::EndArray();
		}
	};

	std::vector<const char*>* GetXStrings();
	std::unordered_map<HandlerHashedAssetType, Worker*>& GetWorkers();
}