#include <includes.hpp>
#include "pools.hpp"
#include "bo3.hpp"
#include "tools/gsc.hpp"

static const char* t7names[]{
	"physpreset",
	"physconstraints",
	"destructibledef",
	"xanim",
	"xmodel",
	"xmodelmesh",
	"material",
	"computeshaderset",
	"techset",
	"image",
	"sound",
	"sound_patch",
	"col_map",
	"com_map",
	"game_map",
	"map_ents",
	"gfx_map",
	"lightdef",
	"lensflaredef",
	"ui_map",
	"font",
	"fonticon",
	"localize",
	"weapon",
	"weapondef",
	"weaponvariant",
	"weaponfull",
	"cgmediatable",
	"playersoundstable",
	"playerfxtable",
	"sharedweaponsounds",
	"attachment",
	"attachmentunique",
	"weaponcamo",
	"customizationtable",
	"customizationtable_feimages",
	"customizationtablecolor",
	"snddriverglobals",
	"fx",
	"tagfx",
	"klf",
	"impactsfxtable",
	"impactsoundstable",
	"player_character",
	"aitype",
	"character",
	"xmodelalias",
	"rawfile",
	"stringtable",
	"structuredtable",
	"leaderboarddef",
	"ddl",
	"glasses",
	"texturelist",
	"scriptparsetree",
	"keyvaluepairs",
	"vehicle",
	"addon_map_ents",
	"tracer",
	"slug",
	"surfacefxtable",
	"surfacesounddef",
	"footsteptable",
	"entityfximpacts",
	"entitysoundimpacts",
	"zbarrier",
	"vehiclefxdef",
	"vehiclesounddef",
	"typeinfo",
	"scriptbundle",
	"scriptbundlelist",
	"rumble",
	"bulletpenetration",
	"locdmgtable",
	"aimtable",
	"animselectortable",
	"animmappingtable",
	"animstatemachine",
	"behaviortree",
	"behaviorstatemachine",
	"ttf",
	"sanim",
	"lightdescription",
	"shellshock",
	"xcam",
	"bgcache",
	"texturecombo",
	"flametable",
	"bitfield",
	"attachmentcosmeticvariant",
	"maptable",
	"maptableloadingimages",
	"medal",
	"medaltable",
	"objective",
	"objectivelist",
	"umbra_tome",
	"navmesh",
	"navvolume",
	"binaryhtml",
	"laser",
	"beam",
	"streamerhint",
	"string",
};


using namespace bo3::pool;

T7XAssetType bo3::pool::T7XAssetIdFromName(const char* name) {
	for (size_t i = 0; i < ARRAYSIZE(t7names); i++) {
		if (!_strcmpi(t7names[i], name)) {
			return (T7XAssetType)i;
		}
	}
	return T7_ASSET_TYPE_COUNT;
}

const char* bo3::pool::T7XAssetName(T7XAssetType type) {
	return type >= T7_ASSET_TYPE_COUNT || type < 0 ? "unknown" : t7names[type];
}
namespace {

	struct objFileInfo_t {
		uintptr_t activeVersion; // GSC_OBJ* 
		uintptr_t unk8; // GSC_OBJ* 
		uintptr_t name; // const char* 
		uintptr_t cseg_start; // byte* 
		uintptr_t cseg_end; // byte* 
		uint64_t unk28;
		int32_t unk30;
		uint32_t unk34;
		uint64_t unk38;
		uint32_t unk40;
		uint32_t unk44;
		uint64_t unk48;
	};



	enum PoolDumpOptionFlags {
		PDOF_DUMP_ALL_RAW = 0x4,
		PDOF_IGNORE_EXIST = 0x8,
	};
	class PoolOption {
	public:
		bool m_help{};
		bool m_any_type{};
		bool m_dump_info{};
		bool m_dump_all_available{};
		bool m_dump_names{};
		const char* m_output = "pool/t7";
		std::vector<bool> m_dump_types{};
		uint64_t flags{};

		bool Compute(const char** args, INT startIndex, INT endIndex) {
			m_dump_types.clear();
			m_dump_types.reserve(T7_ASSET_TYPE_COUNT);
			for (size_t i = 0; i < T7_ASSET_TYPE_COUNT; i++) {
				m_dump_types.push_back(false);
			}
			// default values
			for (size_t i = startIndex; i < endIndex; i++) {
				const char* arg = args[i];

				if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
					m_help = true;
				}
				else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					m_output = args[++i];
				}
				else if (!_strcmpi("--info", arg) || !strcmp("-i", arg)) {
					m_dump_info = true;
				}
				else if (!_strcmpi("--all", arg) || !strcmp("-a", arg)) {
					m_dump_all_available = true;
				}
				else if (!_strcmpi("--names", arg) || !strcmp("-n", arg)) {
					m_dump_names = true;
				}
				else if (!strcmp("-f", arg) || !_strcmpi("--flag", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					auto flagName = args[++i];

					if (!_strcmpi("allraw", flagName)) {
						flags |= PDOF_DUMP_ALL_RAW;
					}
					else if (!_strcmpi("ignoreexist", flagName)) {
						flags |= PDOF_IGNORE_EXIST;
					}
					else {
						std::cerr << "Invalid flag for -" << arg << ": " << flagName << "\n";
						return false;
					}
				}
				else if (*arg == '-') {
					std::cerr << "Invalid argurment: " << arg << "!\n";
					return false;
				}
				else {
					T7XAssetType assetType = T7XAssetIdFromName(arg);

					if (assetType == T7_ASSET_TYPE_COUNT) {
						try {
							assetType = (T7XAssetType)std::strtol(arg, nullptr, 10);
						}
						catch (const std::invalid_argument& e) {
							std::cerr << e.what() << "\n";
							assetType = T7_ASSET_TYPE_COUNT;
						}
						if (assetType || assetType >= T7_ASSET_TYPE_COUNT) {
							std::cerr << "Invalid pool name: " << arg << "!\n";
							return false;
						}
					}

					m_dump_types[assetType] = true;
					m_any_type = true;
				}
			}
			return true;
		}
		void PrintHelp(std::ostream& out) {
			out << "-h --help            : Print help\n"
				<< "-i --info            : Dump pool info\n"
				<< "-n --names           : Dump names\n"
				<< "-a --all             : Dump all available pools\n"
				<< "-o --output [d]      : Output dir\n"
				<< "-f --flag [f]        : Add flag\n"
				;
		}

		bool HasFlag(uint64_t f) {
			return (flags & f) == f;
		}
	};

	const char* GetMTString(Process& proc, int strref) {
		// mt_buffer = off_50DC2D0
		uintptr_t mt_buffer{ proc.ReadMemory<uintptr_t>(proc[0x50DC2D0]) };
		return proc.ReadStringTmp(mt_buffer + (0x1C * strref + 4)); // skip lock
	}
	std::ostream& PrintFormattedString(std::ostream& out, const char* str) {
		if (!str) {
			return out << "nullptr";
		}
		for (; *str; str++) {
			switch (*str) {
			case '\n':
				out << "\\n";
				break;
			case '\r':
				out << "\\r";
				break;
			case '\t':
				out << "\\t";
				break;
			case '\a':
				out << "\\a";
				break;
			case '\b':
				out << "\\b";
				break;
			case '\v':
				out << "\\v";
				break;
			case '"':
				out << "\\\"";
				break;
			default:
				if (*str < 0x20 || *str >= 0x7F) {
					out << "\\" << std::oct << (unsigned int)(*reinterpret_cast<const byte*>(str)) << std::dec;
				}
				else {
					out << *str;
				}
				break;
			}
		}
		return out;
	}

	enum scriptBundleKVPType_t : uint32_t {
		KVP_STRING = 0x0,
		KVP_INT = 0x1,
		KVP_FLOAT = 0x2,
		KVP_ANIMATION = 0x3,
		KVP_PLAYER_ANIMATION = 0x4,
		KVP_SIEGE_ANIMATION = 0x5,
		KVP_MODEL = 0x6,
		KVP_AITYPE = 0x7,
		KVP_CHARACTER = 0x8,
		KVP_FX = 0x9,
		KVP_FX_SURFACE_TABLE = 0xA,
		KVP_RUMBLE = 0xB,
		KVP_SCRIPT_BUNDLE = 0xC,
		KVP_CAM_ANIM = 0xD,
		KVP_MATERIAL = 0xE,
		KVP_IMAGE = 0xF,
		KVP_LOC_STRING = 0x10,
		KVP_WEAPON = 0x11,
		KVP_VEHICLE = 0x12,
		KVP_ENUM = 0x13,
		KVP_STREAMERHINT = 0x14,
		KVP_COUNT = 0x15,
	};
	struct ScriptBundleKVP {
		uint32_t key; // ScrString_t
		uint32_t value; // ScrString_t
		scriptBundleKVPType_t type;
		uint32_t data;
		uintptr_t itemPtr;
	};

	struct SB_Object {
		uint32_t kvpCount;
		uintptr_t kvp; // ScriptBundleKVP*
	};

	struct SB_ObjectsArray {
		uint32_t sbObjectCount;
		uintptr_t sbObjects; // SB_Object*
	};

	struct ScriptBundle {
		uintptr_t name; // const char*
		uint32_t type; // ScrString_t
		uint32_t bundleType; // ScriptBundleType 
		SB_Object kvp;
		SB_ObjectsArray sbObjectsArray;
	};
	static_assert(sizeof(ScriptBundle) == 0x30);

	bool DumpSBObject(Process& proc, std::ostream& out, PoolOption& opt, const SB_Object& obj, int depth) {
		if (!obj.kvpCount) return true;

		auto [kvps, ok] = proc.ReadMemoryArray<ScriptBundleKVP>(obj.kvp, obj.kvpCount);

		if (!ok) {
			LOG_ERROR("Can't read kvp");
			return false;
		}

		for (size_t i = 0; i < obj.kvpCount; i++) {
			if (i) out << ",";
			out << "\n";
			ScriptBundleKVP& kvp{ kvps[i] };

			utils::Padding(out, depth) << "\"" << GetMTString(proc, kvp.key) << "\"" << ": ";

			switch (kvp.type) {
			case KVP_INT:
			case KVP_VEHICLE:
				out << std::dec << kvp.data;
				break;
			case KVP_FLOAT:
				out << *reinterpret_cast<float*>(&kvp.data);
				break;
			case KVP_FX:
				out << "\"#fx:" << GetMTString(proc, kvp.value) << "\"";
				break;
			case KVP_WEAPON:
				out << "\"#weapon:" << GetMTString(proc, kvp.value) << "\"";
				break;
			default:
				out << "\"" << GetMTString(proc, kvp.value) << "\"";
				break;
			}
		}

		return true;
	}

	bool IsNameValid(const char* name) {
		const char* _name{ name };
		if (!_name) return false;
		while (*_name) {
			if (*_name < 0x28 || *_name >= 0x7B) {
				LOG_TRACE("Invalid name: {}", name);
				return false;
			}
			_name++;
		}
		return true;
	}

}
int bo3::pool::t7dp(Process& proc, int argc, const char* argv[]) {
		PoolOption opt;

		if (!opt.Compute(argv, 2, argc) || opt.m_help) {
			opt.PrintHelp(std::cout);
			return tool::OK;
		}
		if (opt.m_dump_names) {
			const uintptr_t spt = proc[xassetpools];
			auto [pool, ok] = proc.ReadMemoryArray<T7XAssetPool>(spt, bo3::pool::T7_ASSET_TYPE_COUNT);

			if (!ok) {
				LOG_ERROR("Can't read pools: {}", spt);
				return tool::BASIC_ERROR;
			}

			LOG_INFO("id,name,alloc,count,size");
			for (size_t i = 0; i < T7_ASSET_TYPE_COUNT; i++) {
				T7XAssetPool& p{ pool[i] };
				LOG_INFO("{},{},0x{:x},0x{:x},0x{:x}", i, T7XAssetName((T7XAssetType)i), p.itemAllocCount, p.itemCount, p.itemSize);
			}

			return tool::OK;
		}

		std::error_code ec;
		std::filesystem::create_directories(opt.m_output, ec);

		char outputName[256];

		if (!opt.m_any_type && !opt.m_dump_all_available) {
			opt.PrintHelp(std::cout);
			return tool::OK;
		}
		hashutils::ReadDefaultFile();

		std::unordered_set<std::string> strings{};
		T7XAssetPool entry{};
		auto ShouldHandle = [&proc, &opt, &outputName, &entry](T7XAssetType id, bool isKnown = true) {
			if (!opt.m_dump_types[id] && !(isKnown && opt.m_dump_all_available)) {
				return false;
			}
			// set to false for the default loop
			opt.m_dump_types[id] = false;

			std::cout << "pool: " << std::dec << T7XAssetName(id) << " (" << (int)id << ")\n";

			if (!proc.ReadMemory(&entry, proc[xassetpools] + sizeof(entry) * id, sizeof(entry))) {
				std::cerr << "Can't read pool entry\n";
				return false;
			}

			sprintf_s(outputName, "%s/pool_%s", opt.m_output, T7XAssetName(id));

			std::cout << std::hex
				<< "pool ........ " << entry.pool << "\n"
				<< "free head ... " << entry.freeHead << "\n"
				<< "item size ... " << entry.itemSize << "\n"
				<< "count ....... " << entry.itemCount << "\n"
				<< "alloc count . " << entry.itemAllocCount << "\n"
				<< "singleton ... " << (entry.isSingleton ? "true" : "false") << "\n"
				;

			return true;
		};

		size_t readFile{};
		if (ShouldHandle(T7_ASSET_TYPE_RAWFILE)) {
			struct RawFileEntry {
				uintptr_t name;
				uint64_t size;
				uintptr_t buffer;
			};
			auto pool = std::make_unique<RawFileEntry[]>(entry.itemCount);

			if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemCount)) {
				std::cerr << "Can't read pool data\n";
				return tool::BASIC_ERROR;
			}
			char dumpbuff[MAX_PATH + 10];
			std::vector<byte> read{};

			for (size_t i = 0; i < entry.itemCount; i++) {
				const auto& p = pool[i];

				auto n = proc.ReadStringTmp(p.name, nullptr);

				if (n) {
					sprintf_s(dumpbuff, "%s/%s", opt.m_output, n);
				}
				else {
					sprintf_s(dumpbuff, "%s/rf_%llx.raw", opt.m_output, i);
				}

				if (!IsNameValid(n) || !p.buffer || !proc.ReadMemory<uint64_t>(p.buffer)) {
					//std::cerr << "error when reading buffer at " << p.buffer << "\n";
					continue;
				}

				std::filesystem::path file{ dumpbuff };
				bool exist{ std::filesystem::exists(file, ec) };
				if (exist && opt.HasFlag(PDOF_IGNORE_EXIST)) {
					continue;
				}

				if (file.has_extension() && !opt.HasFlag(PDOF_DUMP_ALL_RAW)) {
					std::filesystem::path filext{ file.extension() };
					std::string filexts{ filext.string() };

					if (filexts == ".lua" || filexts == ".hkt" || filexts == ".atr" || filexts.starts_with(".ddldata")) {
						//std::cerr << "ignored " << file.string() << ", use - f allraw to dump all the files\n";
						continue; // TODO: decompile atr/ddldata files?
					}
				}

				std::cout << std::dec << i << ": ";
				std::cout << dumpbuff;

				std::filesystem::create_directories(file.parent_path(), ec);

				std::cout << "->" << file;

				if (!exist) {
					readFile++;
					std::cout << " (new)";
				}
				std::cout << "\n";

				const void* output;
				size_t outputsize;

				// empty file
				if (!p.size) {
					output = "";
					outputsize = 0;
				}
				else {
					read.resize(p.size);
					std::fill(read.begin(), read.end(), 0);

					if (!proc.ReadMemory(&read[0], p.buffer, p.size)) {
						std::cerr << "error when reading buffer at " << p.buffer << "\n";
						continue;
					}
					output = &read[0];
					outputsize = p.size;
				}


				if (!utils::WriteFile(file, output, outputsize)) {
					std::cerr << "error when writting file " << file << "\n";
				}

			}
		}
		if (ShouldHandle(T7_ASSET_TYPE_SCRIPTBUNDLE)) {

			auto pool = std::make_unique<ScriptBundle[]>(entry.itemCount);

			if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemCount)) {
				std::cerr << "Can't read pool data\n";
				return tool::BASIC_ERROR;
			}
			char dumpbuff[MAX_PATH + 10];
			std::vector<byte> read{};

			for (size_t i = 0; i < entry.itemCount; i++) {
				const auto& p = pool[i];

				auto n = proc.ReadStringTmp(p.name, nullptr);
				auto type = GetMTString(proc, p.type);

				if (!p.kvp.kvpCount || !IsNameValid(n) || *n < 0x20 || *n > 0x7F) {
					continue;
				}
				if (!*type) {
					type = "default";
				}
		
				sprintf_s(dumpbuff, "%s/scriptbundle/%s/%s.json", opt.m_output, type, n);

				std::filesystem::path file{ dumpbuff };
				bool exist{ std::filesystem::exists(file, ec) };
				if (exist && opt.HasFlag(PDOF_IGNORE_EXIST)) {
					continue;
				}

				std::cout << std::dec << i << ": ";

				std::cout << dumpbuff;

				std::filesystem::create_directories(file.parent_path(), ec);

				std::cout << "->" << file;

				bool readed{};
				if (!exist) {
					readed = true;
					std::cout << " (new)";
				}
				std::cout << "\n";

				std::ofstream out{ file };

				if (!out) {
					std::cerr << "Can't create " << file.string() << "\n";
					continue;
				}

				out << "{";
				if (p.kvp.kvpCount) {
					if (!DumpSBObject(proc, out, opt, p.kvp, 1)) {
						out << "Error when dumping\n";
					}
				}
				else {
					// default value
					out << "\n";
					utils::Padding(out, 1) << "\"name\": \"" << n << "\",\n";
					utils::Padding(out, 1) << "\"type\": \"" << type << "\"";
				}
				if (p.sbObjectsArray.sbObjectCount) {
					out << ",\n";
					utils::Padding(out, 1) << "\"objects\": [";

					auto [objs, oksb] = proc.ReadMemoryArray<SB_Object>(p.sbObjectsArray.sbObjects, p.sbObjectsArray.sbObjectCount);

					if (!oksb) {
						out << "Can't read objects";
						continue;
					}

					for (size_t j = 0; j < p.sbObjectsArray.sbObjectCount; j++) {
						SB_Object& obj{ objs[j] };
						if (j) out << ",";
						utils::Padding(out << "\n", 2) << "{";
						if (!DumpSBObject(proc, out, opt, obj, 3)) {
							out << "Error when dumping\n";
						}
						utils::Padding(out << "\n", 2) << "}";
					}

					utils::Padding(out << "\n", 1) << "]";
				}
				out << "\n}";

				if (readed) readFile++;
				out.close();
			}

		}
		if (ShouldHandle(T7_ASSET_TYPE_STRINGTABLE)) {
			struct StringTable {
				uintptr_t name; // const char*
				int32_t columnCount;
				int32_t rowCount;
				uintptr_t values; // StringTableCell*
				uintptr_t cellIndex; // __int16*
			};
			struct StringTableCell {
				uintptr_t string; // const char*
				int32_t hash;
			};

			auto pool = std::make_unique<StringTable[]>(entry.itemCount);

			if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemCount)) {
				std::cerr << "Can't read pool data\n";
				return tool::BASIC_ERROR;
			}
			char dumpbuff[MAX_PATH + 10];
			std::vector<byte> read{};

			for (size_t i = 0; i < entry.itemCount; i++) {
				const auto& p = pool[i];

				auto n = proc.ReadStringTmp(p.name, nullptr);

				if (IsNameValid(n) && (p.columnCount * p.rowCount)) {
					sprintf_s(dumpbuff, "%s/%s", opt.m_output, n);
				}
				else {
					continue;
				}

				std::filesystem::path file{ dumpbuff };

				bool exist{ std::filesystem::exists(file, ec) };
				if (exist && opt.HasFlag(PDOF_IGNORE_EXIST)) {
					continue;
				}

				std::cout << std::dec << i << ": ";

				auto [cells, okp] = proc.ReadMemoryArray<StringTableCell>(p.values, p.columnCount * p.rowCount);

				if (!okp) {
					std::cerr << "Can't read " << n << "\n";
					continue;
				}

				std::cout << dumpbuff;

				std::filesystem::create_directories(file.parent_path(), ec);

				std::cout << "->" << file;

				if (!exist) {
					readFile++;
					std::cout << " (new)";
				}
				std::cout << "\n";


				std::ofstream out{ file };

				if (!out) {
					std::cerr << "Can't create " << file.string() << "\n";
					continue;
				}

				for (size_t i = 0; i < p.rowCount; i++) {
					if (i) out << "\n";

					for (size_t j = 0; j < p.columnCount; j++) {
						StringTableCell& cell{ cells[i * p.columnCount + j] };
						if (j) out << ",";
						out << proc.ReadStringTmp(cell.string);
					}
				}

				out.close();

			}
		}
		if (ShouldHandle(T7_ASSET_TYPE_STRUCTUREDTABLE)) {
			struct StructuredTable {
				uintptr_t name; // const char*
				int32_t cellCount;
				int32_t columnCount;
				int32_t rowCount;
				uintptr_t cells; // StructuredTableCell*
				uintptr_t cellIndex; // int*
				uintptr_t headers; // StructuredTableHeader*
				uintptr_t headerIndex; // int* 
			};
			struct StructuredTableHeader {
				uintptr_t string; // const char*
				int32_t hash;
				int32_t index;
			};
			enum StructuredTableCellType : int32_t {
				STRUCTURED_TABLE_CELL_TYPE_NONE = 0x0,
				STRUCTURED_TABLE_CELL_TYPE_STRING = 0x1,
				STRUCTURED_TABLE_CELL_TYPE_NUMBER = 0x2,
				STRUCTURED_TABLE_CELL_TYPE_COUNT = 0x3,
			};

			struct StructuredTableCell {
				StructuredTableCellType type;
				uintptr_t string; // const char*
				int32_t number;
				int32_t hash;
			};

			// &a1->cells[column + columnCount * row]

			auto pool = std::make_unique<StructuredTable[]>(entry.itemCount);

			if (!proc.ReadMemory(&pool[0], entry.pool, sizeof(pool[0]) * entry.itemCount)) {
				std::cerr << "Can't read pool data\n";
				return tool::BASIC_ERROR;
			}
			char dumpbuff[MAX_PATH + 10];
			std::vector<byte> read{};

			for (size_t i = 0; i < entry.itemCount; i++) {
				const auto& p = pool[i];

				auto n = proc.ReadStringTmp(p.name, nullptr);

				if (IsNameValid(n) && (p.columnCount * p.rowCount)) {
					sprintf_s(dumpbuff, "%s/%s", opt.m_output, n);
				}
				else {
					continue;
				}

				std::filesystem::path file{ dumpbuff };

				bool exist{ std::filesystem::exists(file, ec) };
				if (exist && opt.HasFlag(PDOF_IGNORE_EXIST)) {
					continue;
				}

				std::cout << std::dec << i << ": ";

				auto [cells, okp] = proc.ReadMemoryArray<StructuredTableCell>(p.cells, p.columnCount * p.rowCount);
				auto [headers, okh] = proc.ReadMemoryArray<StructuredTableHeader>(p.headers, p.columnCount);

				if (!okp || !okh) {
					std::cerr << "Can't read " << n << "\n";
					continue;
				}

				std::cout << dumpbuff;

				std::filesystem::create_directories(file.parent_path(), ec);

				std::cout << "->" << file;

				if (!exist) {
					readFile++;
					std::cout << " (new)";
				}
				std::cout << "\n";


				std::ofstream out{ file };

				if (!out) {
					std::cerr << "Can't create " << file.string() << "\n";
					continue;
				}

				out << "[";
				for (size_t i = 0; i < p.rowCount; i++) {
					if (i) out << ",";
					out << "\n";
					utils::Padding(out, 1) << "{";

					int cIdx{};
					for (size_t j = 0; j < p.columnCount; j++) {
						StructuredTableCell& cell{ cells[i * p.columnCount + j] };
						StructuredTableHeader& header{ headers[j] };
						if (cell.type == STRUCTURED_TABLE_CELL_TYPE_NONE) continue;
						if (cIdx++) out << ",";
						utils::Padding(out << "\n", 2) << "\"" << proc.ReadStringTmp(header.string) << "\": ";

						switch (cell.type) {
						case STRUCTURED_TABLE_CELL_TYPE_NUMBER:
							out << std::dec << cell.number;
							break;
						case STRUCTURED_TABLE_CELL_TYPE_STRING:
							PrintFormattedString(out << "\"", proc.ReadStringTmp(cell.string)) << "\"";
							break;
						default:
							out << "\"<invald type:" << std::dec << cell.type << ">\"";
							break;
						}
					}
					utils::Padding(out << "\n", 1) << "}";
				}
				out << "\n]";

				out.close();

			}
		}
		std::cout << "Dump " << std::dec << readFile << " new file(s)\n";
		return tool::OK;
	}
	
namespace {
	int t7poolscripts(Process& proc, int argc, const char* argv[]) {
		std::filesystem::path outDir;
		if (argc == 2) {
			outDir = "scriptparsetree_t7";
		}
		else {
			outDir = argv[2];
		}

		std::filesystem::create_directories(outDir);
		size_t added{};

		LOG_INFO("Read linked infos...");


		auto [gObjFileInfoCount, ok3] = proc.ReadMemoryArray<int32_t>(proc[gscobjinfocount], 2);
		auto [gObjFileInfo, ok4] = proc.ReadMemoryArray<objFileInfo_t[500]>(proc[gscobjinfo], 2);

		if (!ok3 || !ok4) {
			LOG_ERROR("Error when reading gObjFileInfo");
			return tool::BASIC_ERROR;
		}

		tool::gsc::T7GSCOBJ header{};

		for (size_t inst = 0; inst < 2; inst++) {
			auto& gObjFileInfoI{ gObjFileInfo[inst] };
			for (size_t i = 0; i < gObjFileInfoCount[inst]; i++) {
				auto& entry{ gObjFileInfoI[i] };

				if (!entry.activeVersion || !entry.name) {
					continue;
				}

				const char* name{ proc.ReadStringTmp(entry.name, nullptr) };

				if (!name) {
					continue; // can't read name
				}

				if (!proc.ReadMemory(&header, entry.activeVersion, sizeof(header))) {
					continue; // can't read header
				}

				// probably profile or another shit, but this isn't used by the game
				if (header.script_size < sizeof(header) || header.script_size < header.cseg_offset + header.cseg_size) {
					continue; // bad size, end before cseg
				}
				auto [buffer, okb] = proc.ReadMemoryArray<byte>(entry.activeVersion, header.script_size);

				if (!okb) {
					LOG_ERROR("Can't read script {}", name);
					continue;
				}
				uint64_t magic = *reinterpret_cast<uint64_t*>(&buffer[0]);
				if ((magic & 0xFFFFFFFFFFFFFF) != tool::gsc::opcode::VMI_TRE_BASE) {
					LOG_ERROR("Can't read script {}: invalid magic", name);
					continue;
				}
				byte revision{ buffer[7] };

				std::filesystem::path outFile{ outDir / utils::va("vm_%x", revision) / utils::va("%s.gscc", name)};
				bool isNew{ !std::filesystem::exists(outFile) };
				if (isNew) {
					std::filesystem::create_directories(outFile.parent_path());
					added++;
				}
				if (!utils::WriteFile(outFile, &buffer[0], header.script_size)) {
					LOG_ERROR("Error when dumping {} bytes into {}", header.script_size, outFile.string());
					continue;
				}
				LOG_INFO("Dump into {} ({}){}", outFile.string(), header.script_size, isNew ? " (new)" : "");
			}
		}

		LOG_INFO("For now {} file(s) added", added);

		LOG_INFO("Read SPT pool...");
		const uintptr_t spt = proc[xassetpools] + sizeof(bo3::pool::T7XAssetPool) * bo3::pool::T7_ASSET_TYPE_SCRIPTPARSETREE;
		auto [pool, ok] = proc.ReadMemoryObject<bo3::pool::T7XAssetPool>(spt);

		if (!ok) {
			LOG_ERROR("Can't read SPT pool: {}", spt);
			return tool::BASIC_ERROR;
		}

		auto [entries, ok2] = proc.ReadMemoryArray<bo3::pool::T7ScriptParseTree>(pool->pool, pool->itemAllocCount);

		if (!ok2) {
			LOG_ERROR("Can't read SPT pool data");
			return tool::BASIC_ERROR;
		}

		for (size_t i = 0; i < pool->itemAllocCount; i++) {
			bo3::pool::T7ScriptParseTree& entry{ entries[i] };

			const char* name{ proc.ReadStringTmp(entry.name, nullptr) };

			if (!name) {
				continue;
			}

			auto [buffer, okb] = proc.ReadMemoryArray<byte>(entry.script, entry.scriptSize);

			if (!okb) {
				LOG_ERROR("Can't read script {}", name);
				continue;
			}
			uint64_t magic = *reinterpret_cast<uint64_t*>(&buffer[0]);
			if ((magic & 0xFFFFFFFFFFFFFF) != tool::gsc::opcode::VMI_TRE_BASE) {
				LOG_ERROR("Can't read script {}: invalid magic", name);
				continue;
			}
			byte revision{ buffer[7] };

			std::filesystem::path outFile{ outDir / utils::va("vm_%x", revision) / utils::va("%s.gscc", name) };
			bool isNew{ !std::filesystem::exists(outFile) };
			if (isNew) {
				std::filesystem::create_directories(outFile.parent_path());
				added++;
			}
			if (!utils::WriteFile(outFile, &buffer[0], entry.scriptSize)) {
				LOG_ERROR("Error when dumping {} bytes into {}", entry.scriptSize, outFile.string());
				continue;
			}
			LOG_INFO("Dump into {} ({}){}", outFile.string(), entry.scriptSize, isNew ? " (new)" : "");
		}

		LOG_INFO("Done, {} file(s) added", added);

		return tool::OK;
	}



	
	ADD_TOOL(dpt7, "bo3", " [pool]", "Black Ops 3 dump pool", L"BlackOps3.exe", t7dp);
	ADD_TOOL(wpst7, "bo3", " [output=scriptparsetree_t7]", "dump pooled scripts (bo3)", L"BlackOps3.exe", t7poolscripts);
}
