#include <includes.hpp>
#include <rapidcsv.h>
#include <tools/ff/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {

	union StringTableCellValue {
		XHash hash;
		const char* string_value;
		int64_t int_value;
		float float_value;
		byte bool_value;
	};

	enum StringTableCellType : byte {
		STC_TYPE_UNDEFINED = 0,
		STC_TYPE_STRING = 1,
		STC_TYPE_HASHED2 = 2,
		STC_TYPE_INT = 4,
		STC_TYPE_FLOAT = 5,
		STC_TYPE_BOOL = 6,
		STC_TYPE_HASHED7 = 7,
		STC_TYPE_HASHED8 = 8,
	};

	struct StringTableCell {
		const char* string;
		const char* hash;
	}; static_assert(sizeof(StringTableCell) == 0x10);

	struct StringTableVar {
		StringTableCellValue value{};
		uint32_t pad10{};
		StringTableCellType type{};
	}; static_assert(sizeof(StringTableVar) == 0x18);

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


	class StringTableWorker : public LinkerWorker {
	public:
		StringTableWorker() : LinkerWorker("StringTable") {}

		void Compute(BO4LinkContext& ctx) override {
			for (const char*& stringtableName : ctx.linkCtx.zone.assets["stringtable"]) {
				std::filesystem::path rfpath{ stringtableName };
				std::filesystem::path path{ ctx.linkCtx.input / rfpath };
				utils::InFileCE is{ path };
				if (!is) {
					LOG_ERROR("Can't read {}", path.string());
					ctx.error = true;
					continue;
				}

				rapidcsv::Document doc{};

				doc.Load(is, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(','));

				ctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_STRINGTABLE, fastfile::linker::data::POINTER_NEXT);

				ctx.data.PushStream(XFILE_BLOCK_TEMP);
				StringTable table{};
				table.name.name = ctx.HashPathName(rfpath);
				table.columnCount = (int32_t)doc.GetColumnCount();
				table.rowCount = doc.GetRowCount() ? (int32_t)(doc.GetRowCount() - 1) : 0;
				table.values = (StringTableVar*)fastfile::linker::data::POINTER_NEXT;

				ctx.data.WriteData(table);

				if (table.rowCount) {
					std::unique_ptr<StringTableCellType[]> types{ std::make_unique<StringTableCellType[]>(table.columnCount) };

					// load the type header
					for (size_t i = 0; i < table.columnCount; i++) {
						const std::string type{ doc.GetCell<std::string>(i, 0) };

						switch (hash::Hash64(type)) {
						case hash::Hash64("UNDEFINED"): 
							types[i] = STC_TYPE_UNDEFINED; 
							break;
						case hash::Hash64("STRING"): 
							types[i] = STC_TYPE_STRING; 
							break;
						case hash::Hash64("HASH"):
						case hash::Hash64("HASH2"):
							types[i] = STC_TYPE_HASHED2; 
							break;
						case hash::Hash64("INT"): 
							types[i] = STC_TYPE_INT; 
							break;
						case hash::Hash64("FLOAT"): 
							types[i] = STC_TYPE_FLOAT; 
							break;
						case hash::Hash64("BOOL"): 
							types[i] = STC_TYPE_BOOL; 
							break;
						case hash::Hash64("HASH7"): 
							types[i] = STC_TYPE_HASHED7; 
							break;
						case hash::Hash64("HASH8"): 
							types[i] = STC_TYPE_HASHED8; 
							break;
						default:
							LOG_ERROR("Can't compile string table type {}", type);
							return;
						}
					}

					// load the values
					ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
					size_t offVars{ ctx.data.AllocData(sizeof(StringTableVar) * table.rowCount * table.columnCount) };
					for (size_t i = 1; i <= table.rowCount; i++) {
						for (size_t j = 0; j < table.columnCount; j++) {
							const std::string val{ doc.GetCell<std::string>(j, i) };
							StringTableVar& var{ ctx.data.GetData<StringTableVar>(offVars)[(i - 1) * table.columnCount + j] };
							var.type = types[j];

							switch (var.type) {
							case STC_TYPE_UNDEFINED:
								// nothing to do
								break;
							case STC_TYPE_STRING:
								var.value.string_value = (const char*)fastfile::linker::data::POINTER_NEXT;
								ctx.data.WriteData(val);
								break;
							case STC_TYPE_HASHED2:
							case STC_TYPE_HASHED7:
							case STC_TYPE_HASHED8:
								var.value.hash.name = ctx.HashXHash(val.data());
								break;
							case STC_TYPE_INT:
								var.value.int_value = utils::ParseFormatInt(val.data());
								break;
							case STC_TYPE_FLOAT:
								var.value.float_value = std::strtof(val.data(), nullptr);
								break;
							case STC_TYPE_BOOL:
								var.value.bool_value = utils::EqualIgnoreCase("true", val);
								break;
							default:
								throw std::runtime_error("STRINGTABLE LINKER MISSING CASE");
							}
						}
					}
					ctx.data.PopStream();
				}

				ctx.data.PopStream();

				LOG_INFO("Added asset stringtable {} (hash_{:x})", rfpath.string(), table.name.name);
			}
		}
	};

	utils::ArrayAdder<StringTableWorker, LinkerWorker> impl{ GetWorkers() };
}