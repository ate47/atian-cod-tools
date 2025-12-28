#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <hook/error.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct ArenaSeasonsUnk221 {
				uint64_t unk00;
				uint64_t unk08;
				uint64_t unk10;
				XHash unk18;
				XHash unk28;
				XHash unk38;
			};

			struct ArenaSeasonsUnk22
			{
				XHash hash;
				uint32_t unk10;
				int32_t unk18_count;
				ArenaSeasonsUnk221* unk18;
			};

			struct ArenaSeasonsUnk2
			{
				XHash hash;
				int32_t unk18_count;
				ArenaSeasonsUnk22** unk18;
			};
			struct ArenaSeasonsUnk20 {
				int64_t unk0;
				ArenaSeasonsUnk2* ptr;
			};


			struct ArenaSeasonsUnk12
			{
				uint32_t unk0;
				uint32_t unk4;
			};

			struct ArenaSeasonsUnk1
			{
				XHash hash;
				ArenaSeasonsUnk12* unk10;
				uint32_t unk10_count;
			};

			struct ArenaSeasons {
				uint64_t unk0;
				XHash name;
				uint32_t unk18;
				uint32_t unk20_count;
				ArenaSeasonsUnk20* unk20;
				ArenaSeasonsUnk1* unk28;
				uint32_t unk28_count;
			};
			static_assert(sizeof(ArenaSeasons) == 0x38);

			ArenaSeasons* asset{ (ArenaSeasons*)ptr };

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "arenaseasons" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };
			std::filesystem::create_directories(outFile.parent_path());
			BO4JsonWriter json{};

			LOG_INFO("Dump arenaseasons {}", outFile.string());

			json.BeginObject();
			json.WriteFieldValueHash("unk0", asset->unk0);
			json.WriteFieldValueNumber("unk18", asset->unk18);
			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldNameString("unk28");
			json.BeginArray();
			for (size_t i = 0; i < asset->unk28_count; i++) {
				ArenaSeasonsUnk1* sub{ asset->unk28 + i };
				json.BeginObject();
				json.WriteFieldValueXHash("name", sub->hash);
				json.WriteFieldNameString("unk10");
				for (size_t j = 0; j < sub->unk10_count; j++) {
					ArenaSeasonsUnk12* sub2{ sub->unk10 + j };
					json.BeginObject();
					json.WriteFieldValueHash("unk0", sub2->unk0);
					json.WriteFieldValueHash("unk4", sub2->unk4);
					json.EndObject();
				}
				json.EndObject();
			}
			json.EndArray();
			json.WriteFieldNameString("unk20");
			json.BeginArray();
			for (size_t i = 0; i < asset->unk20_count; i++) {
				ArenaSeasonsUnk20* sub{ asset->unk20 + i };
				json.BeginObject();
				json.WriteFieldValueHash("name", sub->unk0);
				if (sub->ptr) {
					json.WriteFieldNameString("ptr");
					json.BeginObject();
					json.WriteFieldValueXHash("name", sub->ptr->hash);
					json.WriteFieldNameString("unk28");
					json.BeginArray();
					for (size_t j = 0; j < sub->ptr->unk18_count; j++) {
						ArenaSeasonsUnk22* sub2{ sub->ptr->unk18[j] };
						if (!sub2) {
							json.WriteValueNull();
							continue;
						}
						json.BeginObject();
						json.WriteFieldValueXHash("name", sub2->hash);
						json.WriteFieldValueNumber("unk10", sub2->unk10);

						json.WriteFieldNameString("unk28");
						json.BeginArray();

						json.EndArray();

						json.EndObject();
					}

					json.EndArray();
					json.EndObject();
				}
				json.EndObject();
			}
			json.EndArray();
			json.EndObject();


			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_ARENASEASONS, true };
}