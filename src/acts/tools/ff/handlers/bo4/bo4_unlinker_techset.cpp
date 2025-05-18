#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct MaterialDrawMethod {
				uint64_t unk0;
				uint64_t unk8;
				uint64_t unk10;
				uint64_t* unk18;
				uint64_t* unk20;
				uint64_t unk28;
				uint64_t unk30;
			};
			static_assert(sizeof(MaterialDrawMethod) == 0x38);

			struct MaterialTechnique {
				XHash name;
				MaterialDrawMethod drawMethods[4];
				uint32_t unkf0;
				uint16_t unkf4;
				byte unkf8_count;
				uint64_t* unkf8;
			};
			static_assert(sizeof(MaterialTechnique) == 0x100);

			struct MaterialTechniqueSet {
				XHash name;
				XHash unk10;
				uint64_t unk18;
				MaterialTechnique* techniques[16];
			};
			static_assert(sizeof(MaterialTechniqueSet) == 0xa8);

			MaterialTechniqueSet* asset{ (MaterialTechniqueSet*)ptr };

			std::filesystem::path outFile{ opt.m_output / "bo4" / "graphics" / "techset" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };
			std::filesystem::create_directories(outFile.parent_path());
			BO4JsonWriter json{};

			LOG_INFO("Dump techset {}", outFile.string());

			json.BeginObject();
			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXHash("unk10", asset->unk10);
			json.WriteFieldValueHash("unk18", asset->unk18);
			json.WriteFieldNameString("techniques");
			json.BeginArray();
			for (size_t i = 0; i < ARRAYSIZE(asset->techniques); i++) {
				MaterialTechnique* tech = asset->techniques[i];
				if (!tech) {
					json.WriteValueNull();
					continue;
				}
				json.BeginObject();
				json.WriteFieldValueHash("name", tech->name);
				json.WriteFieldValueNumber("unkf0", tech->unkf0);
				json.WriteFieldValueNumber("unkf4", tech->unkf4);
				json.WriteFieldNameString("unkf8");
				json.BeginArray();
				for (size_t j = 0; j < tech->unkf8_count; j++) {
					json.WriteValueHash(tech->unkf8[j]);
				}
				json.EndArray();
				json.EndObject();
			}
			json.EndArray();
			json.EndObject();


			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_TECHNIQUE_SET, true };
}