#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {

	enum StructuredTableCellType : int32_t {
		STRUCTURED_TABLE_CELL_TYPE_NONE = 0x0,
		STRUCTURED_TABLE_CELL_TYPE_STRING = 0x1,
		STRUCTURED_TABLE_CELL_TYPE_NUMBER = 0x2,
		STRUCTURED_TABLE_CELL_TYPE_COUNT = 0x3,
	};

	struct StructuredTableHeader {
		const char* string;
		uint32_t hash; // HashPrime
		int32_t index; // index in the headers
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

	class STWorker : public LinkerWorker {
	public:
		STWorker() : LinkerWorker("StructuredTable") {}

		void Compute(BO4LinkContext& ctx) override {
			for (const char*& luaFileName : ctx.linkCtx.zone.assets["structuredtable"]) {
				std::filesystem::path rfpath{ luaFileName };
				std::filesystem::path path{ ctx.linkCtx.input / rfpath };
				{
					std::string buffer{};
					if (!utils::ReadFile(path, buffer)) {
						LOG_ERROR("Can't read {}", path.string());
						ctx.error = true;
						continue;
					}

					rapidjson::Document main{};
					main.Parse(buffer.data());

					if (main.HasParseError()) {
						LOG_ERROR("Can't parse {}", path.string());
						ctx.error = true;
						continue;
					}

					if (!main.IsArray()) {
						LOG_ERROR("Can't parse {}: Structured table should be defined using a root array", path.string());
						ctx.error = true;
						continue;
					}

					ctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_STRUCTURED_TABLE, fastfile::linker::data::POINTER_NEXT);

					ctx.data.PushStream(XFILE_BLOCK_TEMP);

					size_t stoff{ ctx.data.AllocData(sizeof(StructuredTable)) };

					std::vector<StructuredTableHeader> headers{};
					std::vector<byte> headersStrings{};
					std::unordered_map<uint32_t, size_t> headersIdx{};
					size_t rowCount{};
					size_t columnCount{};

					// load header fields names
					{
						for (auto& member : main.GetArray()) {
							if (!member.IsObject()) {
								LOG_ERROR("Can't parse {}: Structured table should be defined using a root array containg objects", path.string());
								ctx.error = true;
								goto endLoop;
							}
							rowCount++;
							auto obj{ member.GetObj() };
							for (auto& [key, val] : obj) {
								const char* keystr{ key.GetString() };
								uint32_t hash{ hash::HashPrime(keystr) };
								size_t& idx{ headersIdx[hash] };
								if (!idx) {
									StructuredTableHeader& hh{ headers.emplace_back() };
									hh.index = (int32_t)columnCount++;
									idx = columnCount;
									hh.hash = hash;
									hh.string = (const char*)ALLOC_PTR;
									utils::WriteString(headersStrings, keystr);
								}
								// TODO: check collisions
							}
						}
					}
					std::vector<StructuredTableCell> cells{};
					std::vector<byte> cellsStrings{};
					for (auto& member : main.GetArray()) {
						auto obj{ member.GetObj() };
						size_t cccell{ cells.size() };
						cells.resize(cccell + columnCount);
						StructuredTableCell* row{ &cells[cccell] };
						// none type = 0
						std::memset(row, 0, sizeof(*row) * columnCount);
						for (auto& [key, val] : obj) {
							if (val.IsNull()) {
								continue; // ignore
							}
							uint32_t hash{ hash::HashPrime(key.GetString()) };
							size_t idx{ headersIdx[hash] };

							assert(idx);
							StructuredTableCell* cell{ &row[idx - 1] };
							if (val.IsInt()) {
								cell->type = STRUCTURED_TABLE_CELL_TYPE_NUMBER;
								cell->hash = (uint32_t)(cell->value.number = (int64_t)val.GetInt());
							}
							else if (val.IsString()) {
								const char* valstr{ val.GetString() };
								cell->type = STRUCTURED_TABLE_CELL_TYPE_STRING;
								cell->value.str = (const char*)ALLOC_PTR;
								cell->hash = hash::HashPrime(valstr);
								utils::WriteString(cellsStrings, valstr);
							}
							else {
								LOG_ERROR("Can't parse {}: Invalid value type for key {}", path.string(), key.GetString());
								ctx.error = true;
								goto endLoop;
							}
						}
					}

					StructuredTable& st{ *ctx.data.GetData<StructuredTable>(stoff) };
					uint64_t hash{ ctx.HashPathName(rfpath) };
					st.name.name = hash;
					size_t cellCount{ rowCount * columnCount };
					st.cellCount = (int32_t)(cellCount);
					st.rowCount = (int32_t)rowCount;
					st.columnCount = (int32_t)columnCount;
					if (cellCount) {
						st.cells = (StructuredTableCell*)ALLOC_PTR;
						st.cellIndex = (int32_t*)ALLOC_PTR;
					}
					if (columnCount) {
						st.headers = (StructuredTableHeader*)ALLOC_PTR;
						st.headerIndex = (int32_t*)ALLOC_PTR;
					}
					ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);

					if (cellCount) {
						// cells
						ctx.data.Align(8);
						ctx.data.WriteDataVector(cells);
						ctx.data.WriteDataVector(cellsStrings);

						// cell index
						ctx.data.Align<uint32_t>();
						uint32_t* cellIndex{ ctx.data.AllocDataPtr<uint32_t>(cellCount * sizeof(uint32_t)) };
						for (size_t i = 0; i < cellCount; i++) {
							cellIndex[i] = (uint32_t)i;
						}

						std::sort(cellIndex, cellIndex + cellCount, [&cells](uint32_t& idx1, uint32_t& idx2) -> bool {
							return cells[idx1].hash < cells[idx2].hash;
						});
					}

					if (columnCount) {
						// headers
						ctx.data.Align(8);
						ctx.data.WriteDataVector(headers);
						ctx.data.WriteDataVector(headersStrings);

						// header index
						ctx.data.Align<uint32_t>();
						uint32_t* headerIndex{ ctx.data.AllocDataPtr<uint32_t>(columnCount * sizeof(uint32_t)) };
						for (size_t i = 0; i < columnCount; i++) {
							headerIndex[i] = (uint32_t)i;
						}

						std::sort(headerIndex, headerIndex + columnCount, [&headers](uint32_t& idx1, uint32_t& idx2) -> bool {
							return headers[idx1].hash < headers[idx2].hash;
						});
					}

					ctx.data.PopStream();

					ctx.data.PopStream();

					LOG_INFO("Added asset structuredtable {} (hash_{:x})", rfpath.string(), hash);
				}
			endLoop:
				continue;
			}
		}
	};

	utils::ArrayAdder<STWorker, LinkerWorker> impl{ GetWorkers() };
}