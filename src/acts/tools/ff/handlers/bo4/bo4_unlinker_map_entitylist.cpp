#include <includes.hpp>
#include <tools/ff/handlers/bo4/bo4_unlinker_map.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/utils/data_utils.hpp>

namespace fastfile::handlers::bo4::map {
	using namespace fastfile::handlers::bo4;
	using namespace games::bo4::pool;

	struct EntityList {
		XHash name;
		uint64_t count;
		LinkerSpawnVar* spawnVars;
	};
	static_assert(sizeof(EntityList) == 0x20);


	class EntityListWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			EntityList& asset{ *(EntityList*)ptr };


			BO4JsonWriter json{};

			json.BeginObject();
			json.WriteFieldValueXHash("name", asset.name);
			if (asset.spawnVars) {
				json.WriteFieldNameString("spawnVars");
				json.BeginArray();
				for (size_t i = 0; i < asset.count; i++) {
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

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / name / "entitylist.json" };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump entitylist {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<EntityListWorker, games::bo4::pool::XAssetType, Worker> implcom{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_ENTITY_LIST };
}