#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/fastfile/handlers/bo4/bo4_unlinker_scriptbundle.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	struct SprayOrGestureItemGestureSub {
		Gesture* gesture;
		Gesture* gesture2;
		uint32_t roles_count;
		XHash* roles;
	}; static_assert(sizeof(SprayOrGestureItemGestureSub) == 0x20);

	struct SprayOrGestureItemGesture {
		Gesture* gesture;
		Gesture* gesture2;
		uint32_t sub_count;
		SprayOrGestureItemGestureSub* sub;
	};

	struct SprayOrGestureItemTag {
		Material* material;
	};

	union SprayOrGestureItemUnion {
		SprayOrGestureItemGesture gesture;
		SprayOrGestureItemTag tag;
	}; static_assert(sizeof(SprayOrGestureItemUnion) == 0x20);

	struct SprayOrGestureItem {
		XHash name;
		XHash type; // "gesture"
		SprayOrGestureItemUnion val;
		scriptbundle::SB_ObjectsArray bundle;
	}; static_assert(sizeof(SprayOrGestureItem) == 0x60);

	struct SprayOrGestureList {
		XHash name;
		uint32_t count;
		SprayOrGestureItem** items;
	}; static_assert(sizeof(SprayOrGestureList) == 0x20);

	class SprayOrGestureWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			SprayOrGestureItem* asset{ (SprayOrGestureItem*)ptr };

			const char* n{ hashutils::ExtractTmp("file", asset->name.name) };
			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "sprayorgesture" 
				/ (asset->type ? hashutils::ExtractTmp("hash", asset->type) : "invalid")
				/ std::format("{}.json", n) };
			std::filesystem::create_directories(outFile.parent_path());

			utils::raw_file_extractor::JsonWriter json{};

			LOG_OPT_INFO("Dump sprayorgestureitem {}", outFile.string());
			json.BeginObject();

			json.WriteFieldNameString("name");
			json.WriteValueHash(asset->name);
			json.WriteFieldNameString("type");
			json.WriteValueHash(asset->type);

			switch (asset->type.name) {
			case hash::Hash64("gesture"): {
				SprayOrGestureItemGesture& gesture{ asset->val.gesture };
				if (gesture.gesture) {
					json.WriteFieldNameString("gesture");
					json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_GESTURE, gesture.gesture)->name);
				}
				if (gesture.gesture2) {
					json.WriteFieldNameString("gesture2");
					json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_GESTURE, gesture.gesture2)->name);
				}
				if (gesture.sub_count) {
					json.WriteFieldNameString("sub");
					json.BeginArray();
					for (size_t i = 0; i < gesture.sub_count; i++) {
						SprayOrGestureItemGestureSub& sub{ gesture.sub[i] };
						json.BeginObject();

						if (sub.gesture) {
							json.WriteFieldNameString("gesture");
							json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_GESTURE, sub.gesture)->name);
						}

						if (sub.gesture2) {
							json.WriteFieldNameString("gesture2");
							json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_GESTURE, sub.gesture2)->name);
						}

						if (sub.roles_count) {
							json.WriteFieldNameString("roles");
							json.BeginArray();

							for (size_t i = 0; i < sub.roles_count; i++) {
								json.WriteValueHash(sub.roles[i]);
							}

							json.EndArray();
						}

						json.EndObject();
					}
					json.EndArray();
				}

				break;
			}
			case hash::Hash64("tag"): {
				SprayOrGestureItemTag& tag{ asset->val.tag };
				if (tag.material) {
					json.WriteFieldNameString("material");
					json.WriteValueHash(games::bo4::pool::GetAssetName(games::bo4::pool::ASSET_TYPE_MATERIAL, tag.material)->name);
				}
				break;
			}
			default:
				LOG_ERROR("INVALID SPRAY OR GESTURE TYPE: {}", hashutils::ExtractTmp("hash", asset->type));
				break;
			}

			bool error{};
			json.WriteFieldNameString("bundle");
			scriptbundle::WriteObject(json, asset->bundle, error);

			if (error) return;

			json.EndObject();


			if (!json.WriteToFile(outFile) || error) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};


	class SprayOrGestureListWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			SprayOrGestureList* asset{ (SprayOrGestureList*)ptr };

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "sprayorgesture" / "list" / std::format("{}.csv", hashutils::ExtractTmp("file", asset->name.name)) };

			std::filesystem::create_directories(outFile.parent_path());


			utils::OutFileCE os{ outFile };

			if (!os) {
				LOG_ERROR("Error when dumping {}", outFile.string());

			}

			LOG_OPT_INFO("Dump sprayorgesturelist {}", outFile.string());

			os << "type,name";

			for (size_t i = 0; i < asset->count; i++) {
				os << "\n"
					<< "#" << hashutils::ExtractTmp("hash", asset->items[i]->type)
					<< ",#" << hashutils::ExtractTmp("hash", asset->items[i]->name);
			}

		}
	};

	utils::MapAdder<SprayOrGestureWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_SPRAYORGESTUREITEM };
	utils::MapAdder<SprayOrGestureListWorker, games::bo4::pool::XAssetType, Worker> impllist{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_SPRAYORGESTURELIST };
}