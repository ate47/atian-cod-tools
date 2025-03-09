#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>

namespace {
	using namespace fastfile::handlers::bo4;

	enum StringTableType : byte {
		STC_TYPE_UNDEFINED = 0x0,
		STC_TYPE_STRING = 0x1,
		STC_TYPE_HASHED2 = 0x2,
		STC_TYPE_UNK3 = 0x3,
		STC_TYPE_INT = 0x4,
		STC_TYPE_FLOAT = 0x5,
		STC_TYPE_BOOL = 0x6,
		STC_TYPE_HASHED7 = 0x7,
		STC_TYPE_HASHED8 = 0x8,
		STC_TYPE_UNK9 = 0x9,
	};


	struct StringTableVar {
		union {
			bool b;
			float f;
			int32_t i;
			const char* str;
			uint64_t u64;
			XHash hash;
		} value;
		uint32_t unk10;
		StringTableType type;
	}; static_assert(sizeof(StringTableVar) == 0x18);

	struct StringTableCell {
		const char* string;
		const char* hash;
	};

	struct StringTable {
		XHash name;
		int columnCount;
		int rowCount;
		int cellcount;
		int unk38_count;
		StringTableCell* cells;
		StringTableVar* values;
		uint16_t* unk30;
		XHash* unk38;
	};
	static_assert(sizeof(StringTable) == 0x40);

	class ScriptParseTreeWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			StringTable* asset{ (StringTable*)ptr };

			const char* n{ hashutils::ExtractPtr(asset->name.name) };

			if (!n) {
				n = utils::va("hashed/stringtable/file_%llx.csv", asset->name.name);
			}

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / n };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump {}", outFile.string());

			if (!(asset->columnCount * asset->rowCount) && std::filesystem::exists(outFile)) return; // ignore empty files

			utils::OutFileCE os{ outFile };

			if (!os) {
				LOG_ERROR("Can't dump {}", outFile.string());
				return;
			}

			if (asset->cellcount) {
				for (size_t i = 0; i < asset->cellcount; i++) {
					StringTableCell& cell{ asset->cells[i] };

					if (i) os << ",";
					os << cell.string << "/" << cell.hash;
				}
				os << "\n";
			}

			for (size_t i = 0; i < asset->rowCount; i++) {
				if (i) os << "\n";
				for (size_t j = 0; j < asset->columnCount; j++) {
					size_t idx{ i * asset->columnCount + j };
					StringTableVar& cell{ asset->values[idx] };
					if (j) os << ",";
					//os << asset->unk30[idx] << "/";
					//os << std::hex << cell.unk10 << std::dec << "/";
					switch (cell.type) {
					case STC_TYPE_UNDEFINED:
						os << "undefined";
						break;
					case STC_TYPE_STRING:
						os << cell.value.str;
						break;
					case STC_TYPE_HASHED2:
						os << "#" << hashutils::ExtractTmp("hash", cell.value.hash);
						break;
					case STC_TYPE_INT:
						os << std::dec << cell.value.i;
						break;
					case STC_TYPE_FLOAT:
						os << cell.value.f;
						break;
					case STC_TYPE_BOOL:
						os << (cell.value.b ? "TRUE" : "FALSE");
						break;
					case STC_TYPE_HASHED7:
						os << "7#" << hashutils::ExtractTmp("hash", cell.value.hash);
						break;
					case STC_TYPE_HASHED8:
						os << "8#" << hashutils::ExtractTmp("hash", cell.value.hash);
						break;
					case STC_TYPE_UNK3:
						os << "u3?" << std::dec << cell.value.u64;
						break;
					case STC_TYPE_UNK9:
						os << "u9?" << std::dec << cell.value.u64;
						break;
					default:
						os << "<badtype:" << (int)cell.type << ">#" << hashutils::ExtractTmp("hash", cell.value.hash);
						break;
					}
				}
			}
		}
	};

	utils::MapAdder<ScriptParseTreeWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_STRINGTABLE };
}