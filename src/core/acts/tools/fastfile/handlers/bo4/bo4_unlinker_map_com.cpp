#include <includes.hpp>
#include <tools/fastfile/handlers/bo4/bo4_unlinker_map.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/utils/data_utils.hpp>

namespace fastfile::handlers::bo4::map {
	using namespace games::bo4::pool;

	struct ComProbeTriggerData {
		int32_t probeID;
		int32_t volumeID;
		int32_t delayOn;
		int32_t delayOff;
		int32_t fadeIn;
		int32_t fadeOut;
		bool reversed;
	};
	static_assert(sizeof(ComProbeTriggerData) == 0x1c);

	struct ComProbeExploder {
		XHash nameHash;
		int32_t triggerCount;
		ComProbeTriggerData* triggerData;
	};
	static_assert(sizeof(ComProbeExploder) == 0x20);

	struct ComLightTriggerData {
		int32_t delayOn;
		int32_t delayOff;
		int32_t fadeIn;
		int32_t fadeOut;
		uint16_t primaryLightIndex;
		bool reversed;
	};
	static_assert(sizeof(ComLightTriggerData) == 0x14);

	struct ComLightExploder {
		XHash nameHash;
		int32_t triggerCount;
		ComLightTriggerData* triggerData;
	};
	static_assert(sizeof(ComLightExploder) == 0x20);

	struct ComPrimaryLight; // todo

	struct ComWorld {
		XHash name;
		bool isInUse;
		uint32_t primaryLightCount;
		ComPrimaryLight* primaryLights;
		uint32_t lightExploderCount;
		ComLightExploder* lightExploders;
		uint32_t sunVolumeCount;
		uint32_t* sunVolumeProbeCount;
		uint32_t probeCount;
		uint32_t* probeGuids;
		uint32_t probeExploderCount;
		ComProbeExploder* probeExploders;
	};
	static_assert(sizeof(ComWorld) == 0x60);

	class ComWorldWorker : public Worker {
		void PreLoadWorker(fastfile::FastFileContext* ctx) override {
			if (!ctx) {
				return; // no base ones
			}

			// compute the map name using the ff name
			const char* mapname{ ctx->ffname };
			std::string gamemode{ mapname };
			size_t fdd{ gamemode.find('_') };
			if (fdd != std::string::npos) {
				gamemode.resize(fdd);
			}

			const char* str{ utils::va("maps/%s/%s.d3dbsp", gamemode.c_str(), mapname) };
			hashutils::AddPrecomputed(hash::Hash64(str), str, true);
		}

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			ComWorld& asset{ *(ComWorld*)ptr };


			BO4JsonWriter json{};

			json.BeginObject();
			json.WriteFieldValueXHash("name", asset.name);
			if (asset.isInUse) json.WriteFieldValueBool("isInUse", asset.isInUse);
			json.WriteFieldValueNumber("primaryLights", asset.primaryLightCount); // todo
			if (asset.lightExploders) {
				json.WriteFieldNameString("lightExploders");
				json.BeginArray();
				for (size_t i = 0; i < asset.lightExploderCount; i++) {
					ComLightExploder* lightExploder{ &asset.lightExploders[i] };
					json.BeginObject();
					json.WriteFieldValueXHash("lightExploderName", lightExploder->nameHash);

					if (lightExploder->triggerData) {
						json.WriteFieldNameString("triggerData");
						json.BeginArray();
						for (size_t j = 0; j < lightExploder->triggerCount; j++) {
							ComLightTriggerData* data{ &lightExploder->triggerData[j] };
							json.BeginObject();

							json.WriteFieldValueNumber("delayOn", data->delayOn);
							json.WriteFieldValueNumber("delayOff", data->delayOff);
							json.WriteFieldValueNumber("fadeIn", data->fadeIn);
							json.WriteFieldValueNumber("fadeOut", data->fadeOut);
							json.WriteFieldValueNumber("primaryLightIndex", data->primaryLightIndex);
							json.WriteFieldValueBool("reversed", data->reversed);

							json.EndObject();
						}
						json.EndArray();
					}

					json.EndObject();
				}
				json.EndArray();
			}
			if (asset.probeExploders) {
				json.WriteFieldNameString("probeExploders");
				json.BeginArray();
				for (size_t i = 0; i < asset.probeExploderCount; i++) {
					ComProbeExploder* lightExploder{ &asset.probeExploders[i] };
					json.BeginObject();
					json.WriteFieldValueXHash("lightExploderName", lightExploder->nameHash);

					if (lightExploder->triggerData) {
						json.WriteFieldNameString("triggerData");
						json.BeginArray();
						for (size_t j = 0; j < lightExploder->triggerCount; j++) {
							ComProbeTriggerData* data{ &lightExploder->triggerData[j] };
							json.BeginObject();
							json.WriteFieldValueNumber("probeID", data->probeID);
							json.WriteFieldValueNumber("volumeID", data->volumeID);
							json.WriteFieldValueNumber("delayOn", data->delayOn);
							json.WriteFieldValueNumber("delayOff", data->delayOff);
							json.WriteFieldValueNumber("fadeIn", data->fadeIn);
							json.WriteFieldValueNumber("fadeOut", data->fadeOut);
							json.WriteFieldValueBool("reversed", data->reversed);
							json.EndObject();
						}
						json.EndArray();
					}

					json.EndObject();
				}
				json.EndArray();
			}
			if (asset.sunVolumeProbeCount) {
				json.WriteFieldNameString("sunVolumeProbeCount");
				json.BeginArray();
				for (size_t i = 0; i < asset.sunVolumeCount; i++) {
					json.WriteValueNumber(asset.sunVolumeProbeCount[i]);
				}
				json.EndArray();
			}
			if (asset.probeGuids) {
				json.WriteFieldNameString("probeGuids");
				json.BeginArray();
				for (size_t i = 0; i < asset.probeCount; i++) {
					json.WriteValueNumber(asset.probeGuids[i]);
				}
				json.EndArray();
			}

			json.EndObject();

			const char* name{ hashutils::ExtractPtr(asset.name) };
			if (!name) {
				name = utils::va("maps/hashed/file_%llx.d3dbsp", asset.name.name);
			}

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / name / "com_map.json" };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump com_map {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ComWorldWorker, games::bo4::pool::XAssetType, Worker> implcom{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_COMWORLD };
}