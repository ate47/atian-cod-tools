#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <utils/data_utils.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>
#include <tools/ff/handlers/bo4/bo4_unlinker_scriptbundle.hpp>

namespace {
	using namespace fastfile::handlers::bo4;

	struct StreamKey;

	struct SndBankStreamKeys {
		StreamKey* streamKey1;
		byte* data1;
		uint32_t len1;
		StreamKey* fileKey;
		byte* fileData;
		uint32_t fileLen;
	};

	struct SndIndexEntry {
		unsigned __int16 value;
		unsigned __int16 next;
	};
	struct SndAlias {
		XHash name;
		XHash subtitle;
		XHash secondaryname;
		XHash stopAliasName;
		ScrString_t facialAnimation;
		XHash assetId;
		XHash assetFileName;
		XHash assetIdSustain;
		XHash assetFileNameSustain;
		XHash assetIdRelease;
		XHash assetFileNameRelease;
		uint64_t flags0;
		uint64_t flags1;
		XHash duck;
		XHash contextType;
		XHash contextValue;
		XHash contextType1;
		XHash contextValue1;
		XHash contextType2;
		XHash contextValue2;
		XHash contextType3;
		XHash contextValue3;
		XHash stopOnPlay;
		XHash futzPatch;
		XHash unk168;
		float reverbSend;
		float centerSend;
		float volMin;
		float volMax;
		float envelopPercentage;
		uint16_t fluxTime;
		uint16_t startDelay;
		uint16_t pitchMin;
		uint16_t pitchMax;
		uint16_t distMin;
		uint16_t distMax;
		uint16_t distReverbMax;
		uint16_t envelopMin;
		uint16_t envelopMax;
		uint16_t distanceLpfMin;
		uint16_t distanceLpfMax;
		int16_t fadeIn;
		int16_t fadeOut;
		int16_t dopplerScale;
		uint8_t minPriorityThreshold;
		uint8_t maxPriorityThreshold;
		uint8_t probability;
		uint8_t occlusionLevel;
		uint8_t minPriority;
		uint8_t maxPriority;
		uint8_t pan;
		uint8_t limitCount;
		uint8_t entityLimitCount;
		uint8_t duckGroup;
		uint8_t bus;
		uint8_t group;
		uint64_t unk178;
	};
	static_assert(sizeof(SndAlias) == 0x1c0);

	struct SndAliasList {
		XHash name;
		SndAlias* aliases;
		int32_t count;
		int32_t sequence;
		float cullDistance;
		bool spatial;
	};
	static_assert(sizeof(SndAliasList) == 0x28);

	typedef float SndFloatAlign32;

	struct SndDuck {
		char name[64];
		XHash id;
		float fadeIn;
		float fadeOut;
		float startDelay;
		float distance;
		float length;
		XHash fadeInCurve;
		XHash fadeOutCurve;
		SndFloatAlign32* attenuation;
		SndFloatAlign32* filter;
		SndFloatAlign32* unk98;
		bool updateWhilePaused;
		bool trackAmplitude;
		bool disableInSplitscreen;
		float aliasAttenuation;
		float aliasLpf;
		XHash duckAlias;
	};
	static_assert(sizeof(SndDuck) == 0xc0);

	struct SndAmbient {
		char name[64];
		XHash id;
		bool defaultRoom;
		XHash reverb;
		char reverbName[64];
		XHash nearVerb;
		char nearVerbName[64];
		XHash farVerb;
		char farVerbName[64];
		float reverbDryLevel;
		float reverbWetLevel;
		float unk230;
		float unk234;
		XHash loop;
		char duck[64];
		XHash duckId;
		XHash entityContextType0;
		XHash entityContextValue0;
		XHash entityContextType1;
		XHash entityContextValue1;
		XHash entityContextType2;
		XHash entityContextValue2;
		XHash globalContextType;
		XHash globalContextValue;
	};
	static_assert(sizeof(SndAmbient) == 0x238);

	struct SndDialogScriptIdLookup {
		XHash scriptId;
		XHash aliasId;
	};
	static_assert(sizeof(SndDialogScriptIdLookup) == 0x20);


	struct SndBank {
		const char* name;
		XHash nameHash;
		const char* zone;
		const char* gameLanguage;
		const char* soundLanguage;
		uint32_t aliasCount;
		SndAliasList* alias;
		SndIndexEntry* aliasIndex;
		uint32_t reverbCount;
		uint64_t reverbs;
		uint32_t duckCount;
		SndDuck* ducks;
		uint32_t ambientCount;
		SndAmbient* ambients;
		uint32_t scriptIdLookupCount;
		SndDialogScriptIdLookup* scriptIdLookups;
		uint32_t unk90_count;
		uint64_t unk90;
		uint32_t unka0_count;
		uint64_t unka0;
		uint32_t unkb0_count;
		uint64_t unkb0;
		uint32_t unkc0_count;
		uint64_t unkc0;
		SndBankStreamKeys streamKeysSABL;
		SndBankStreamKeys streamKeysSABS;
		bool patchZone;
	};
	static_assert(sizeof(SndBank) == 0x130);

	class ImplWorker : public Worker {

		void GenDefaultXHashes(fastfile::FastFileContext* ctx) override {
			if (!ctx) {
				return; // no base ones
			}

			const char* suffixes[]{
				"all", "bp", "ea", "en", "es", "fj", "fr", "ge", "it", "ko", "ms", "po", "ru"
			};

			for (const char* suffix : suffixes) {
				const char* str{ utils::va("%s.%s", ctx->ffname, suffix) };
				hashutils::AddPrecomputed(hash::Hash64(str), str, true);
			}
		}

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			SndBank* asset{ (SndBank*)ptr };

			if (asset->name) hashutils::Add(asset->name);
			if (asset->zone) hashutils::Add(asset->zone);
			if (asset->gameLanguage) hashutils::Add(asset->gameLanguage);
			if (asset->soundLanguage) hashutils::Add(asset->soundLanguage);

			const char* n{ hashutils::ExtractTmp("file", asset->nameHash.name) };
			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "sound" / std::format("{}.json", n) };
			std::filesystem::create_directories(outFile.parent_path());

			BO4JsonWriter json{};

			LOG_INFO("Dump sound {}", outFile.string());
			json.BeginObject();

			if (asset->name) json.WriteFieldValueString("name", asset->name);
			if (asset->zone) json.WriteFieldValueString("zone", asset->zone);
			if (asset->gameLanguage) json.WriteFieldValueString("gameLanguage", asset->gameLanguage);
			if (asset->soundLanguage) json.WriteFieldValueString("soundLanguage", asset->soundLanguage);
			json.WriteFieldValueXHash("nameHash", asset->nameHash);
			json.WriteFieldValueBool("patchZone", asset->patchZone);

			json.WriteFieldNameString("alias");
			json.BeginArray();
			for (size_t i = 0; i < asset->aliasCount; i++) {
				SndAliasList* list{ asset->alias + i };

				json.BeginObject();
				if (asset->aliasIndex) {
					json.WriteFieldValueNumber("value", asset->aliasIndex[i].value);
					json.WriteFieldValueNumber("next", asset->aliasIndex[i].next);
				}
				json.WriteFieldValueXHash("name", list->name);
				json.WriteFieldValueNumber("sequence", list->sequence);
				json.WriteFieldValueNumber("cullDistance", list->cullDistance);
				json.WriteFieldValueNumber("spatial", list->spatial);

				json.WriteFieldNameString("aliases");
				json.BeginArray();
				for (size_t j = 0; j < list->count; j++) {
					SndAlias* alias{ list->aliases + j };

					json.BeginObject();
					json.WriteFieldValueXHash("name", alias->name);
					json.WriteFieldValueXHash("subtitle", alias->subtitle);
					json.WriteFieldValueXHash("secondaryname", alias->secondaryname);
					json.WriteFieldValueXHash("stopAliasName", alias->stopAliasName);
					json.WriteFieldValueXHash("assetId", alias->assetId);
					json.WriteFieldValueXHash("assetFileName", alias->assetFileName);
					json.WriteFieldValueXHash("assetIdSustain", alias->assetIdSustain);
					json.WriteFieldValueXHash("assetFileNameSustain", alias->assetFileNameSustain);
					json.WriteFieldValueXHash("assetIdRelease", alias->assetIdRelease);
					json.WriteFieldValueXHash("assetFileNameRelease", alias->assetFileNameRelease);
					json.WriteFieldValueXHash("duck", alias->duck);
					json.WriteFieldValueXHash("contextType", alias->contextType);
					json.WriteFieldValueXHash("contextValue", alias->contextValue);
					json.WriteFieldValueXHash("contextType1", alias->contextType1);
					json.WriteFieldValueXHash("contextValue1", alias->contextValue1);
					json.WriteFieldValueXHash("contextType2", alias->contextType2);
					json.WriteFieldValueXHash("contextValue2", alias->contextValue2);
					json.WriteFieldValueXHash("contextType3", alias->contextType3);
					json.WriteFieldValueXHash("contextValue3", alias->contextValue3);
					json.WriteFieldValueXHash("stopOnPlay", alias->stopOnPlay);
					json.WriteFieldValueXHash("futzPatch", alias->futzPatch);
					json.WriteFieldValueXHash("unk168", alias->unk168);
					json.WriteFieldValueScrString("facialAnimation", alias->facialAnimation);

					json.WriteFieldValueNumber("reverbSend", alias->reverbSend); // float 
					json.WriteFieldValueNumber("centerSend", alias->centerSend); // float 
					json.WriteFieldValueNumber("volMin", alias->volMin); // float 
					json.WriteFieldValueNumber("volMax", alias->volMax); // float 
					json.WriteFieldValueNumber("envelopPercentage", alias->envelopPercentage); // float 
					json.WriteFieldValueNumber("fluxTime", alias->fluxTime); // uint16_t 
					json.WriteFieldValueNumber("startDelay", alias->startDelay); // uint16_t 
					json.WriteFieldValueNumber("pitchMin", alias->pitchMin); // uint16_t 
					json.WriteFieldValueNumber("pitchMax", alias->pitchMax); // uint16_t 
					json.WriteFieldValueNumber("distMin", alias->distMin); // uint16_t 
					json.WriteFieldValueNumber("distMax", alias->distMax); // uint16_t 
					json.WriteFieldValueNumber("distReverbMax", alias->distReverbMax); // uint16_t 
					json.WriteFieldValueNumber("envelopMin", alias->envelopMin); // uint16_t 
					json.WriteFieldValueNumber("envelopMax", alias->envelopMax); // uint16_t 
					json.WriteFieldValueNumber("distanceLpfMin", alias->distanceLpfMin); // uint16_t 
					json.WriteFieldValueNumber("distanceLpfMax", alias->distanceLpfMax); // uint16_t 
					json.WriteFieldValueNumber("fadeIn", alias->fadeIn); // int16_t 
					json.WriteFieldValueNumber("fadeOut", alias->fadeOut); // int16_t 
					json.WriteFieldValueNumber("dopplerScale", alias->dopplerScale); // int16_t 
					json.WriteFieldValueNumber<int>("minPriorityThreshold", alias->minPriorityThreshold); // uint8_t 
					json.WriteFieldValueNumber<int>("maxPriorityThreshold", alias->maxPriorityThreshold); // uint8_t 
					json.WriteFieldValueNumber<int>("probability", alias->probability); // uint8_t 
					json.WriteFieldValueNumber<int>("occlusionLevel", alias->occlusionLevel); // uint8_t 
					json.WriteFieldValueNumber<int>("minPriority", alias->minPriority); // uint8_t 
					json.WriteFieldValueNumber<int>("maxPriority", alias->maxPriority); // uint8_t 
					json.WriteFieldValueNumber<int>("pan", alias->pan); // uint8_t 
					json.WriteFieldValueNumber<int>("limitCount", alias->limitCount); // uint8_t 
					json.WriteFieldValueNumber<int>("entityLimitCount", alias->entityLimitCount); // uint8_t 
					json.WriteFieldValueNumber<int>("duckGroup", alias->duckGroup); // uint8_t 
					json.WriteFieldValueNumber<int>("bus", alias->bus); // uint8_t 
					json.WriteFieldValueNumber<int>("group", alias->group); // uint8_t 
					json.WriteFieldValueHash("unk178", alias->unk178); 


					json.EndObject();
				}
				json.EndArray();

				json.EndObject();
			}
			json.EndArray();

			json.WriteFieldNameString("duck");
			json.BeginArray();
			for (size_t i = 0; i < asset->duckCount; i++) {
				SndDuck* duck{ asset->ducks + i };

				hashutils::Add(duck->name);

				json.BeginObject();
				json.WriteFieldValueString("name", duck->name);
				json.WriteFieldValueXHash("id", duck->id);
				json.WriteFieldValueXHash("fadeInCurve", duck->fadeInCurve);
				json.WriteFieldValueXHash("fadeOutCurve", duck->fadeOutCurve);
				json.WriteFieldValueXHash("duckAlias", duck->duckAlias);
				json.WriteFieldValueBool("updateWhilePaused", duck->updateWhilePaused);
				json.WriteFieldValueBool("trackAmplitude", duck->trackAmplitude);
				json.WriteFieldValueBool("disableInSplitscreen", duck->disableInSplitscreen);
				json.WriteFieldValueNumber("fadeIn", duck->fadeIn);
				json.WriteFieldValueNumber("fadeOut", duck->fadeOut);
				json.WriteFieldValueNumber("startDelay", duck->startDelay);
				json.WriteFieldValueNumber("distance", duck->distance);
				json.WriteFieldValueNumber("length", duck->length);
				json.WriteFieldValueNumber("aliasAttenuation", duck->aliasAttenuation);
				json.WriteFieldValueNumber("aliasLpf", duck->aliasLpf);

				if (duck->attenuation) json.WriteFieldValueString("attenuation", utils::data::ArrayAsString(duck->attenuation, 64, " ", "", ""));
				if (duck->filter) json.WriteFieldValueString("filter", utils::data::ArrayAsString(duck->filter, 64, " ", "", ""));
				if (duck->unk98) json.WriteFieldValueString("unk98", utils::data::ArrayAsString(duck->unk98, 64, " ", "", ""));
				
				json.EndObject();
			}
			json.EndArray();

			json.WriteFieldNameString("ambients");
			json.BeginArray();
			for (size_t i = 0; i < asset->ambientCount; i++) {
				SndAmbient* ambient{ asset->ambients + i };

				if (*ambient->name) hashutils::Add(ambient->name);
				if (*ambient->reverbName) hashutils::Add(ambient->reverbName);
				if (*ambient->nearVerbName) hashutils::Add(ambient->nearVerbName);
				if (*ambient->farVerbName) hashutils::Add(ambient->farVerbName);
				if (*ambient->duck) hashutils::Add(ambient->duck);

				json.BeginObject();
				json.WriteFieldValueString("name", ambient->name);
				json.WriteFieldValueString("reverbName", ambient->reverbName);
				json.WriteFieldValueString("nearVerbName", ambient->nearVerbName);
				json.WriteFieldValueString("farVerbName", ambient->farVerbName);
				json.WriteFieldValueString("duck", ambient->duck);
				json.WriteFieldValueXHash("id", ambient->id);
				json.WriteFieldValueXHash("reverb", ambient->reverb);
				json.WriteFieldValueXHash("nearVerb", ambient->nearVerb);
				json.WriteFieldValueXHash("farVerb", ambient->farVerb);
				json.WriteFieldValueXHash("loop", ambient->loop);
				json.WriteFieldValueXHash("duckId", ambient->duckId);
				json.WriteFieldValueXHash("entityContextType0", ambient->entityContextType0);
				json.WriteFieldValueXHash("entityContextValue0", ambient->entityContextValue0);
				json.WriteFieldValueXHash("entityContextType1", ambient->entityContextType1);
				json.WriteFieldValueXHash("entityContextValue1", ambient->entityContextValue1);
				json.WriteFieldValueXHash("entityContextType2", ambient->entityContextType2);
				json.WriteFieldValueXHash("entityContextValue2", ambient->entityContextValue2);
				json.WriteFieldValueXHash("globalContextType", ambient->globalContextType);
				json.WriteFieldValueXHash("globalContextValue", ambient->globalContextValue);

				json.WriteFieldValueBool("defaultRoom", ambient->defaultRoom);
				json.WriteFieldValueNumber("reverbDryLevel", ambient->reverbDryLevel);
				json.WriteFieldValueNumber("reverbWetLevel", ambient->reverbWetLevel);
				json.WriteFieldValueNumber("unk230", ambient->unk230);
				json.WriteFieldValueNumber("unk234", ambient->unk234);

				json.EndObject();
			}
			json.EndArray();

			json.WriteFieldNameString("scriptIdLookups");
			json.BeginArray();
			for (size_t i = 0; i < asset->scriptIdLookupCount; i++) {
				SndDialogScriptIdLookup* scriptIdLookup{ asset->scriptIdLookups + i };

				json.BeginObject();
				json.WriteFieldValueXHash("scriptId", scriptIdLookup->scriptId);
				json.WriteFieldValueXHash("aliasId", scriptIdLookup->aliasId);
				json.EndObject();
			}
			json.EndArray();

			auto WriteSndStream = [&json](const char* name, SndBankStreamKeys* bnk) {
				json.WriteFieldNameString(name);
				json.BeginObject();
				json.WriteFieldValueXAsset("streamKey1", games::bo4::pool::ASSET_TYPE_STREAMKEY, bnk->streamKey1);
				json.WriteFieldValueNumber("dataSize1", bnk->len1);
				json.WriteFieldValueXAsset("fileKey", games::bo4::pool::ASSET_TYPE_STREAMKEY, bnk->fileKey);
				json.WriteFieldValueNumber("fileLen", bnk->fileLen);
				json.EndObject();
			};

			WriteSndStream("streamKeysSABL", &asset->streamKeysSABL);
			WriteSndStream("streamKeysSABS", &asset->streamKeysSABS);

			json.EndObject();


			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_SOUND };
}