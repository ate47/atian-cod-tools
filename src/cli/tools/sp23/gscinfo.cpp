#include <includes.hpp>
#include "tools/gsc.hpp"

// tests with mwiii
namespace {

	struct StringInfo {
		UINT64 script;
		std::string str;
		BYTE ref;
		BYTE type;
	};

	struct GscInfo23Option {
		bool m_dcomp = false;
		bool m_dasm = false;
		bool m_help = false;
		bool m_verbose = true;
		bool m_header = false;
		bool m_imports = false;
		bool m_strings = false;
		bool m_gvars = false;
		bool m_includes = true;
		bool m_addresses = false;
		bool m_func_rloc = false;
		LPCCH m_dump_str = NULL;
		LPCCH m_outputDir = NULL;
		LPCCH m_copyright = NULL;


		std::vector<LPCCH> m_inputFiles{};
		std::vector<StringInfo> m_dumpStrings{};

		bool Compute(LPCCH* args, INT startIndex, INT endIndex) {
			// default values
			for (size_t i = startIndex; i < endIndex; i++) {
				LPCCH arg = args[i];

				if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
					m_help = true;
				}
				else if (!strcmp("-g", arg) || !_strcmpi("--gsc", arg)) {
					m_dcomp = true;
				}
				else if (!strcmp("-a", arg) || !_strcmpi("--asm", arg)) {
					m_dasm = true;
				}
				else if (!strcmp("-s", arg) || !_strcmpi("--silent", arg)) {
					m_verbose = false;
				}
				else if (!strcmp("-H", arg) || !_strcmpi("--header", arg)) {
					m_header = true;
				}
				else if (!strcmp("-I", arg) || !_strcmpi("--imports", arg)) {
					m_imports = true;
				}
				else if (!strcmp("-S", arg) || !_strcmpi("--strings", arg)) {
					m_strings = true;
				}
				else if (!strcmp("-G", arg) || !_strcmpi("--gvars", arg)) {
					m_gvars = true;
				}
				else if (!strcmp("-U", arg) || !_strcmpi("--noincludes", arg)) {
					m_includes = false;
				}
				else if (!strcmp("-l", arg) || !_strcmpi("--rloc", arg)) {
					m_func_rloc = true;
				}
				else if (!strcmp("-A", arg) || !_strcmpi("--addresses", arg)) {
					m_addresses = true;
				}
				else if (!_strcmpi("--dump-string", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					m_dump_str = args[++i];
				}
				else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					m_outputDir = args[++i];
				}
				else if (!strcmp("-C", arg) || !_strcmpi("--copyright", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					m_copyright = args[++i];
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

		void PrintHelp(std::ostream& out) {
			out << "-h --help          : Print help\n"
				<< "-g --gsc           : Produce GSC\n"
				<< "-a --asm           : Produce ASM\n"
				<< "-o --output [d]    : ASM/GSC output dir, default same.gscasm\n"
				<< "-s --silent        : Silent output, only errors\n"
				<< "-H --header        : Write file header\n"
				<< "-l --rloc          : Write relative location of the function code\n"
				<< "-I --imports       : Write imports\n"
				<< "-S --strings       : Write strings\n"
				<< "-G --gvars         : Write gvars\n"
				<< "-A --addresses     : Element addresses\n"
				<< "-C --copyright [t] : Set a comment text to put in front of every file\n";
		}

		void AddString(UINT64 script, LPCCH str, BYTE count, BYTE type) {
			if (m_dump_str) {
				m_dumpStrings.emplace_back(script, str, count, type);
			}
		}
	};

	struct GscObj23 {
		byte magic[8];
		uint64_t name;
		uint16_t size;
		uint16_t unk2c_count;
		uint16_t unk30_count;
		uint16_t unk16;
		uint16_t export_count;
		uint16_t fixup_count;
		uint16_t unk1C;
		uint16_t imports_count;
		uint16_t includes_count;
		uint16_t unk22;
		uint16_t string_count;
		uint16_t unk26;
		uint16_t unk28;
		uint16_t unk2A;
		uint32_t unk2c_offset;
		uint32_t unk30_offset;
		uint32_t unk34;
		uint32_t unk38;
		uint32_t unk3C;
		uint32_t export_offset;
		uint32_t fixup_offset;
		uint32_t unk48;
		uint32_t import_table;
		uint32_t include_table;
		uint32_t unk54;
		uint32_t string_table;
		uint32_t unk5C;

		UINT64 GetMagic() {
			return *reinterpret_cast<UINT64*>(magic);
		}
	};
	struct GSC_STRINGTABLE_ITEM
	{
		uint32_t string;
		uint8_t num_address;
		uint8_t type;
		uint8_t pad[2];
	};
	struct GSC_EXPORT_ITEM
	{
		uint64_t name;
		uint64_t name_space;
		uint64_t file_name_space;
		uint64_t checksum;
		uint32_t address;
		uint8_t param_count;
		uint8_t flags;
	};
	struct GSC_IMPORT_ITEM
	{
		uint64_t name;
		uint64_t name_space;
		uint16_t num_address;
		uint8_t param_count;
		byte flags;
	};

	struct Ctx23 {
		LPCCH outputDir;
		GscInfo23Option& opt;
		std::unordered_set<UINT64> hashes{};


		LPCCH Extract(LPCCH type, UINT64 hash) {
			hashes.insert(hash);
			return hashutils::ExtractTmp(type, hash);
		}
		LPCCH ExtractScript(UINT64 hash) {
			hashes.insert(hash);
			return hashutils::ExtractTmpScript(hash);
		}
	};

	struct ExportCTX {
		Ctx23& ctx;
		const GSC_EXPORT_ITEM& exp;
		GscObj23& file;
		std::vector<UINT64> params{};
		bool error = false;
		byte* bytecode = nullptr;

		void DumpHeader(std::ostream& asmout) {
			asmout
				<< "// Namespace " << ctx.Extract("namespace", exp.name_space) << std::flush << "/" << ctx.Extract("namespace", exp.file_name_space) << "\n"
				<< "// Params " << std::dec << (int)exp.param_count << ", eflags: 0x" << std::hex << (int)exp.flags;

			if (exp.flags & tool::gsc::T8GSCExportFlags::AUTOEXEC) {
				asmout << " autoexec?";
			}

			if (exp.flags & tool::gsc::T8GSCExportFlags::PRIVATE) {
				asmout << " private?";
			}

			if (exp.flags & tool::gsc::T8GSCExportFlags::EVENT) {
				asmout << " event?";
			}

			if (exp.flags & tool::gsc::T8GSCExportFlags::VE) {
				asmout << " ve?";
			}

			if (exp.flags & tool::gsc::T8GSCExportFlags::LINKED) {
				asmout << " linked?";
			}

			asmout
				<< "\n"
				<< "// Checksum 0x" << std::hex << exp.checksum << ", Offset: 0x" << exp.address << "\n"
				<< "function " << ctx.Extract("function", exp.name);
			asmout << "(";
			if (params.size() >= exp.param_count) {
				for (size_t i = 0; i < exp.param_count; i++) {
					if (i) {
						asmout << ", ";
					}

					asmout << hashutils::ExtractTmp("var", params[i]) << std::flush;
				}
			}

			asmout << ")";
		}

		UINT32 RLoc() {
			return (UINT32)(bytecode - file.magic);
		}

		std::ostream& WriteLocation(std::ostream& out) {
			return out << "." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << RLoc() << ": ";
		}

		std::ostream& WriteOperation(std::ostream& out, BYTE code, LPCCH name) {
			return WriteLocation(out) << std::hex << std::setfill('0') << std::setw(sizeof(byte) << 1) << (int) code
				<< " "
				<< std::setfill(' ') << std::setw(25) << std::left << name << std::right
				<< " ";
		}
		std::ostream& WritePadding(std::ostream& out) {
			return out << "." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << RLoc() << ": "
				// no opcode write
				<< std::setfill(' ') << std::setw((sizeof(byte) << 1) + 25 + 2) << " ";
		}

		void HandleASM(std::ostream& asmout) {
			bytecode = file.magic + exp.address;
			while (true) {
				auto opcode = *(bytecode++);
				switch (opcode) {
				case 0x98: { // create params
					auto p = *(bytecode++);
					if (p < exp.param_count) {
						asmout << "Bad param count, excepted " << (int)exp.param_count << " had " << (int)p << "\n";
						error = true;
						break;
					}

					WriteOperation(asmout, opcode, "AllocateParams") << std::dec << (int)p << " names:\n";

					for (size_t i = 0; i < p; i++) {
						auto varname = *reinterpret_cast<UINT64*>(bytecode);
						params.push_back(varname);
						WritePadding(asmout) << i << " - " << hashutils::ExtractTmp("var", varname) << "\n";
						bytecode += 8;
					}
					break;
				}
				case 0x43: // check clear params
					WriteOperation(asmout, opcode, "CheckClearParams") << "\n";
					if (exp.param_count) {
						asmout << "Bad param count, excepted " << (int)exp.param_count << " had 0\n";
						error = true;
						break;
					}
					break;
				default:
					asmout << "Unknown bytecode operation at location 0x" << (int)*bytecode << "\n";
					for (size_t i = 0; i < 0x30; i++) {
						asmout << std::hex << std::setw(2) << std::setfill('0') << (int)bytecode[i] << " ";
					}
					asmout << "\n";
					error = true;
					return;
				}
			}
		}
	};


	int handlefile(GscObj23* file, size_t size, const std::filesystem::path& path, Ctx23& ctx) {
		char outFileName[MAX_PATH + 1];

		if (file->GetMagic() != 0xa0d4353478a) {
			std::cerr << "Bad magic for " << path << "\n";
			return tool::OK;
		}
		
		ctx.ExtractScript(file->name);

		auto n = hashutils::ExtractPtr(file->name);

		std::filesystem::path outFile;

		if (ctx.outputDir) {
			if (n) {
				outFile = std::filesystem::path(ctx.outputDir) / n;
			}
			else {
				sprintf_s(outFileName, "script_%llx.gsc", file->name);
				outFile = std::filesystem::path(ctx.outputDir) / "hashed" / outFileName;
			}
		}
		else {
			outFile = std::filesystem::path(std::format("{}asm", path.string()));
		}


		std::cout << "Parsing " << path << " to " << outFile << "...\n";

		std::filesystem::create_directories(outFile.parent_path());

		std::ofstream asmout{ outFile };

		if (!asmout) {
			std::cerr << "Can't create file " << outFile << "\n";
			return tool::BASIC_ERROR;
		}

		if (ctx.opt.m_header) {
			asmout
				<< "// Name:     " << ctx.ExtractScript(file->name) << " magic: 0x" << std::hex << file->GetMagic() << " Size:     0x" << std::hex << size << "\n"
				<< "// Exports:  " << std::dec << file->export_count << " -> 0x" << std::hex << file->export_offset << "\n"
				<< "// Strings:  " << std::dec << file->string_count << " -> 0x" << std::hex << file->string_table << "\n"
				<< "// Imports:  " << std::dec << file->imports_count << " -> 0x" << std::hex << file->import_table << "\n"
				<< "// Includes: " << std::dec << file->includes_count << " -> 0x" << std::hex << file->include_table << "\n"
				<< "// Fixups:   " << std::dec << file->fixup_count << " -> 0x" << std::hex << file->fixup_offset << "\n"
				<< "// Unk2C:    " << std::dec << file->unk2c_count << " -> 0x" << std::hex << file->unk2c_offset << "\n"
				<< "// Unk30:    " << std::dec << file->unk30_count << " -> 0x" << std::hex << file->unk30_offset << "\n"
				;

			asmout
				<< "nfo\n"
				<< "Unk10:   " << std::hex << file->size << " (" << sizeof(file->size) << ")\n"
				<< "Unk16:   " << std::hex << file->unk16 << " (" << sizeof(file->unk16) << ")\n"
				<< "Unk1C:   " << std::hex << file->unk1C << " (" << sizeof(file->unk1C) << ")\n"
				<< "Unk22:   " << std::hex << file->unk22 << " (" << sizeof(file->unk22) << ")\n"
				<< "Unk26:   " << std::hex << file->unk26 << " (" << sizeof(file->unk26) << ")\n"
				<< "Unk28:   " << std::hex << file->unk28 << " (" << sizeof(file->unk28) << ")\n"
				<< "Unk2A:   " << std::hex << file->unk2A << " (" << sizeof(file->unk2A) << ")\n"
				<< "Unk34:   " << std::hex << file->unk34 << " (" << sizeof(file->unk34) << ")\n"
				<< "Unk38:   " << std::hex << file->unk38 << " (" << sizeof(file->unk38) << ")\n"
				<< "Unk3C:   " << std::hex << file->unk3C << " (" << sizeof(file->unk3C) << ")\n"
				<< "Unk48:   " << std::hex << file->unk48 << " (" << sizeof(file->unk48) << ")\n"
				;
		}


		auto* includes = reinterpret_cast<UINT64*>(file->magic + file->include_table);

		// usings
		for (size_t i = 0; i < file->includes_count; i++) {
			asmout << "#using " << ctx.ExtractScript(includes[i]) << ";\n";
		}

		// strings
		if (ctx.opt.m_strings && file->string_count) {
			asmout << "--- Strings (" << std::dec << file->string_count << ") ---\n";
			uintptr_t str_location = reinterpret_cast<uintptr_t>(file->magic) + file->string_table;

			for (size_t i = 0; i < file->string_count; i++) {

				const auto* str = reinterpret_cast<GSC_STRINGTABLE_ITEM*>(str_location);

				asmout << std::hex << "String addr:" << str->string << ", count:" << (int)str->num_address << " ";

				LPCH cstr = reinterpret_cast<LPCH>(&file->magic[str->string]);

				ctx.opt.AddString(file->name, cstr, str->num_address, str->type);

				asmout << '"' << cstr << "\"\n";

				asmout << "location(s): ";

				const auto* strings = reinterpret_cast<const UINT32*>(&str[1]);
				asmout << std::hex << strings[0];
				for (size_t j = 1; j < str->num_address; j++) {
					asmout << std::hex << "," << strings[j];
				}
				asmout << "\n";
				str_location += sizeof(*str) + sizeof(*strings) * str->num_address;
			}
		}


		// imports

		
		if (ctx.opt.m_imports && file->imports_count) {
			asmout << "--- Import (" << std::dec << file->imports_count << ") ---\n";
			uintptr_t imp_location = reinterpret_cast<uintptr_t>(file->magic) + file->import_table;

			for (size_t i = 0; i < file->imports_count; i++) {

				const auto* imp = reinterpret_cast<GSC_IMPORT_ITEM*>(imp_location);

				asmout << std::hex << ctx.Extract("namespace", imp->name_space) << "::" << std::flush << ctx.Extract("function", imp->name)
					<< " flags:" << (int)imp->flags << " params: " << std::dec << (int)imp->param_count << "\n";

				asmout << "location(s): ";

				const auto* imports = reinterpret_cast<const UINT32*>(&imp[1]);
				asmout << std::hex << imports[0];
				for (size_t j = 1; j < imp->num_address; j++) {
					asmout << std::hex << "," << imports[j];
				}
				asmout << "\n";
				imp_location += sizeof(*imp) + sizeof(*imports) * imp->num_address;
			}
		}

		if (ctx.opt.m_addresses) {
			if (file->unk2c_count) {
				asmout << "--- unk2c (" << std::dec << file->unk2c_count << ") ---\n";

				struct Unk2C {
					int count;
					int address;
				};

				auto* section2c = reinterpret_cast<Unk2C*>(file->magic + file->unk2c_offset);

				for (size_t i = 0; i < file->unk2c_count; i++) {
					// 0 int ???
					// 4 int address -> char*
					// 8 int[]
					asmout << std::dec << "count: " << section2c->count << "\n"
						<< "str: " << reinterpret_cast<LPCCH>(file->magic + section2c->address) << "\n"
						<< "add:";
					auto* addresses = reinterpret_cast<UINT32*>(section2c + 1);

					for (size_t i = 0; i < section2c->count; i++) {
						asmout << " " << std::hex << addresses[i];
					}
					asmout << "\n";
					section2c = reinterpret_cast<Unk2C*>(addresses + section2c->count);
				}
			}
			if (file->unk30_count) {
				asmout << "--- unk30 (" << std::dec << file->unk30_count << ") ---\n";

				struct Unk30 {
					int count;
					int address1;
					int address2;
				};

				auto* section30 = reinterpret_cast<Unk30*>(file->magic + file->unk30_offset);

				for (size_t i = 0; i < file->unk30_count; i++) {
					// 0 int ???
					// 4 int address -> char*
					// 8 int[]
					asmout << reinterpret_cast<LPCCH>(file->magic + section30->address1) << "%" << reinterpret_cast<LPCCH>(file->magic + section30->address2) << "\n"
						<< "add:";
					auto* addresses = reinterpret_cast<UINT32*>(section30 + 1);

					for (size_t i = 0; i < section30->count; i++) {
						asmout << " " << std::hex << addresses[i];
					}
					asmout << "\n";
					section30 = reinterpret_cast<Unk30*>(addresses + section30->count);
				}
			}
		}

		UINT64 nsp = 0;

		if (file->export_count) {
			//asmout << "--- Export (" << std::dec << file->export_count << ") -- - \n";

			auto* exports = reinterpret_cast<GSC_EXPORT_ITEM*>(file->magic + file->export_offset);

			for (size_t i = 0; i < file->export_count; i++) {
				const auto& exp = exports[i];

				if (nsp != exp.name_space) {
					asmout << "\n#namespace " << ctx.Extract("namespace", exp.name_space) << ";\n\n";
					nsp = exp.name_space;
				}

				// 0x98 SafeCreateLocalVariables
				// 0x43 CheckClearParams
				// 0x34 End
				// 81D1A30?

				ExportCTX ectx{ ctx, exp, *file };

				std::ofstream nullstream;
				nullstream.setstate(std::ios_base::badbit);

				// if we aren't dumping the ASM, we compute all the nodes first
				std::ostream& output = ctx.opt.m_dasm ? asmout : nullstream;

				ectx.DumpHeader(output);

				output << "\ngscasm {\n";
				ectx.HandleASM(output);
				output << "}\n";

				if (exp.param_count > ectx.params.size()) {
					std::cerr << "Bad param count\n";
					continue;
				}

				if (ectx.error) {
					continue;
				}
				if (ctx.opt.m_dcomp) {
					ectx.DumpHeader(asmout);
					asmout << "{}\n";
				}

			}
		}

		asmout.close();
		return tool::OK;
	}

	int gscinfo(Process& _, int argc, const char* argv[]) {
		if (argc < 4) {
			return tool::BAD_USAGE;
		}

		GscInfo23Option opt{};

		if (!opt.Compute(argv, 2, argc)) {
			return tool::BASIC_ERROR;
		}

		if (opt.m_help) {
			opt.PrintHelp(std::cout);
			return tool::OK;
		}

		std::vector<std::filesystem::path> files{};

		for (const auto* filePath : opt.m_inputFiles) {
			utils::GetFileRecurse(filePath, files);
		}

		if (files.empty()) {
			std::cerr << "No file\n";
			return tool::BASIC_ERROR;
		}

		hashutils::ReadDefaultFile(true, true);
		hashutils::SaveExtracted(true);


		if (opt.m_outputDir) {
			std::filesystem::create_directories(opt.m_outputDir);
		}

		Ctx23 ctx{ opt.m_outputDir, opt };

		for (const auto& path : files) {
			auto name = path.string();

			if (!(name.ends_with(".gscbin") || name.ends_with(".gscc") || name.ends_with(".cscbin") || name.ends_with(".cscc"))) {
				continue; // ignore bad files
			}

			LPVOID bufferAligned = NULL;
			size_t bufferSizeAligned = 0;

			LPVOID buffer = NULL;
			size_t bufferSize = 0;

			if (!utils::ReadFileAlign(path, buffer, bufferAligned, bufferSize, bufferSizeAligned)) {
				std::cerr << "Can't read file " << path << "\n";
				return tool::BASIC_ERROR;
			}

			auto out = handlefile(reinterpret_cast<GscObj23*>(bufferAligned), bufferSizeAligned, path, ctx);

			std::free(buffer);
			if (out != tool::OK) {
				return out;
			}
		}

		if (ctx.opt.m_dump_str) {
			std::ofstream ofs{ ctx.opt.m_dump_str, std::ios::out };

			if (ofs) {
				ofs << "script\ttype\trefs\tvalue\n";

				for (const auto& si : ctx.opt.m_dumpStrings) {
					ofs << hashutils::ExtractTmpScript(si.script) << "\t" << (int)si.type << "\t" << (int)si.ref << "\t" << si.str << "\n";
				}
			}
		}

		hashutils::WriteExtracted("hashes.txt");
		std::ofstream outHashes{ "hashes_all.txt" };
		if (outHashes) {
			for (const auto h : ctx.hashes) {
				outHashes << std::hex << h << "\n";
			}

			outHashes.close();
		}

		std::cout << "done.\n";

		return tool::OK;
	}

	enum AssetType : int {
		ASSET_PHYSICSLIBRARY = 0x0,
		ASSET_PHYSICSSFXEVEN = 0x1,
		ASSET_PHYSICSVFXEVEN = 0x2,
		ASSET_PHYSICSASSET = 0x3,
		ASSET_PHYSICSFXPIPEL = 0x4,
		ASSET_PHYSICSFXSHAPE = 0x5,
		ASSET_PHYSICSDEBUGDA = 0x6,
		ASSET_XANIM = 0x7,
		ASSET_XMODELSURFS = 0x8,
		ASSET_XMODEL = 0x9,
		ASSET_MAYHEM = 0xA,
		ASSET_MATERIAL = 0xB,
		ASSET_COMPUTESHADER = 0xC,
		ASSET_TILESHADER = 0xD,
		ASSET_LIBSHADER = 0xE,
		ASSET_VERTEXSHADER = 0xF,
		ASSET_HULLSHADER = 0x10,
		ASSET_DOMAINSHADER = 0x11,
		ASSET_PIXELSHADER = 0x12,
		ASSET_SERIALIZEDSHAD = 0x13,
		ASSET_TECHSET = 0x14,
		ASSET_IMAGE = 0x15,
		ASSET_SOUNDGLOBALVOL = 0x16,
		ASSET_SOUNDGLOBALENT = 0x17,
		ASSET_SOUNDGLOBALCON = 0x18,
		ASSET_SOUNDGLOBALWHI = 0x19,
		ASSET_SOUNDGLOBALBUL = 0x1A,
		ASSET_SOUNDGLOBALPER = 0x1B,
		ASSET_SOUNDGLOBALOCC = 0x1C,
		ASSET_SOUNDGLOBALSUR = 0x1D,
		ASSET_SOUNDGLOBALCUR = 0x1E,
		ASSET_SOUNDGLOBALDOP = 0x1F,
		ASSET_SOUNDGLOBALSHA = 0x20,
		ASSET_SOUNDGLOBALFUT = 0x21,
		ASSET_SOUNDGLOBALSEN = 0x22,
		ASSET_SOUNDGLOBALNAM = 0x23,
		ASSET_SOUNDGLOBALFAC = 0x24,
		ASSET_SOUNDBANK = 0x25,
		ASSET_SOUNDBANKMERGE = 0x26,
		ASSET_SOUNDBANKTRANS = 0x27,
		ASSET_COLMAP = 0x28,
		ASSET_COMMAP = 0x29,
		ASSET_GLASSMAP = 0x2A,
		ASSET_AIPATHS = 0x2B,
		ASSET_NAVMESH = 0x2C,
		ASSET_TACGRAPH = 0x2D,
		ASSET_AIREGIONGRAPHS = 0x2E,
		ASSET_MAPENTS = 0x2F,
		ASSET_MAPENTSTRZONE = 0x30,
		ASSET_FXMAP = 0x31,
		ASSET_GFXMAP = 0x32,
		ASSET_GFXMAPTRZONE = 0x33,
		ASSET_IESPROFILE = 0x34,
		ASSET_LIGHTDEF = 0x35,
		ASSET_GRADINGCLUT = 0x36,
		ASSET_FOGSPLINE = 0x37,
		ASSET_ANIMCLASS = 0x38,
		ASSET_PLAYERANIM = 0x39,
		ASSET_NK_8A41D00 = 0x3A,
		ASSET_LOCALIZE = 0x3B,
		ASSET_ATTACHMENT = 0x3C,
		ASSET_WEAPON = 0x3D,
		ASSET_VFX = 0x3E,
		ASSET_IMPACTFX = 0x3F,
		ASSET_SURFACEFX = 0x40,
		ASSET_AITYPE = 0x41,
		ASSET_CHARACTER = 0x42,
		ASSET_XMODELALIAS = 0x43,
		ASSET_RAWFILE = 0x44,
		ASSET_GSCOBJ = 0x45,
		ASSET_GSCGDB = 0x46,
		ASSET_STRINGTABLE_0 = 0x47,
		ASSET_DDL = 0x48,
		ASSET_TRACER = 0x49,
		ASSET_VEHICLE = 0x4A,
		ASSET_NETCONSTSTRING = 0x4B,
		ASSET_LUAFILE = 0x4C,
		ASSET_SCRIPTABLE = 0x4D,
		ASSET_VECTORFIELD = 0x4E,
		ASSET_PARTICLESIMANI = 0x4F,
		ASSET_STREAMINGINFO = 0x50,
		ASSET_LASER = 0x51,
		ASSET_GAMEPROPS = 0x52,
		ASSET_MATERIALSTANDA = 0x53,
		ASSET_BEAM = 0x54,
		ASSET_TTF = 0x55,
		ASSET_SUIT = 0x56,
		ASSET_SUITANIMPACKAG = 0x57,
		ASSET_CAMERA = 0x58,
		ASSET_HUDOUTLINE = 0x59,
		ASSET_RUMBLE = 0x5A,
		ASSET_RUMBLEGRAPH = 0x5B,
		ASSET_ANIMPKG = 0x5C,
		ASSET_SFXPKG = 0x5D,
		ASSET_VFXPKG = 0x5E,
		ASSET_FOOTSTEPVFX = 0x5F,
		ASSET_BEHAVIORTREE = 0x60,
		ASSET_BEHAVIORSEQUEN = 0x61,
		ASSET_SIGHTCONFIG = 0x62,
		ASSET_SIGHTCONFIGTEM = 0x63,
		ASSET_AIANIMSET = 0x64,
		ASSET_AIASM = 0x65,
		ASSET_PROCEDURALBONE = 0x66,
		ASSET_DYNAMICBONES = 0x67,
		ASSET_PROCEDURALBLEN = 0x68,
		ASSET_RETICLE_0 = 0x69,
		ASSET_XANIMCURVE = 0x6A,
		ASSET_COVERSELECTOR = 0x6B,
		ASSET_ENEMYSELECTOR = 0x6C,
		ASSET_CLIENTCHARACTE_1 = 0x6D,
		ASSET_CLOTHASSET = 0x6E,
		ASSET_CINEMATICMOTIO = 0x6F,
		ASSET_ACCESSORY = 0x70,
		ASSET_LOCDMGTABLE = 0x71,
		ASSET_BULLETPENETRAT = 0x72,
		ASSET_SCRIPTBUNDLE = 0x73,
		ASSET_BLENDSPACE2D = 0x74,
		ASSET_XCAM = 0x75,
		ASSET_CAMO_0 = 0x76,
		ASSET_XCOMPOSITEMODE = 0x77,
		ASSET_XMODELDETAILCO = 0x78,
		ASSET_STREAMTREEOVER = 0x79,
		ASSET_KEYVALUEPAIRS = 0x7A,
		ASSET_STTERRAIN = 0x7B,
		ASSET_VHMDATA = 0x7C,
		ASSET_VTMSURFACEMAPS = 0x7D,
		ASSET_NK_8A41CB0 = 0x7E,
		ASSET_EXECUTION = 0x7F,
		ASSET_CARRYOBJECT = 0x80,
		ASSET_SOUNDBANKLIST = 0x81,
		ASSET_WEAPONACCURACY = 0x82,
		ASSET_DECALVOLUMEMAT = 0x83,
		ASSET_DECALVOLUMEMAS = 0x84,
		ASSET_DYNENTITYLIST = 0x85,
		ASSET_FXMAPTRZONE = 0x86,
		ASSET_VOLUMETRICHEIG = 0x87,
		ASSET_DLOGSCHEMA = 0x88,
		ASSET_EDGELIST = 0x89,
		ASSET_STANDALONEUMBR = 0x8A,
		ASSET_XBONESET = 0x8B,
		ASSET_RAGDOLLASSET = 0x8C,
		ASSET_PHYSICSBONEGRA = 0x8D,
		ASSET_CURVE = 0x8E,
		ASSET_SKELETONCONSTR = 0x8F,
		ASSET_TRIGGEREFFECT = 0x90,
		ASSET_WEAPONTRIGGER = 0x91,
		ASSET_VOLUMETRICCLOU = 0x92,
		ASSET_CODCASTERDATA = 0x93,
		ASSET_WATERSYSTEM = 0x94,
		ASSET_WATERBUOYANCY = 0x95,
		ASSET_KEYBINDS_0 = 0x96,
		ASSET_CALLOUTMARKERP_1 = 0x97,
		ASSET_LIGHTSTATE = 0x98,
		ASSET_RADIANTTELEMET = 0x99,
		ASSET_AIMARKUP = 0x9A,
		ASSET_AIMARKUPGENERA = 0x9B,
		ASSET_SCENARIO = 0x9C,
		ASSET_AIINTERACTION = 0x9D,
		ASSET_MAPVOXELIZEDST = 0x9E,
		ASSET_WATERVIS = 0x9F,
		ASSET_GAMETYPE_0 = 0xA0,
		ASSET_GAMETYPETABLE = 0xA1,
		ASSET_SNDMODIFIER = 0xA2,
		ASSET_WEAPONBLUEPRIN = 0xA3,
		ASSET_ATTACHMENTBLUE = 0xA4,
		ASSET_MOVINGPLATFORM = 0xA5,
		ASSET_HWCONFIG = 0xA6,
		ASSET_TIMELAPSESKY = 0xA7,
		ASSET_HWCONFIGURATOR = 0xA8,
		ASSET_OBJECTIVEDATA = 0xA9,
		ASSET_FONT = 0xAA,
		ASSET_MOTIONMATCHING = 0xAB,
		ASSET_MOTIONMATCHING_0 = 0xAC,
		ASSET_GAMETYPESTATDA = 0xAD,
		ASSET_FONTICON = 0xAE,
		ASSET_CALLOUTMARKERP_2 = 0xAF,
		ASSET_HWCONFIGVAR = 0xB0,
		ASSET_ZIVART = 0xB1,
		ASSET_MOVIE = 0xB2,
		ASSET_MAPINFO = 0xB3,
		ASSET_MAPTABLE = 0xB4,
		ASSET_ACHIEVEMENT_0 = 0xB5,
		ASSET_ACHIEVEMENTLIS = 0xB6,
		ASSET_MATERIALDEBUGD = 0xB7,
		ASSET_SCRIPTABLEVARI = 0xB8,
		ASSET_LEAGUEPLAYSEAS = 0xB9,
		ASSET_SETTINGCONTEXT = 0xBA,
		ASSET_AIEVENTLIST = 0xBB,
		ASSET_SOUNDEVENT = 0xBC,
		ASSET_CALLOUTMARKERP_3 = 0xBD,
		ASSET_PROJECT = 0xBE,
		ASSET_PROJECTTABLE = 0xBF,
		ASSET_GAMEMODE = 0xC0,
		ASSET_SNDASSET = 0xC1,
		ASSET_GFXUMBRATOME = 0xC2,
		ASSET_AUDIOVISUALIZE = 0xC3,
		ASSET_MATERIALANIMAT_0 = 0xC4,
		ASSET_NAMEPLATESETTI = 0xC5,
		ASSET_REACTIVEAUDIOP = 0xC6,
		ASSET_REACTIVEVFXPAC = 0xC7,
		ASSET_MATERIALSFXTAB = 0xC8,
		ASSET_FOOTSTEPSFXTAB = 0xC9,
		ASSET_REACTIVESTAGES = 0xCA,
		ASSET_FOLIAGESFXTABL = 0xCB,
		ASSET_IMPACTSFXTABLE = 0xCC,
		ASSET_AIIMPACTVFXTAB = 0xCD,
		ASSET_TYPEINFO = 0xCE,
		ASSET_HANDPLANTSFXTA = 0xCF,
		ASSET_SNDTABLE = 0xD0,
		ASSET_EQUIPMENTSFX = 0xD1,
		ASSET_SOUNDSUBMIX = 0xD2,
		ASSET_SHOCK = 0xD3,
		ASSET_STORAGEFILE = 0xD4,
		ASSET_ECSASSET = 0xD5,
		ASSET_TRACKERFOOTSTE = 0xD6,
		ASSET_PLAYERSPAWNSET = 0xD7,
		ASSET_PLAYERSPAWNINF = 0xD8,
		ASSET_SOUNDSPEAKERMA = 0xD9,
		ASSET_REVERBPRESET = 0xDA,
		ASSET_AISHOOTSTYLESL = 0xDB,
		ASSET_OPERATORLIST = 0xDC,
		ASSET_OPERATOR = 0xDD,
		ASSET_OPERATORSKIN = 0xDE,
		ASSET_DISMEMBERMENT = 0xDF,
		ASSET_CONVERSATION = 0xE0,
		ASSET_XANIMNODE = 0xE1,
		ASSET_SNDMODIFIERSET = 0xE2,
		ASSET_SNDCURVE = 0xE3,
		ASSET_TTLOS = 0xE4,
		ASSET_MATERIALTINTAN = 0xE5,
		ASSET_MATERIALUVANIM = 0xE6,
		ASSET_MATERIALCAMOAN = 0xE7,
		ASSET_MATERIALANIMAT = 0xE8,
		ASSET_IMPACTFXTABLE = 0xE9,
		ASSET_IMPACTTYPETOIM = 0xEA,
		ASSET_REACTIVEOPERAT = 0xEB,
		ASSET_WEATHERVOLUME = 0xEC,
		ASSET_VEHICLETRICK = 0xED,
		ASSET_REACTIVEAUDIOP_2 = 0xEE,
		ASSET_AMBIENTSFXPACK = 0xEF,
		ASSET_OBJECTSTOREPRO = 0xF0,
		ASSET_OBJECTSTOREGAM = 0xF1,
		ASSET_PROCEDURALBONE_2 = 0xF2,
		ASSET_HWCONFIGVARGRO = 0xF3,
		ASSET_HWCONFIGTIERED = 0xF4,
		ASSET_SNDMASTERPRESE = 0xF5,
		ASSET_GENERICBLUEPRI = 0xF6,
		ASSET_COUNT = 0xF7,
	};


	enum StringTableCellType : byte {
		STT_UNK_1_64 = 1, // string
		STT_UNK_9_64 = 9, // string2??
		STT_UNK_A_64 = 0xA, // string3??

		STT_UNK_2_64 = 2, // int??
		STT_UNK_5_64 = 5, // hash (0xCBF29CE484222325/0x100000001B3)
		STT_UNK_6_64 = 6, // hash (0x47F5817A5EF961BA/0x100000001B3)
		STT_UNK_7_64 = 7, // ?

		STT_UNK_3_32 = 3, // float??
		STT_UNK_8_32 = 8, // ?

		STT_BYTE = 4,
	};

	struct StringTableResult {
		byte* result; // 0
		int unk8; // 8
		StringTableCellType type; // 12
	};

	struct StringTableColumn {
		StringTableCellType type;
		uint16_t* unk8;
		uint16_t* rowindexes;
		uint64_t unk18;
		byte* rowdata;
	};

	struct StringTable {
		uint64_t name;
		int columnCount;
		int rowCount;
		uint64_t cellIndices;
		StringTableColumn* columns;
		uint64_t strings;
	};

	void DumpStringTable(void* ptr, UINT64 name) {
		auto nameFormat = std::format("stringtable_{:x}.csv", name);

		// change here the dump location dir
		std::filesystem::path outPath = std::filesystem::path{ "csv" } / nameFormat;

		auto* st = reinterpret_cast<StringTable*>(ptr);

		if (!st->rowCount || !st->columnCount || !st->columns) {
			return; // empty, ignore
		}

		std::ofstream os{ outPath, std::ios::out };

		if (!os) {
			return; // wtf?
		}

		for (size_t i = 0; i < st->rowCount; i++) {
			for (size_t j = 0; j < st->columnCount; j++) {
				if (j) {
					os << ",";
				}
				auto& columns = st->columns[j];

				auto rowIndex = columns.rowindexes[i];

				int elemSize;
				switch (columns.type) {
				case STT_UNK_1_64:
				case STT_UNK_9_64:
				case STT_UNK_A_64:
				case STT_UNK_2_64:
				case STT_UNK_5_64:
				case STT_UNK_6_64:
				case STT_UNK_7_64:
					elemSize = 8;
					break;
				case STT_UNK_3_32:
				case STT_UNK_8_32:
					elemSize = 4;
					break;
				case STT_BYTE:
					elemSize = 1;
					break;
				default:
					elemSize = 0;
					break;
				}
				if (!elemSize) {
					os << "<badtype:" << std::hex << columns.type << ">";
					continue; // wtf?
				}

				auto* value = &columns.rowdata[elemSize * rowIndex];

				switch (columns.type) {
					/*
					
	STT_UNK_1_64 = 1, // string
	STT_UNK_9_64 = 9, // string2??
	STT_UNK_A_64 = 0xA, // string3??

	STT_UNK_2_64 = 2, // int??
	STT_UNK_5_64 = 5, // hash (0xCBF29CE484222325/0x100000001B3)
	STT_UNK_6_64 = 6, // hash (0x47F5817A5EF961BA/0x100000001B3)
	STT_UNK_7_64 = 7, // ?

	STT_UNK_3_32 = 3, // float??
	STT_UNK_8_32 = 8, // ?

	STT_BYTE = 4,
					
					*/
				case STT_UNK_1_64:
				case STT_UNK_9_64:
				case STT_UNK_A_64:
					// strings??
					os << *reinterpret_cast<const char**>(value);
					break;
				case STT_UNK_2_64:
					// int?
					os << std::dec << *reinterpret_cast<INT64*>(value);
					break;
				case STT_UNK_5_64:
				case STT_UNK_6_64:
					os << "hash_" << std::hex << *reinterpret_cast<UINT64*>(value);
					break;
				case STT_UNK_7_64:
					os << "7?" << std::hex << *reinterpret_cast<UINT64*>(value);
					break;
				case STT_UNK_3_32:
					os << *reinterpret_cast<float*>(value);
					break;
				case STT_UNK_8_32:
					os << "8?" << std::hex << *reinterpret_cast<UINT32*>(value);
					break;
				case STT_BYTE:
					os << (*value ? "TRUE" : "FALSE");
					break;
				default:
					os << "<error>";
					break;
				}
			}

			os << "\n";
		}

		os.close();
	}
	struct DBAssetPool {
		uintptr_t* m_entries;
		uintptr_t* m_freeHead;
		int m_poolSize;
		int m_elementSize;
		int m_usedCount;
		int m_maxUsedCount;
	};

#define DB_GetAssetEntryPool(type) ((DBAssetPool*)0x12345678)

	void DumpPoolValues() {

		std::ofstream os{ "pools.csv", std::ios::out };

		if (!os) return;

		os << "id,count,max,poolSize,size\n";

		for (size_t i = 0; i < 0xEE; i++) {
			auto* pool = DB_GetAssetEntryPool(i);
			os
				<< std::hex << i << ","
				<< std::dec
				<< pool->m_usedCount << ","
				<< pool->m_maxUsedCount << ","
				<< pool->m_poolSize << ","
				<< std::hex << pool->m_elementSize << "\n";
		}

		os.close();
	}

	struct LocalizeEntry {
		UINT64 name;
		LPCCH str;
	};

	void DumpLocalizeValues() {
		std::ofstream os{ "localize.csv", std::ios::out };
		std::ofstream osbin{ "localize.csvbin", std::ios::out | std::ios::binary };

		if (!os || !osbin) {
			if (os) os.close();
			if (osbin) osbin.close();
			return;
		}

		os << "name,string\n";

		auto* pool = DB_GetAssetEntryPool(AssetType::ASSET_LOCALIZE);

		for (size_t i = 0; i < pool->m_usedCount; i++) {
			auto& entry = reinterpret_cast<LocalizeEntry*>(pool->m_entries)[i];
			if (*entry.str && (*entry.str & 0xC0) == 0x80) {
				// decrypted
				os
					<< "hash_" << std::hex << entry.name << ","
					<< entry.str << "\n";
			}
			else {
				osbin.write(reinterpret_cast<const char*>(&entry.name), sizeof(entry.name));
				osbin.write(entry.str, 400);
			}
		}

		os.close();
		osbin.close();
	}



	int decryptlocalize(Process& _, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		std::filesystem::path file{ argv[2] };

		std::cout << "decrypting " << file.string() << "\n";

		LPVOID bufferPtr = nullptr;
		size_t size = 0;

		if (!utils::ReadFileNotAlign(file, bufferPtr, size)) {
			std::cerr << "can't read file.\n";
			return tool::BASIC_ERROR;
		}

		BYTE* buffer = reinterpret_cast<BYTE*>(bufferPtr);

		size_t location = 0;

		auto f = file.parent_path() / std::format("decrypt_{}.csv", file.filename().string());

		std::cout << "into " << f.string() << "\n";

		std::ofstream os{ f };

		if (!os) {
			std::free(bufferPtr);
			std::cerr << "can't write decrypt file.\n";
			return tool::BASIC_ERROR;
		}
		hashutils::ReadDefaultFile(true, true);

		os << "name,string\n";

		while (location < size) {
			auto name = *reinterpret_cast<UINT64*>(&buffer[location]);
			auto* dec = decrypt::DecryptStringIW(reinterpret_cast<char*>(&buffer[location + 8]));
			if (!dec) {
				continue;
			}
			os << hashutils::ExtractTmp("hash", name) 
				<< ","
				<< dec
				<< "\n";
			location += 408;
		}

		std::free(bufferPtr);

		return tool::OK;
	}

	int hash23(Process& _, int argc, const char* argv[]) {
		for (size_t i = 2; i < argc; i++) {

			std::cout << "--- " << argv[i] << "\n";


			UINT64 methods[20][2] = {
				{ 0xcbf29ce484222325LL, 0x100000001b3 },
				{ 0x47F5817A5EF961BALL, 0x100000001b3 },
				{ 0x79D6530B0BB9B5D1LL, 0x10000000233 }
			};

			for (size_t j = 0; j < sizeof(methods) / sizeof(methods[0]); j++) {

				std::cout 
					<< std::hex 
					<< methods[j][0] << "/" << methods[j][1] << " -> "
					<< hashutils::Hash64(argv[i], methods[j][0], methods[j][1])
					<< "\n";

			}

		}

		return tool::OK;
	}
}

ADD_TOOL("gscinfo23", " [file]", "gsc info 23", nullptr, gscinfo);
ADD_TOOL("local23", " [file]", "decrypt local dump 23", nullptr, decryptlocalize);
ADD_TOOL("hash23", " [str]", "hash using iw values", nullptr, hash23);
