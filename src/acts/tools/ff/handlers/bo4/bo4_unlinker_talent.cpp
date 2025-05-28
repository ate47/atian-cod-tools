#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>
#include <tools/ff/handlers/bo4/bo4_unlinker_scriptbundle.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct Talent
			{
				XHash name;
				XHash basename;
				float perks[53];
				uint32_t unkf4;
				uint32_t unkf8;
				uint32_t unkfc;
				uint32_t unk100;
				uint32_t unk104;
				uint32_t unk108;
				uint32_t unk10c;
				bool unk110;
				scriptbundle::SB_ObjectsArray bundle;
			};

			static_assert(sizeof(Talent) == 0x138);

			Talent* asset{ (Talent*)ptr };

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "talent" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };
			std::filesystem::create_directories(outFile.parent_path());
			BO4JsonWriter json{};

			LOG_INFO("Dump talent {}", outFile.string());

			json.BeginObject();
			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXHash("basename", asset->basename);

			json.WriteFieldNameString("perks");
			json.BeginArray();
			for (size_t i = 0; i < ARRAYSIZE(asset->perks); i++) {
				json.WriteValueNumber(asset->perks[i]);
			}
			json.EndArray();
			if (asset->unkf4) json.WriteFieldValueNumber("unkf4", asset->unkf4);
			if (asset->unkf8) json.WriteFieldValueNumber("unkf8", asset->unkf8);
			if (asset->unkfc) json.WriteFieldValueNumber("unkfc", asset->unkfc);
			if (asset->unk100) json.WriteFieldValueNumber("unk100", asset->unk100);
			if (asset->unk104) json.WriteFieldValueNumber("unk104", asset->unk104);
			if (asset->unk108) json.WriteFieldValueNumber("unk108", asset->unk108);
			if (asset->unk10c) json.WriteFieldValueNumber("unk10c", asset->unk10c);
			json.WriteFieldValueBool("unk110", asset->unk110);
			bool err{};
			json.WriteFieldNameString("bundle");
			scriptbundle::WriteObject(json, asset->bundle, err);
			if (err) {
				LOG_ERROR("Can't dump bundle");
				return;
			}

			json.EndObject();


			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};
	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_TALENT, true };
}