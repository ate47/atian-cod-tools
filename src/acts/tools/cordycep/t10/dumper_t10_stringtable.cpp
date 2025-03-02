#include <includes.hpp>
#include <tools/cordycep/t10/dumper_t10.hpp>

namespace {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace tool::cordycep::dump::t10;
	using namespace bo6;

	enum StringTableType : uint8_t {
		STT_UNK0 = 0x0,
		STT_STRING = 0x1,
		STT_INT64 = 0x2,
		STT_FLOAT = 0x3,
		STT_BOOL = 0x4,
		STT_XHASH = 0x5,
		STT_XHASH_RES = 0x6,
		STT_XHASH_DVAR = 0x7,
		STT_UNK8_32 = 0x8,
		STT_XHASH_LOCALIZED = 0x9,
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
		uintptr_t unk8_row; // uint16_t*
		uintptr_t offsets_row; // uint16_t*
		uint64_t unk18;
		uintptr_t data; // void*
	};

	struct StringTable {
		uint64_t name;
		int32_t columnCount;
		int32_t rowCount;
		uint64_t unk10;
		uintptr_t columns; // StringTableColumn*
	};


	class UnlinkerImpl : public Unlinker {
	public:
		UnlinkerImpl() {}

		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) {
			Process& proc{ ctx.proc };
			PoolOptionImpl& opt{ ctx.opt };
			StringTable entry{};
			if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
				LOG_ERROR("Can't read StringTable {:x}", asset.Header);
				return false;
			}

			const char* dfilename = hashutils::ExtractPtr(entry.name);

			if (!dfilename) dfilename = utils::va("hashed/scripttable/file_%llx.csv", entry.name);

			std::filesystem::path loc{ ctx.outDir / "dump" / dfilename };
			if (opt.m_ignoreOld && std::filesystem::exists(loc)) return true;
			std::filesystem::create_directories(loc.parent_path());
			utils::OutFileCE os{ loc };

			if (!os) {
				LOG_ERROR("Can't open {}", loc.string());
				return false;
			}


			if (entry.columnCount * entry.rowCount) {
				if (!entry.columns) {
					LOG_ERROR("No columns");
					return false;
				}
				auto columns{ proc.ReadMemoryArrayEx<StringTableColumn>(entry.columns, entry.columnCount) };

				for (size_t i = 0; i < entry.columnCount; i++) {
					StringTableColumn& col{ columns[i] };

					if (i) os << ",";

					switch (col.type) {
					case STT_STRING: os << "string"; break;
					case STT_INT64: os << "int"; break;
					case STT_FLOAT: os << "float"; break;
					case STT_BOOL: os << "bool"; break;
					case STT_XHASH: os << "xhash"; break;
					case STT_XHASH_RES: os << "xhashres"; break;
					case STT_XHASH_LOCALIZED: os << "xhashlocalized"; break;
					case STT_XHASH_DVAR: os << "xhashdvar"; break;
					case STT_UNK8_32: os << "unk8_32"; break;
					default: os << "unk" << std::dec << col.type; break;
					}
				}
				for (size_t i = 0; i < entry.rowCount; i++) {
					os << "\n";
					for (size_t j = 0; j < entry.columnCount; j++) {
						if (j) {
							os << ",";
						}
						StringTableColumn& column = columns[j];
						if (!column.data) {
							if (opt.m_showErr) {
								os << "<no_data>";
							}
							continue;
						}
						int16_t rowIndex = proc.ReadMemory<int16_t>(column.offsets_row + i * sizeof(int16_t));

						int elemSize;
						switch (column.type) {
						case STT_STRING:
						case STT_XHASH:
						case STT_XHASH_RES:
						case STT_XHASH_LOCALIZED:
						case STT_XHASH_DVAR:
						case STT_INT64:
							elemSize = 8;
							break;
						case STT_UNK8_32:
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
							os << "<badtype:" << std::hex << column.type << ">";
							continue; // wtf?
						}
						byte value[0x20];

						if (!proc.ReadMemory(value, column.data + elemSize * rowIndex, elemSize)) {
							LOG_ERROR("Can't read column {}x{} {:x}", i, j, column.data);
							continue;
						}
						switch (column.type) {
						case STT_STRING: {
							const char* strr{ proc.ReadStringTmp(*reinterpret_cast<uintptr_t*>(value), nullptr) };
							if (strr) {
								os << opt.AddString(strr);
							}
							else if (opt.m_showErr) {
								os << "<invalid:0x" << std::hex << *reinterpret_cast<uintptr_t*>(value) << ">";
							}
						}
									   break;
						case STT_XHASH:
							os << "#" << hashutils::ExtractTmp("hash", *reinterpret_cast<uint64_t*>(value));
							break;
						case STT_XHASH_RES:
							os << "%#" << hashutils::ExtractTmp("hash", *reinterpret_cast<uint64_t*>(value));
							break;
						case STT_XHASH_LOCALIZED:
							os << "r#" << ctx.GetLocalized(*reinterpret_cast<uint64_t*>(value));
							break;
						case STT_XHASH_DVAR:
							os << "@#" << hashutils::ExtractTmp("hash", *reinterpret_cast<uint64_t*>(value));
							break;
						case STT_INT64:
							os << std::dec << *reinterpret_cast<uint64_t*>(value);
							break;
						case STT_UNK8_32:
							os << std::dec << *reinterpret_cast<uint32_t*>(value);
							break;
						case STT_FLOAT:
							os << *reinterpret_cast<float*>(value);
							break;
						case STT_BOOL:
							os << (*value ? "TRUE" : "FALSE");
							break;
						default:
							os << "unk:" << column.type;
							break;
						}
					}
				}

			}

			return true;
		}

	};

	utils::MapAdder<UnlinkerImpl, bo6::T10RAssetType, Unlinker> impl{ GetUnlinkers(), bo6::T10R_ASSET_STRINGTABLE };
}