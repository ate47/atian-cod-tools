#include <includes.hpp>
#include <tools/fastfile/handlers/bo4/bo4_unlinker_map.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/utils/data_utils.hpp>

namespace fastfile::handlers::bo4::map {
	using namespace fastfile::handlers::bo4;
	using namespace games::bo4::pool;
	
	static const char* spawnVarKeyTypeNames[]{
		"unk_0",
		"unk_1",
		"string",
		"vector",
		"xhash",
		"float",
		"int",
	};

	const char* SpawnVarKeyTypeName(SpawnVarKeyType type) {
		return type >= ARRAYSIZE(spawnVarKeyTypeNames) ? utils::va("unk_%d", type) : spawnVarKeyTypeNames[type];
	}

	typedef uint32_t contents_t;
	struct Bounds {
		vec3_t midPoint;
		vec3_t halfSize;
	};
	static_assert(sizeof(Bounds) == 0x18);


	struct TriggerModel {
		contents_t contents;
		uint16_t hullCount;
		uint16_t firstHull;
	};
	static_assert(sizeof(TriggerModel) == 8);
	struct TriggerHull
	{
		Bounds bounds;
		contents_t contents;
		uint16_t slabCount;
		uint16_t firstSlab;
	};
	static_assert(sizeof(TriggerHull) == 0x20);

	struct TriggerSlab {
		vec3_t dir;
		float midPoint;
		float halfSize;
	};
	static_assert(sizeof(TriggerSlab) == 0x14);


	struct TriggerList {
		XHash name;
		uint32_t modelsCount;
		TriggerModel* models;
		uint32_t hullCount;
		TriggerHull* hulls;
		uint32_t slabCount;
		TriggerSlab* slabs;
		uint64_t spawnVarsCount;
		LinkerSpawnVar* spawnVars;
	};
	static_assert(sizeof(TriggerList) == 0x50);


	class TriggerListWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			TriggerList& asset{ *(TriggerList*)ptr };


			BO4JsonWriter json{};

			json.BeginObject();
			json.WriteFieldValueXHash("name", asset.name);
			if (asset.models) {
				json.WriteFieldNameString("models");
				json.BeginArray();
				for (size_t i = 0; i < asset.modelsCount; i++) {
					TriggerModel* var{ &asset.models[i] };
					json.BeginObject();
					json.WriteFieldValueNumber("contents", (int)var->contents);
					json.WriteFieldValueNumber("hullCount", var->hullCount);
					json.WriteFieldValueNumber("firstHull", var->firstHull);
					json.EndObject();
				}
				json.EndArray();
			}
			if (asset.hulls) {
				json.WriteFieldNameString("hulls");
				json.BeginArray();
				for (size_t i = 0; i < asset.hullCount; i++) {
					TriggerHull* var{ &asset.hulls[i] };
					json.BeginObject();
					json.WriteFieldValueVector("boundsMidPoint", var->bounds.midPoint);
					json.WriteFieldValueVector("boundsHullCount", var->bounds.halfSize);
					json.WriteFieldValueNumber("contents", (int)var->contents);
					json.WriteFieldValueNumber("hullCount", var->slabCount);
					json.WriteFieldValueNumber("firstHull", var->firstSlab);
					json.EndObject();
				}
				json.EndArray();
			}
			if (asset.slabs) {
				json.WriteFieldNameString("slabs");
				json.BeginArray();
				for (size_t i = 0; i < asset.slabCount; i++) {
					TriggerSlab* var{ &asset.slabs[i] };
					json.BeginObject();
					json.WriteFieldValueVector("boundsMidPoint", var->dir);
					json.WriteFieldValueNumber("hullCount", var->midPoint);
					json.WriteFieldValueNumber("firstHull", var->halfSize);
					json.EndObject();
				}
				json.EndArray();
			}
			if (asset.spawnVars) {
				json.WriteFieldNameString("spawnVars");
				json.BeginArray();
				for (size_t i = 0; i < asset.spawnVarsCount; i++) {
					LinkerSpawnVar* var{ &asset.spawnVars[i] };
					json.BeginObject();
					json.WriteFieldValueNumber("modelIndex", var->modelIndex);
					json.WriteFieldValueVector("origin", var->origin);
					json.WriteFieldValueVector("angles", var->angles);
					if (var->spawnVars) {
						json.WriteFieldNameString("spawnVars");
						json.BeginArray();
						for (size_t j = 0; j < var->numSpawnVars; j++) {
							SpawnVarKeyValue* spawnVar{ &var->spawnVars[j] };
							json.BeginObject();
							json.WriteFieldValueXString("key", spawnVar->key);
							json.WriteFieldValueString("type", SpawnVarKeyTypeName(spawnVar->type));
							switch (spawnVar->type) {
							case SVKT_STRING:
								json.WriteFieldValueXString("value", spawnVar->value.string);
								break;
							case SVKT_VECTOR:
								json.WriteFieldValueVector("value", spawnVar->value.vector);
								break;
							case SVKT_XHASH:
								json.WriteFieldValueXHash("value", spawnVar->value.hash);
								break;
							case SVKT_FLOAT:
								json.WriteFieldValueNumber("value", spawnVar->value.flt);
								break;
							case SVKT_INT:
								json.WriteFieldValueNumber("value", spawnVar->value.i32);
								break;
							default:
								json.WriteFieldValueUnknown("value", &spawnVar->value, sizeof(spawnVar->value));
								break;
							}
							if (spawnVar->unk1a) json.WriteFieldValueBool("unk1a", spawnVar->unk1a);
							json.EndObject();
						}
						json.EndArray();
					}

					json.EndObject();
				}
				json.EndArray();
			}

			json.EndObject();

			const char* name{ hashutils::ExtractPtr(asset.name) };
			if (!name) {
				name = utils::va("maps/hashed/file_%llx.d3dbsp", asset.name.name);
			}

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / name / "triggerlist.json" };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump triggerlist {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<TriggerListWorker, games::bo4::pool::XAssetType, Worker> implcom{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_TRIGGER_LIST };
}