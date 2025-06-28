#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>
#include <tools/ff/handlers/bo4/bo4_unlinker_scriptbundle.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct GameTypeSettingsDef {
				char* name;
				char* value;
			};
			static_assert(sizeof(GameTypeSettingsDef) == 0x10);

			struct GameTypeTableEntry {
				const char* nameStr;
				const char* unk8;
				const char* baseGameType;
				XHash name;
				XHash nameRef;
				XHash nameRefCaps;
				XHash descriptionRef;
				XHash baseGameTypeHash;
				bool isHardcoreAvailable;
				XHash hardcoreNameRef;
				bool isTeamBased;
				bool hideHudScore;
				const char* groupName;
				GfxImage* image;
				XHash presenceString;
				uint32_t unka8;
				uint32_t unkac;
				const char* scoreInfoFile;
				GameTypeSettingsDef* settings;
				int32_t settingsCount;
				scriptbundle::SB_ObjectsArray bundle;
				int32_t unkf0_count;
				byte* unkf0;
				int32_t uniqueID;
				XHash loadout;
			};
			static_assert(sizeof(GameTypeTableEntry) == 0x110);

			struct GameTypeTable {
				XHash name;
				uint32_t gameTypeCount;
				GameTypeTableEntry* gameTypes;
				eModes sessionMode;
			};
			static_assert(sizeof(GameTypeTable) == 0x28);

			GameTypeTable* asset{ (GameTypeTable*)ptr };

			if (asset->gameTypeCount == 0) return; // ignore

			const char* name{ hashutils::ExtractTmp("file", asset->name) };
			std::filesystem::path outDir{ opt.m_output / "bo4" / "source" / "tables" / "gametype" / name };
			std::filesystem::create_directories(outDir);

			LOG_INFO("Dump gametypetable {}", outDir.string());
			{
				std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "gametype" / std::format("{}.json", name) };
				BO4JsonWriter json{};
				json.BeginObject();
				json.WriteFieldValueXHash("name", asset->name);
				json.WriteFieldValueString("sessionMode", GetEModeName(asset->sessionMode));
				json.EndObject();

				if (!json.WriteToFile(outFile)) {
					LOG_ERROR("Error when dumping {}", outFile.string());
				}
			}

			for (size_t i = 0; i < asset->gameTypeCount; i++) {
				GameTypeTableEntry* entry{ asset->gameTypes + i };
				if (!entry->nameStr) continue;

				std::filesystem::path outFile{ outDir / std::format("{}.json", entry->nameStr) };
				BO4JsonWriter json{};
				json.BeginObject();
				json.WriteFieldValueNumber("uniqueID", entry->uniqueID);
				if (entry->nameStr) json.WriteFieldValueString("nameStr", entry->nameStr);
				if (entry->unk8) json.WriteFieldValueString("unk8", entry->unk8);
				if (entry->baseGameType) json.WriteFieldValueString("baseGameType", entry->baseGameType);
				if (entry->groupName) json.WriteFieldValueString("groupName", entry->groupName);
				if (entry->scoreInfoFile) json.WriteFieldValueString("scoreInfoFile", entry->scoreInfoFile);
				json.WriteFieldValueXHash("name", entry->name);
				json.WriteFieldValueXHash("nameRef", entry->nameRef);
				json.WriteFieldValueXHash("nameRefCaps", entry->nameRefCaps);
				json.WriteFieldValueXHash("descriptionRef", entry->descriptionRef);
				json.WriteFieldValueXHash("baseGameTypeHash", entry->baseGameTypeHash);
				json.WriteFieldValueXHash("hardcoreNameRef", entry->hardcoreNameRef);
				json.WriteFieldValueXHash("presenceString", entry->presenceString);
				json.WriteFieldValueXHash("loadout", entry->loadout);
				json.WriteFieldValueBool("isHardcoreAvailable", entry->isHardcoreAvailable);
				json.WriteFieldValueBool("isTeamBased", entry->isTeamBased);
				json.WriteFieldValueBool("hideHudScore", entry->hideHudScore);
				json.WriteFieldValueXAsset("image", games::bo4::pool::XAssetType::ASSET_TYPE_IMAGE, entry->image);

				if (entry->unka8) json.WriteFieldValueNumber("unka8", entry->unka8);
				if (entry->unkac) json.WriteFieldValueNumber("unkac", entry->unkac);

				if (entry->unkf0_count) {
					std::filesystem::path outFileF0{ outDir / std::format("{}.bin", entry->nameStr) };

					if (!utils::WriteFile(outFileF0, entry->unkf0, entry->unkf0_count)) {
						LOG_ERROR("Error when dumping {}", outFileF0.string());
					}
					LOG_INFO("Dump {}", outFileF0.string());
				}

				if (entry->settingsCount) {
					json.WriteFieldNameString("settings");
					json.BeginArray();
					for (size_t j = 0; j < entry->settingsCount; j++) {
						GameTypeSettingsDef* def{ entry->settings + j };

						json.BeginObject();

						if (def->name)json.WriteFieldValueString("nameStr", def->name);
						if (def->value)json.WriteFieldValueString("value", def->value);

						json.EndObject();
					}
					json.EndArray();
				}

				json.WriteFieldNameString("bundle");
				bool err{};
				scriptbundle::WriteObject(json, entry->bundle, err);
				if (err) {
					LOG_ERROR("Bundle error");
					return;
				}
				json.EndObject();

				if (!json.WriteToFile(outFile)) {
					LOG_ERROR("Error when dumping {}", outFile.string());
				}
			}
		}
	};
	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_GAMETYPETABLE };
}