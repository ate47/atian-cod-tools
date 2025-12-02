#pragma once
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/bo7/bo7.hpp>

namespace fastfile::handlers::bo7 {
	using namespace tool::bo7;
	constexpr const char* gamePath = "bo7";
	constexpr const char* gameExe = "cod_dump.exe";
	constexpr const char* gameDumpId = "bo7";
	constexpr const char* handlerId = "bo7";
	constexpr const char* handlerName = "Black Ops 7";

	typedef SatAssetType HandlerAssetType;
	typedef SatHashAssetType HandlerHashedAssetType;

	struct GfxImage;
	struct MaterialAnimation;
	struct ParticleSystem;
	struct FootStepSFXTable;
	struct FoliageSFXTable;
	struct HandPlantSFXTable;
	struct AIImpactVFXTable;
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

	typedef const char* XString;

	struct ScrString_t {
		uint32_t id;

		operator uint32_t() {
			return id;
		}
	};

	class Worker {
	public:
		bool graphic;
		size_t assetSize;

		Worker(size_t assetSize, bool graphic = false) : graphic(graphic), assetSize(assetSize) {}
		virtual void GenDefaultXHashes(fastfile::FastFileContext* ctx) {}
		virtual void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) = 0;
		virtual void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) {}
		virtual void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) {}
	};

	const char* GetScrString(ScrString_t id);
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

		void WriteFieldValueScrString(uint64_t hash, ScrString_t val) {
			if (!val) return;
			JsonWriter::WriteFieldValueString(hash, GetScrString(val));
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