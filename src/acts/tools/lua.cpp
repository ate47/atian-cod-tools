#include <includes.hpp>
#include "tools/pool.hpp"
#include "lua.hpp"
#include "lua_opcodes.hpp"
#include "lua_opcodes_load.hpp"

namespace tool::lua {
	using namespace opcodes;

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
	LuaType ReadLuaType(const char* type) {
		static std::unordered_map<std::string, LuaType> luaTypeMap{
			{ "TNIL", LT_NIL },
			{ "TBOOLEAN", LT_BOOLEAN },
			{ "TLIGHTUSERDATA", LT_LIGHTUSERDATA },
			{ "TNUMBER", LT_NUMBER },
			{ "TSTRING", LT_STRING },
			{ "TTABLE", LT_TABLE },
			{ "TFUNCTION", LT_FUNCTION },
			{ "TUSERDATA", LT_USERDATA },
			{ "TTHREAD", LT_THREAD },
			{ "TIFUNCTION", LT_IFUNCTION },
			{ "TCFUNCTION", LT_CFUNCTION },
			{ "TUI64", LT_UI64 },
			{ "TSTRUCT", LT_STRUCT },
			{ "TXHASH", LT_XHASH },
		};

		auto it = luaTypeMap.find(type);

		return it == luaTypeMap.end() ? LT_BADTYPE : it->second;
	}


	struct CLIOption {
		bool m_help{};
		bool m_verbose{ true };
		bool m_header{};
		bool m_asm{};
		bool m_inst{};
		bool m_types{};

		const char* m_outputDir{};
		std::vector<const char*> m_inputFiles{};

		bool Compute(const char** args, INT startIndex, INT endIndex) {
			// default values
			for (size_t i = startIndex; i < endIndex; i++) {
				const char* arg = args[i];

				if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
					m_help = true;
				}
				else if (!strcmp("-s", arg) || !_strcmpi("--silent", arg)) {
					m_verbose = false;
				}
				else if (!strcmp("-H", arg) || !_strcmpi("--header", arg)) {
					m_header = true;
				}
				else if (!strcmp("-T", arg) || !_strcmpi("--types", arg)) {
					m_types = true;
				}
				else if (!strcmp("-a", arg) || !_strcmpi("--asm", arg)) {
					m_asm = true;
				}
				else if (!strcmp("-i", arg) || !_strcmpi("--inst", arg)) {
					m_inst = true;
				}
				else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					m_outputDir = args[++i];
				}

				else if (*arg == '-') {
					std::cerr << "Unknown option: " << arg << "!\n";
					return false;
				}
				else {
					m_inputFiles.push_back(arg);
				}
			}
			if (!m_inputFiles.size()) {
				m_inputFiles.push_back("scriptparsetree");
			}
			return true;
		}

		void PrintHelp() {
			LOG_INFO("-h --help       : Print help");
			LOG_INFO("-o --output [d] : Output dir");
			LOG_INFO("-H --header     : Decompile script header");
			LOG_INFO("-a --asm        : Decompile ASM");
			LOG_INFO("-i --inst       : Decompile instructions");
			LOG_INFO("-T --types      : Decompile types name");
		}
	};

	struct LuaHeader {
		// 0
		uint32_t magic;
		// 4
		byte version;
		byte compVersion;
		byte endian;
		byte sizeInt;
		byte sizeSize;
		byte sizeInst;
		byte sizeNumber;
		byte flag;
		byte game;
		// 14
		uint16_t numTypeObjectsA;
	};

	struct LuaInst {
		uint64_t code;
		uint64_t data;
		size_t loc;
	};
	class HKSInstHandler;

	class LuaDecompContext {
	public:
		CLIOption& opt;
		byte* file;
		size_t size;
		LuaHeader* header;
		size_t loc{};
		opcodes::LuaVM vm;
		std::unordered_map<uint32_t, LuaType> typeIds{};
		int depth{};
		LuaDecompContext(CLIOption& opt, byte* file, size_t size) : opt(opt), file(file), size(size), header((LuaHeader*)file) {
			vm = (opcodes::LuaVM)opcodes::LuaVmOf(header->magic, header->version);
		}

		template<typename T>
		T Read() {
			if (loc + sizeof(T) > size) {
				throw std::runtime_error("Reading too much");
			}
			T* v{ (T*)&file[loc] };
			loc += sizeof(T);
			return *v;
		}

		const char* ReadString() {
			const char* ptr{ (const char*)&file[loc] };
			size_t len{ std::strlen(ptr) };
			loc += len + 1; // \0
			return ptr;
		}

		template<typename T>
		void Align() {
			loc = utils::Aligned<T>(loc);
		}

		HKSInstId GetOpCodeId(uint64_t code) {
			return opcodes::GetOpCode(vm, code);
		}

		std::ostream& WriteAsmLocPadding(std::ostream& out, size_t floc = 0) {
			return utils::Padding(out, depth) << "." << std::setfill('0') << std::setw(7) << std::hex << (floc ? floc : loc) << " ";
		}

		LuaInst ReadInst() {
			switch (header->sizeInst) {
			case 4: {
				size_t floc = loc;
				uint32_t inst{ Read<uint32_t>() };

				return { inst >> 25, inst & ~(~0 << 25), floc };
			}
			default:
				throw std::runtime_error(utils::va("Invalid inst size %d", header->sizeInst));
			}
		}

		int64_t ReadInt() {
			switch (header->sizeInt) {
			case 1:
				return Read<int8_t>();
			case 2:
				return Read<int16_t>();
			case 4:
				return Read<int32_t>();
			case 8:
				return Read<int64_t>();
			default:
				throw std::runtime_error(utils::va("Invalid int size %d", header->sizeInt));
			}
		}

		double ReadNumber() {
			switch (header->sizeNumber) {
			case 4:
				return Read<float>();
			case 8:
				return Read<double>();
			default:
				throw std::runtime_error(utils::va("Invalid number size %d", header->sizeNumber));
			}
		}
	};

	enum HKSConstType {
		HKSC_UNK = 0,
		HKSC_NIL,
		HKSC_HASH,
		HKSC_NUMBER,
		HKSC_INT,
		HKSC_BOOL,
		HKSC_STRING,
	};

	class HKSConstVal {
	public:
		HKSConstType type;
		HKSConstVal(HKSConstType type) : type(type) {}
		virtual ~HKSConstVal() {}
		virtual void Dump(std::ostream& out) {
			out << "<INVALID>";
		}
	};
	class HKSConstValNil : public HKSConstVal {
	public:
		HKSConstValNil() : HKSConstVal(HKSC_NIL) {}
		void Dump(std::ostream& out) override {
			out << "nil";
		}
	};

	class HKSConstValHash : public HKSConstVal {
	public:
		uint64_t val;
		HKSConstValHash(uint64_t val) : HKSConstVal(HKSC_HASH), val(val) {}
		void Dump(std::ostream& out) override {
			out << "@\"" << hashutils::ExtractTmp("hash", val) << "\"";
		}
	};

	class HKSConstValBool : public HKSConstVal {
	public:
		bool val;
		HKSConstValBool(bool val) : HKSConstVal(HKSC_BOOL), val(val) {}
		void Dump(std::ostream& out) override {
			out << val ? "true" : "false";
		}
	};

	class HKSConstValNumber : public HKSConstVal {
	public:
		double val;
		HKSConstValNumber(double val) : HKSConstVal(HKSC_NUMBER), val(val) {}
		void Dump(std::ostream& out) override {
			out << val;
		}
	};

	class HKSConstValUInt : public HKSConstVal {
	public:
		uint64_t val;
		HKSConstValUInt(int64_t val) : HKSConstVal(HKSC_INT), val(val) {}
		void Dump(std::ostream& out) override {
			out << val;
		}
	};

	class HKSConstValStr : public HKSConstVal {
	public:
		const char* val;
		HKSConstValStr(const char* val) : HKSConstVal(HKSC_INT), val(val) {}
		void Dump(std::ostream& out) override {
			PrintFormattedString(out << "\"", val) << "\"";
		}
	};
	class HKSInstHandler {
	public:
		const char* name;
		opcodes::HKSInstId type;

		HKSInstHandler(opcodes::HKSInstId type, const char* name) : type(type), name(name) {}
		virtual ~HKSInstHandler() {}

		virtual bool Read(std::ostream& out, LuaDecompContext& ctx) {
			out << "Invalid OP\n";
			return false;
		}
	};


	namespace {
		std::unordered_map<opcodes::HKSInstId, HKSInstHandler*> handlers{
		
		};

		HKSInstHandler* GetInstHandler(opcodes::HKSInstId id) {
			auto it = handlers.find(id);
			if (it == handlers.end()) {
				static thread_local HKSInstHandler def{ opcodes::LUA_OPCODE_UNDEFINED, "UNDEFINED" };
				return &def;
			}
			return it->second;
		}
	}

	struct LuaFuncHeader {
		uint32_t unk0;
		uint32_t params;
		bool varArgs;
		uint32_t registerCount;
		uint32_t instsCount;
		uint32_t unk17;
		size_t location;
		size_t startOp;
		size_t sizeOp{};
		uint32_t constsCount;
		uint32_t childFunc0;
		uint32_t childFunc4;
		uint32_t childFuncCount;

		std::vector<LuaInst> insts{};
		std::vector<HKSConstVal*> consts{};
		std::vector<LuaFuncHeader> childFuncs{};

		void Clean() {
			for (LuaFuncHeader& child : childFuncs) {
				child.Clean();
			}
			for (HKSConstVal* cst : consts) {
				delete cst;
			}
		}

		bool Decompile(std::ostream& asmout, LuaDecompContext& ctx) {
			// dump asm
			bool anyErr{};

			std::ostream& out = ctx.opt.m_asm ? asmout : utils::NullStream();
			for (LuaInst& inst : insts) {
				HKSInstId id{ ctx.GetOpCodeId(inst.code) };
				HKSInstHandler* handler{ GetInstHandler(id) };

				ctx.WriteAsmLocPadding(out, inst.loc)
					<< std::setfill('0') << std::setw(2) << (int)inst.code
					<< " "
					<< std::setfill(' ') << std::setw(28) << std::left
					<< GetOpCodeName(id)
					<< std::right << " ";

				if (!handler) {
					LOG_ERROR("Can't find opcode for 0x{:x}/0x{:x}", inst.code, (int)id);
					anyErr = true;
					break;
				}

				if (!handler->Read(out, ctx)) {
					anyErr = true;
					break;
				}
			}

			for (LuaFuncHeader& child : childFuncs) {
				ctx.depth++;
				anyErr |= child.Decompile(asmout, ctx);
				ctx.depth--;
			}

			return !anyErr;
		}

		bool Read(std::ostream& asmout, LuaDecompContext& ctx) {
			location = ctx.loc;

			unk0 = ctx.Read<uint32_t>();
			params = ctx.Read<uint32_t>();
			varArgs = ctx.Read<byte>() == 1;
			registerCount = ctx.Read<uint32_t>();
			instsCount = ctx.Read<uint32_t>();
			unk17 = ctx.Read<uint32_t>();
			ctx.Align<uint32_t>();

			startOp = ctx.loc;


			// dump inst
			std::ostream& out = ctx.opt.m_inst ? asmout : utils::NullStream();

			DumpHeader(out, ctx);

			for (size_t i = 0; i < instsCount; i++) {
				ctx.WriteAsmLocPadding(out);
				LuaInst inst{ ctx.ReadInst() };
				out << std::hex << std::setfill('0') << std::setw(2) << inst.code << " ";

				if (inst.data) {
					out << "0x" << inst.data;
				}
				out << "\n";
				insts.push_back(inst);
			}

			constsCount = ctx.Read<uint32_t>();

			for (size_t i = 0; i < constsCount; i++) {
				ctx.WriteAsmLocPadding(out) << "- 0x" << std::hex << i << ": ";
				byte type = ctx.Read<byte>();

				auto tit = ctx.typeIds.find(type);

				if (tit == ctx.typeIds.end()) {
					LOG_ERROR("Unknown const type {}", (int)type);
					return false;
				}

				LuaType ct{ tit->second };
				HKSConstVal* val{};

				switch (ct) {
				case LT_NIL:
					val = new HKSConstValNil();
					break; // nothing
				case LT_XHASH:
					val = new HKSConstValHash(ctx.Read<uint64_t>());
					break;
				case LT_BOOLEAN:
					val = new HKSConstValBool(ctx.Read<byte>() == 1);
					break;
				case LT_NUMBER:
					val = new HKSConstValNumber(ctx.ReadNumber());
					break;
				case LT_UI64:
					val = new HKSConstValUInt(ctx.Read<uint64_t>());
					break;
				case LT_STRING: {
					uint32_t len = ctx.Read<uint32_t>();
					uint32_t unk4 = ctx.Read<uint32_t>();
					const char* str = ctx.ReadString();
					val = new HKSConstValStr(str);
					break;
				}
				case LT_LIGHTUSERDATA:
				case LT_TABLE:
				case LT_FUNCTION:
				case LT_USERDATA:
				case LT_THREAD:
				case LT_IFUNCTION:
				case LT_CFUNCTION:
				case LT_STRUCT:
				default:
					LOG_ERROR("Const handler type not implemented {}", (int)ct);
					return false;
				}

				consts.push_back(val);
				val->Dump(out);
				out << "\n";
			}
			childFunc0 = ctx.Read<uint32_t>();
			childFunc4 = ctx.Read<uint32_t>();
			childFuncCount = ctx.Read<uint32_t>();
			// childFuncs
			ctx.depth++;
			for (size_t i = 0; i < childFuncCount; i++) {
				auto& child = childFuncs.emplace_back();

				if (!child.Read(asmout, ctx)) {
					ctx.depth--;
					return false;
				}
			}
			ctx.depth--;
			sizeOp = ctx.loc - startOp;

			return true;
		}

		void DumpHeader(std::ostream& out, LuaDecompContext& ctx) {
			utils::Padding(out, ctx.depth) << "-- Function(" << std::dec << params << (varArgs ? "..." : "") << ")\n";
			utils::Padding(out, ctx.depth) << "-- Register: " << registerCount << "\n";
			utils::Padding(out, ctx.depth) << "-- Instructions: " << instsCount << "\n";
			utils::Padding(out, ctx.depth) << "-- Unk17: " << unk17 << "\n";
			utils::Padding(out, ctx.depth) << "-- Location: 0x" << std::hex << location << "/0x" << startOp << "\n";
			if (sizeOp) {
				utils::Padding(out, ctx.depth) << "-- Size: 0x" << std::hex << sizeOp << "\n";
			}
		}

		void Dump(std::ostream& out, LuaDecompContext& ctx) {
			DumpHeader(out, ctx);
		}
	};

	static bool HandleByteCode(byte* buffer, size_t size, const std::filesystem::path& origin, CLIOption& opt) {
		constexpr size_t luaHeader = sizeof(LuaHeader) - sizeof(LuaHeader::numTypeObjectsA);
		constexpr size_t minimumSize = luaHeader // header
			+ sizeof(uint32_t) // numTypeObjects
			+ sizeof(uint32_t) // typeId
			+ sizeof(uint32_t) // nameSize
			;
		LuaDecompContext ctx{ opt, buffer, size };
		LuaHeader* header{ ctx.header };
		if (size < minimumSize || header->magic != 0x61754C1B) {
			LOG_ERROR("Bad magic: 0x{:x}", header->magic);
			return false;
		}

		if (header->version != 0x51) {
			LOG_ERROR("Bad version: 0x{:x}", (int)header->version);
			return false;
		}

		// header is 14 bytes
		size_t& loc{ ctx.loc };

		loc = luaHeader;

		std::filesystem::path outputFile;
		const auto fn = origin.filename().string();
		if (opt.m_outputDir) {
			outputFile = std::filesystem::path{ opt.m_outputDir } / std::format("{}.dec.lua", fn);
		}
		else {
			outputFile = origin.parent_path() / std::format("{}.dec.lua", fn);
		}

		std::filesystem::create_directories(outputFile.parent_path());

		std::ofstream out{ outputFile };

		if (!out) {
			LOG_ERROR("Can't create output file {}", outputFile.string());
			return false;
		}

		utils::CloseEnd ce{ out };

		if (opt.m_header) {
			out << "-- file: " << origin.string() << "\n"
				<< std::hex << "-- Magic ....... 0x" << header->magic << "\n"
				<< std::hex << "-- Version ..... 0x" << (int)header->version << " / 0x" << (int)header->compVersion << " 0x" << ctx.vm << "\n"
				<< std::hex << "-- Endian ...... 0x" << (int)header->endian << "\n"
				<< std::hex << "-- Size ........"
				<< " sizeInt:0x" << (int)header->sizeInt
				<< " sizeSize:0x" << (int)header->sizeSize
				<< " sizeInst:0x" << (int)header->sizeInst
				<< " sizeNumber:0x" << (int)header->sizeNumber << "\n"
				<< std::hex << "-- Flag ........ 0x" << (int)header->flag << "\n"
				<< std::hex << "-- Game ........ 0x" << (int)header->game << "\n"
				;
		}


		uint32_t numTypeObjects = ctx.Read<uint32_t>();

		if (opt.m_types) {
			out << "-- types (" << std::dec << numTypeObjects << "):\n";
		}

		for (size_t i = 0; i < numTypeObjects; i++) {
			if (loc + 8 >= size) {
				LOG_ERROR("Bad type declaration (not enough bytes)");
				return false;
			}

			uint32_t typeId = ctx.Read<uint32_t>();
			uint32_t typeSize = ctx.Read<uint32_t>();

			if (typeSize > 32 || loc + typeSize >= size || buffer[loc + typeSize - 1]) {
				LOG_ERROR("Bad text buffer");
				return false;
			}

			const char* name = ctx.ReadString();
			LuaType luaType = ReadLuaType(name);

			if (luaType == LT_BADTYPE) {
				LOG_ERROR("Unknown Lua type: {}", name);
				return false;
			}

			ctx.typeIds[typeId] = luaType;

			if (opt.m_types) {
				out << "-- " << std::dec << std::setw(2) << std::setfill(' ') << typeId << " : " << name << "\n";
			}
		}

		LuaFuncHeader main{};
		utils::CloseEnd fce{ [&main] { main.Clean(); } };

		// if we aren't dumping the ASM, we compute all the nodes first
		std::ostream& asmout = opt.m_asm ? out : utils::NullStream();

		bool ok = main.Read(asmout, ctx);

		if (!ok) {
			out << "-- error when reading script\n";
			return false;
		}
		main.Dump(out, ctx);
		main.Decompile(out, ctx);

		LOG_INFO("Decompiled into {}", outputFile.string());

		return true;
	}

	namespace {
		int luad(int argc, const char* argv[]) {
			CLIOption opt{};

			if (!opt.Compute(argv, 2, argc) || opt.m_help || !opt.m_inputFiles.size()) {
				opt.PrintHelp();
				return tool::OK;
			}
			lua::opcodes::RegisterLuaOpCodesMap();

			std::vector<std::filesystem::path> paths{};

			for (const auto* file : opt.m_inputFiles) {
				utils::GetFileRecurse(file, paths, [](const std::filesystem::path& f) {
					const auto str = f.string();
					return str.ends_with(".luac") || str.ends_with(".lua");
				});
			}

			if (!paths.size()) {
				LOG_ERROR("No input file");
				return tool::BASIC_ERROR;
			}

			std::string buffer{};

			for (const auto& path : paths) {
				LOG_TRACE("Reading {}", path.string());

				if (!utils::ReadFile(path, buffer)) {
					LOG_ERROR("Can't read file {}", path.string());
					continue;
				}

				if (!HandleByteCode(reinterpret_cast<byte*>(buffer.data()), buffer.length(), path, opt)) {
					LOG_ERROR("Error when reading {}", path.string());
				}
			}
			LOG_INFO("Done");

			return tool::OK;
		}
	}
	ADD_TOOL(luad, "lua", " [file]", "Lua Havok Script", luad);
}
