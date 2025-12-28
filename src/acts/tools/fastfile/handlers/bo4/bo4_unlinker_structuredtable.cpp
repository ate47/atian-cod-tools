#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <utils/data_utils.hpp>

namespace fastfile::handlers::bo4::scriptbundle {
	using namespace fastfile::handlers::bo4;

	enum StructuredTableCellType : __int32 {
		STRUCTURED_TABLE_CELL_TYPE_NONE = 0x0,
		STRUCTURED_TABLE_CELL_TYPE_STRING = 0x1,
		STRUCTURED_TABLE_CELL_TYPE_NUMBER = 0x2,
		STRUCTURED_TABLE_CELL_TYPE_COUNT = 0x3,
	};

	struct StructuredTableHeader {
		const char* string;
		uint32_t hash; // HashPrime
		int32_t index; // index in the headers, not sorted
	}; static_assert(sizeof(StructuredTableHeader) == 0x10);


	struct StructuredTableCell {
		union {
			int64_t number;
			const char* str;
		} value;
		StructuredTableCellType type;
		uint32_t hash; // str HashPrime or number value
	}; static_assert(sizeof(StructuredTableCell) == 0x10);


	struct StructuredTable {
		XHash name;
		int32_t cellCount;
		int32_t columnCount;
		int32_t rowCount;
		StructuredTableCell* cells;
		int32_t* cellIndex; // sorted by cell hash value
		StructuredTableHeader* headers;
		int32_t* headerIndex; // sorted by hash value
	}; static_assert(sizeof(StructuredTable) == 0x40);


	class ImplWorker : public Worker {

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {

			StructuredTable* asset{ (StructuredTable*)ptr };


			BO4JsonWriter json{};

			json.BeginArray();

			if (opt.m_header) {

				json.BeginObject();

				json.WriteFieldValueString("_comment", "header");

				json.WriteFieldValueNumber("cellCount", asset->cellCount);
				json.WriteFieldValueNumber("rowCount", asset->rowCount);
				json.WriteFieldValueNumber("columnCount", asset->columnCount);
				json.WriteFieldValueString("cellIndex", utils::data::ArrayAsString(asset->cellIndex, asset->columnCount * asset->rowCount));
				json.WriteFieldValueString("headerIndex", utils::data::ArrayAsString(asset->headerIndex, asset->columnCount));
				json.WriteFieldNameString("cellIndexA");
				json.BeginArray();
				for (size_t i = 0; i < asset->columnCount * asset->rowCount; i++) {
					json.WriteValueString(std::format("{}/{:x}", asset->cellIndex[i], asset->cells[asset->cellIndex[i]].hash));
				}
				json.EndArray();
				json.WriteFieldNameString("headerIndexA");
				json.BeginArray();
				for (size_t i = 0; i < asset->columnCount; i++) {
					json.WriteValueString(std::format("{}/{:x}", asset->headerIndex[i], asset->headers[asset->headerIndex[i]].hash));
				}
				json.EndArray();

				json.WriteFieldNameString("header");
				json.BeginObject();
				for (size_t j = 0; j < asset->columnCount; j++) {
					StructuredTableHeader* header{ &asset->headers[j] };

					json.WriteFieldValueString(header->string, std::format("{:x}/{}", header->hash, header->index));
				}

				json.EndObject();

				json.EndObject();
			}


			for (size_t i = 0; i < asset->rowCount; i++) {
				json.BeginObject();

				for (size_t j = 0; j < asset->columnCount; j++) {
					int32_t hk{ asset->headerIndex[j] };
					size_t ck{ j + i * asset->columnCount };

					StructuredTableHeader* header{ &asset->headers[j] };
					StructuredTableCell* cell{ &asset->cells[ck] };

					if (!opt.m_header && cell->type == STRUCTURED_TABLE_CELL_TYPE_NONE) continue; // unused

					json.WriteFieldNameString(header->string);
					
					switch (cell->type) {
					case STRUCTURED_TABLE_CELL_TYPE_NONE:
						json.WriteValueNull();
						break;
					case STRUCTURED_TABLE_CELL_TYPE_NUMBER:
						json.WriteValueNumber(cell->value.number);
						break;
					case STRUCTURED_TABLE_CELL_TYPE_STRING:
						json.WriteValueString(utils::FormatString(cell->value.str));
						break;
					default:
						json.WriteValueString(std::format("Invalid:{}", (int)cell->type));
						break;
					}
				}

				json.EndObject();
			}
			json.EndArray();

			const char* n{ hashutils::ExtractPtr(asset->name.name) };

			if (!n) {
				n = utils::va("hashed/structuredtable/file_%llx.json", asset->name.name);
			}

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / n };
			std::filesystem::create_directories(outFile.parent_path());

			LOG_INFO("Dump structuredtable {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_STRUCTURED_TABLE };
}