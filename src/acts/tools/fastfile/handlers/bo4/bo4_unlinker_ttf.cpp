#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct TTFKerningEntry {
				uint16_t height;
				uint16_t prevCharIndex;
				uint16_t nextCharIndex;
				int16_t kerning;
			};

			struct TTFDef {
				XHash name;
				XHash filename;
				int32_t fileLen;
				byte* file;
				void* ftFace;
				TTFDef* next;
				TTFKerningEntry* kerningCache;
				uint32_t kerningCacheCount;
			};

			static_assert(sizeof(TTFDef) == 0x50);

			TTFDef* asset{ (TTFDef*)ptr };

			std::filesystem::path outFileGraphic{ opt.m_output / "bo4" / "graphics" / "ttf" / std::format("{}.ttf", hashutils::ExtractTmp("file", asset->name)) };
			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "ttf" / std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };

			if (!asset->fileLen) {
				LOG_WARNING("Invalid TTF, ignore {}", outFile.string());
				return;
			}
			std::filesystem::create_directories(outFileGraphic.parent_path());
			std::filesystem::create_directories(outFile.parent_path());
			BO4JsonWriter json{};


			LOG_INFO("Dump ttf {}", outFile.string());
			LOG_INFO("Dump graphic ttf {}", outFileGraphic.string());

			json.BeginObject();
			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXHash("filename", asset->filename);
			if (asset->next) json.WriteFieldValueXHash("next", asset->next->name);
			if (asset->kerningCacheCount) {
				json.WriteFieldValueBool("kerningCache", true);
				if (asset->kerningCacheCount != 0x4000) {
					json.WriteFieldValueNumber("kerningCacheSize", asset->kerningCacheCount);
				}
			}
			else {
				json.WriteFieldValueBool("kerningCache", false);
			}
			//json.BeginArray();
			//for (size_t i = 0; i < asset->kerningCacheCount; i++) {
			//	TTFKerningEntry* entry{ asset->kerningCache + i };
			//	json.BeginObject();
			//	json.WriteFieldValueNumber("height", entry->height);
			//	json.WriteFieldValueNumber("prevCharIndex", entry->prevCharIndex);
			//	json.WriteFieldValueNumber("nextCharIndex", entry->nextCharIndex);
			//	json.WriteFieldValueNumber("kerning", entry->kerning);
			//	json.EndObject();
			//}
			//json.EndArray();
			json.EndObject();


			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}

			if (!utils::WriteFile(outFileGraphic, asset->file, asset->fileLen)) {
				LOG_ERROR("Error when dumping graphic {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_TTF };
}