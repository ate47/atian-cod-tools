#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo6_sp.hpp>

namespace {
	using namespace fastfile::handlers::bo6sp;
	struct AIEvent {
		XHash64 name;
		int32_t unk8;
		float unkc;
		int32_t unk10;
		uint32_t unk14;
		uint32_t unk18;
		float unk1c;
		float unk20;
		float unk24;
		uint16_t unk28;
		uint16_t unk2a;
		uint32_t unk2c;
		float unk30;
		float unk34;
		float unk38;
		uint32_t unk3c;
	};
	static_assert(sizeof(AIEvent) == 0x40);

	struct AIEventList {
		XHash64 name;
		AIEvent* events;
		int32_t count;
	};

	static_assert(sizeof(AIEventList) == 0x18);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			AIEventList* asset{ (AIEventList*)ptr };
			HandlerJsonWriter json{};

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);

			json.WriteFieldNameString("events");
			json.BeginArray();

			for (size_t i = 0; i < asset->count; i++) {
				AIEvent* ev{ &asset->events[i] };
				json.BeginObject();
				json.WriteFieldValueXHash("name", ev->name);
				
				json.WriteFieldValueNumber("unk8", ev->unk8);
				json.WriteFieldValueNumber("unkc", ev->unkc);
				json.WriteFieldValueNumber("unk10", ev->unk10);
				json.WriteFieldValueNumber("unk14", ev->unk14);
				json.WriteFieldValueNumber("unk18", ev->unk18);
				json.WriteFieldValueNumber("unk1c", ev->unk1c);
				json.WriteFieldValueNumber("unk20", ev->unk20);
				json.WriteFieldValueNumber("unk24", ev->unk24);
				json.WriteFieldValueNumber("unk28", ev->unk28);
				json.WriteFieldValueNumber("unk2a", ev->unk2a);
				json.WriteFieldValueNumber("unk2c", ev->unk2c);
				json.WriteFieldValueNumber("unk30", ev->unk30);
				json.WriteFieldValueNumber("unk34", ev->unk34);
				json.WriteFieldValueNumber("unk38", ev->unk38);
				json.WriteFieldValueNumber("unk3c", ev->unk3c);
				json.EndObject();
			}

			json.EndArray();
			json.EndObject();


			std::filesystem::path outFile{
				opt.m_output / "bo6sp" / "source" / "tables" / "ai" / "eventlist"
				/ std::format("{}.json", hashutils::ExtractTmp("file", asset->name))
			};

			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump ai_eventlist {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10HashAssetType, Worker> impl{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_AI_EVENTLIST, sizeof(AIEventList) };
}