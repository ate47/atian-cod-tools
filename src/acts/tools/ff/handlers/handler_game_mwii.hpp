#pragma once
#pragma once
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/mw22/mw22.hpp>

namespace fastfile::handlers::mwii {
	// used for fast changes
	constexpr const char* gamePath = "mwii";
	constexpr const char* gameExe = "cod22-cod_dump.exe";
	constexpr const char* gameDumpId = "mw2";
	constexpr const char* handlerId = "mwii";
	constexpr const char* handlerName = "Modern Warfare II";

	using namespace tool::mw22;

	typedef IW9AssetType HandlerAssetType;
	typedef IW9HashAssetType HandlerHashedAssetType;

	typedef uint64_t XHash64;
	typedef uint32_t XHash32;

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
	uint64_t GetXAssetName(HandlerHashedAssetType type, void* handle);

	class HandlerJsonWriter : public utils::raw_file_extractor::JsonWriter {
	public:
		using utils::raw_file_extractor::JsonWriter::JsonWriter;

		void WriteFieldValueXHash(const char* name, XHash64 val) {
			if (!val) return;
			JsonWriter::WriteFieldValueHash(name, val);
		}

		void WriteFieldValueXHash(uint64_t hash, XHash64 val) {
			if (!val) return;
			JsonWriter::WriteFieldValueHash(hash, val);
		}


		void WriteFieldValueXString(const char* name, XString val) {
			if (!val) return;
			JsonWriter::WriteFieldValueString(name, val);
		}

		void WriteFieldValueXString(uint64_t hash, XString val) {
			if (!val) return;
			JsonWriter::WriteFieldValueString(hash, val);
		}

		void WriteFieldValueScrString(const char* name, ScrString_t val) {
			if (!val) return;
			JsonWriter::WriteFieldValueString(name, GetScrString(val));
		}


		void WriteValueScrString(ScrString_t val) {
			if (!val) return;
			JsonWriter::WriteValueString(GetScrString(val));
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

		void WriteFieldValueXAsset(const char* name, HandlerHashedAssetType type, void* val) {
			if (!val) return;
			XHash64 hname{ GetXAssetName(type, val) };
			if (hname) {
				WriteFieldValueXHash(name, hname);
			}
		}

		void WriteFieldValueXAsset(uint64_t hash, HandlerHashedAssetType type, void* val) {
			if (!val) return;
			XHash64 hname{ GetXAssetName(type, val) };
			if (hname) {
				WriteFieldValueXHash(hash, hname);
			}
		}

		void WriteFieldValueXAssetArray(const char* name, HandlerHashedAssetType type, size_t count, void* handle, bool ignoreEmpty = true) {
			void** val{ (void**)handle };
			if (ignoreEmpty && (!count || !*val)) return;

			JsonWriter::WriteFieldNameString(name);
			JsonWriter::BeginArray();

			for (size_t i = 0; i < count; i++) {
				if (!val[i]) break;
				JsonWriter::WriteValueHash(GetXAssetName(type, val[i]));
			}
			JsonWriter::EndArray();
		}

		void WriteFieldValueXAssetArray(uint64_t hash, HandlerHashedAssetType type, size_t count, void* handle, bool ignoreEmpty = true) {
			void** val{ (void**)handle };
			if (ignoreEmpty && (!count || !*val)) return;
			JsonWriter::WriterFieldNameHash(hash);
			JsonWriter::BeginArray();

			for (size_t i = 0; i < count; i++) {
				if (!val[i]) break;
				JsonWriter::WriteValueHash(GetXAssetName(type, val[i]));
			}
			JsonWriter::EndArray();
		}

		void WriteValueLocalized(XHash64 val) {
			fastfile::FastFileOption& opt{ fastfile::GetCurrentOptions() };
			JsonWriter::WriteValueString(std::format("&#{}", utils::FormattedStringJson{ opt.GetTranslation(val) }));
		}

		void WriteFieldValueLocalized(const char* name, XHash64 val) {
			if (!val) {
				return;
			}
			JsonWriter::WriteFieldNameString(name);
			WriteValueLocalized(val);
		}
	};

	std::vector<const char*>* GetXStrings();
	std::unordered_map<HandlerHashedAssetType, Worker*>& GetWorkers();
}