#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>

namespace {

	using namespace fastfile::handlers::bo6;
	class ImplWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			// tablelookupbyrow  26a6c8cb862c750f
			enum StringTableType : uint8_t {
				STT_UNK0 = 0x0,
				STT_STRING = 0x1,
				STT_INT64 = 0x2,
				STT_FLOAT = 0x3,
				STT_BOOL = 0x4,
				STT_XHASH = 0x5,
				STT_XHASH_RES = 0x6,
				STT_XHASH_DVAR = 0x7,
				STT_XHASH_OMNVAR = 0x8,
				STT_XHASH_32 = 0x9,
				STT_XHASH_LOCALIZED = 0xa,
				STT_UNKB = 0xb,
			};

			union StringTableValue {
				uint64_t u64;
				uint32_t u32;
				float f;
				const char* str;
				bool b;
			};

			struct StringTableOutput {
				StringTableValue output;
				uint32_t unk8;
				StringTableType type;
			};

			struct StringTableColumn {
				StringTableType type;
				uint16_t* unk8_row;
				uint16_t* offsets_row;
				uint64_t unk18;
				void* data;
			};

			struct StringTable {
				uint64_t name;
				int32_t columnCount;
				int32_t rowCount;
				int32_t cellscount;
				StringTableColumn* columns;
				uint64_t* cells;
			}; static_assert(sizeof(StringTable) == 0x28);
			StringTable* asset{ (StringTable*)ptr };

			const char* n{ hashutils::ExtractPtr(asset->name) };

			if (!n) {
				n = utils::va("hashed/stringtable/file_%llx.csv", asset->name);
			}

			std::filesystem::path outFile{ opt.m_output / "bo6" / "source" / n };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump stringtable {}", outFile.string());

			utils::OutFileCE os{ outFile };

			if (!os) {
				LOG_ERROR("Error when dumping {}", outFile.string());
				return;
			}

			//for (size_t i = 0; i < asset->cellscount; i++) {
			//	os << hashutils::ExtractTmp("hash", asset->cells[i]) << " ";
			//}


			if (asset->columnCount * asset->rowCount) {
				for (size_t i = 0; i < asset->columnCount; i++) {
					StringTableColumn* col{ asset->columns + i };

					if (i) os << ",";

					switch (col->type) {
					case STT_STRING: os << "string"; break;
					case STT_INT64: os << "int"; break;
					case STT_FLOAT: os << "float"; break;
					case STT_BOOL: os << "bool"; break;
					case STT_XHASH: os << "xhash"; break;
					case STT_XHASH_RES: os << "xhashres"; break;
					case STT_XHASH_LOCALIZED: os << "xhashlocalized"; break;
					case STT_XHASH_DVAR: os << "xhashdvar"; break;
					case STT_XHASH_32: os << "xhash32"; break;
					case STT_XHASH_OMNVAR: os << "xhashomnvar"; break;
					case STT_UNKB: os << "unkb"; break;
					default: os << "unk" << std::dec << col->type; break;
					}
				}
				for (size_t i = 0; i < asset->rowCount; i++) {
					*os << std::endl;
					for (size_t j = 0; j < asset->columnCount; j++) {
						if (j) {
							os << ",";
						}
						StringTableColumn* column = asset->columns + j;
						if (!column->data) {
							continue;
						}
						
						int16_t rowIndex = column->offsets_row[i];

						int elemSize;
						switch (column->type) {
						case STT_STRING:
						case STT_XHASH:
						case STT_XHASH_RES:
						case STT_XHASH_LOCALIZED:
						case STT_XHASH_DVAR:
						case STT_XHASH_OMNVAR:
						case STT_UNKB:
						case STT_INT64:
							elemSize = 8;
							break;
						case STT_XHASH_32:
						case STT_FLOAT:
							elemSize = 4;
							break;
						case STT_BOOL:
							elemSize = 1;
							break;
						default:
							elemSize = 0;
							break;
						}
						if (!elemSize) {
							os << "<badtype:" << std::hex << column->type << ">";
							continue; // wtf?
						}

						void* value{ &((byte*)column->data)[elemSize * rowIndex] };

						switch (column->type) {
						case STT_STRING: 
							os << *(const char**)value;
							break;
						case STT_XHASH:
							os << "#" << hashutils::ExtractTmp("hash", *(uint64_t*)value);
							break;
						case STT_XHASH_RES:
							os << "%#" << hashutils::ExtractTmp("hash", *(uint64_t*)value);
							break;
						case STT_XHASH_LOCALIZED:
							os << "r#" << hashutils::ExtractTmp("hash", *(uint64_t*)value);
							break;
						case STT_XHASH_DVAR:
							os << "@#" << hashutils::ExtractTmp("hash", *(uint64_t*)value);
							break;
						case STT_XHASH_OMNVAR:
							os << "o#" << hashutils::ExtractTmp("hash", *(uint64_t*)value);
							break;
						case STT_UNKB:
							os << "?b#" << hashutils::ExtractTmp("hash", *(uint64_t*)value);
							break;
						case STT_INT64:
							os << std::dec << *(uint64_t*)value;
							break;
						case STT_XHASH_32:
							os << "x32#" << hashutils::ExtractTmp("hash", *(uint32_t*)value);
							break;
						case STT_FLOAT:
							os << *(float*)value;
							break;
						case STT_BOOL:
							os << (*(bool*)value ? "TRUE" : "FALSE");
							break;
						default:
							os << "unk:" << column->type;
							break;
						}
					}
				}

			}

		}
	};

	utils::MapAdder<ImplWorker, bo6::T10RAssetType, Worker> impl{ GetWorkers(), bo6::T10RAssetType::T10R_ASSET_STRINGTABLE };

}