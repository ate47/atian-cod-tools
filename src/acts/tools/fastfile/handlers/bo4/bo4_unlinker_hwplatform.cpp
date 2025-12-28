#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct HWPlatformUnk2 {
				XHash hash;
				uint16_t unk10;
				uint16_t unk12;
				float unk14;
			};
			static_assert(sizeof(HWPlatformUnk2) == 0x18);

			struct HWPlatformUnk {
				uint64_t count;
				HWPlatformUnk2* ptr;
			};
			static_assert(sizeof(HWPlatformUnk) == 0x10);

			struct HWPlatform {
				XHash name;
				HWPlatformUnk unk10[5];
				uint32_t minWidth;
				uint32_t minHeight;
				uint32_t maxWidth;
				uint32_t maxHeight;
			};

			static_assert(sizeof(HWPlatform) == 0x70);

			HWPlatform* asset{ (HWPlatform*)ptr };

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "hwplatform" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name))};
			std::filesystem::create_directories(outFile.parent_path());
			BO4JsonWriter json{};

			LOG_INFO("Dump hwplatform {}", outFile.string());

			json.BeginObject();
			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueNumber("minWidth", asset->minWidth);
			json.WriteFieldValueNumber("minHeight", asset->minHeight);
			json.WriteFieldValueNumber("maxWidth", asset->maxWidth);
			json.WriteFieldValueNumber("maxHeight", asset->maxHeight);
			json.WriteFieldNameString("info");
			json.BeginArray();
			for (size_t i = 0; i < ARRAYSIZE(asset->unk10); i++) {
				HWPlatformUnk* sub{ asset->unk10 + i };
				json.BeginArray();
				for (size_t j = 0; j < sub->count; j++) {
					HWPlatformUnk2* sub2{ sub->ptr + j };
					json.BeginObject();
					json.WriteFieldValueXHash("name", sub2->hash);
					json.WriteFieldValueNumber("unk10", sub2->unk10);
					json.WriteFieldValueNumber("unk12", sub2->unk12);
					json.WriteFieldValueNumber("unk14", sub2->unk14);
					json.EndObject();
				}
				json.EndArray();
			}
			json.EndArray();
			json.EndObject();


			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	// doesn't seem to be used
	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_HWPLATFORM, true };
}