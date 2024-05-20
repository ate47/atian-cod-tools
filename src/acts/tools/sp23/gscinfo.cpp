#include <includes.hpp>
#include "tools/gsc.hpp"

// tests with mwiii
namespace {

	struct StringInfo {
		uint64_t script;
		std::string str;
		byte ref;
		byte type;
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
		const char* m_dump_str{};
		const char* m_outputDir{};
		const char* m_copyright{};


		std::vector<const char*> m_inputFiles{};
		std::vector<StringInfo> m_dumpStrings{};

		bool Compute(const char** args, INT startIndex, INT endIndex) {
			// default values
			for (size_t i = startIndex; i < endIndex; i++) {
				const char* arg = args[i];

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
						LOG_ERROR("Missing value for param: {}!", arg);
						return false;
					}
					m_dump_str = args[++i];
				}
				else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
					if (i + 1 == endIndex) {
						LOG_ERROR("Missing value for param: {}!", arg);
						return false;
					}
					m_outputDir = args[++i];
				}
				else if (!strcmp("-C", arg) || !_strcmpi("--copyright", arg)) {
					if (i + 1 == endIndex) {
						LOG_ERROR("Missing value for param: {}!", arg);
						return false;
					}
					m_copyright = args[++i];
				}

				else if (*arg == '-') {
					LOG_ERROR("Unknown option: {}!", arg);
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
			LOG_INFO("-h --help          : Print help");
			LOG_INFO("-g --gsc           : Produce GSC");
			LOG_INFO("-a --asm           : Produce ASM");
			LOG_INFO("-o --output [d]    : ASM/GSC output dir, default same.gscasm");
			LOG_INFO("-s --silent        : Silent output, only errors");
			LOG_INFO("-H --header        : Write file header");
			LOG_INFO("-l --rloc          : Write relative location of the function code");
			LOG_INFO("-I --imports       : Write imports");
			LOG_INFO("-S --strings       : Write strings");
			LOG_INFO("-G --gvars         : Write gvars");
			LOG_INFO("-A --addresses     : Element addresses");
			LOG_INFO("-C --copyright [t] : Set a comment text to put in front of every file");
		}

		void AddString(uint64_t script, const char* str, byte count, byte type) {
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

		uint64_t GetMagic() {
			return *reinterpret_cast<uint64_t*>(magic);
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
		const char* outputDir;
		GscInfo23Option& opt;
		std::unordered_set<uint64_t> hashes{};


		const char* Extract(const char* type, uint64_t hash) {
			hashes.insert(hash);
			return hashutils::ExtractTmp(type, hash);
		}
		const char* ExtractScript(uint64_t hash) {
			hashes.insert(hash);
			return hashutils::ExtractTmpScript(hash);
		}
	};

	struct ExportCTX {
		Ctx23& ctx;
		const GSC_EXPORT_ITEM& exp;
		GscObj23& file;
		std::vector<uint64_t> params{};
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

		uint32_t RLoc() {
			return (uint32_t)(bytecode - file.magic);
		}

		std::ostream& WriteLocation(std::ostream& out) {
			return out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << RLoc() << ": ";
		}

		std::ostream& WriteOperation(std::ostream& out, byte code, const char* name) {
			return WriteLocation(out) << std::hex << std::setfill('0') << std::setw(sizeof(byte) << 1) << (int) code
				<< " "
				<< std::setfill(' ') << std::setw(25) << std::left << name << std::right
				<< " ";
		}
		std::ostream& WritePadding(std::ostream& out) {
			return out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << RLoc() << ": "
				// no opcode write
				<< std::setfill(' ') << std::setw((sizeof(byte) << 1) + 25 + 2) << " ";
		}

		void HandleASM(std::ostream& asmout) {
			bytecode = file.magic + exp.address;
			while (true) {
				auto opcode = *(bytecode++);
				switch (opcode) {
				case 0x57: {
					auto p = *(uint32_t*)(bytecode);
					bytecode += 4;

					WriteOperation(asmout, opcode, "unk") << std::dec << p << "\n";


					break;
				}
				case 0x70: {
					auto p = *(uint32_t*)(bytecode);
					bytecode += 4;

					WriteOperation(asmout, opcode, "unk") << std::dec << p << "\n";


					break;
				}
				case 0:
				case 0x83:{
					// one is jump, one devblock?

					auto delta = *(int16_t*)(bytecode);
					bytecode += 2;

					WriteOperation(asmout, opcode, "unk") << std::hex << (delta < 0 ? "-" : "") << (delta < 0 ? -delta : delta) << "\n";
					break;
				}
				case 0x16:
				case 0x61:
				case 0x62: {
					// no bytecode operations
					WriteOperation(asmout, opcode, "unk") << "\n";
					break;
				}
				case 0x2B:
				case 0xA4: {
					auto v = *(uint64_t*)(bytecode);
					bytecode += 8;

					WriteOperation(asmout, opcode, "RegisterVar") << hashutils::ExtractTmp("hash", v) << "\n";
					params.push_back(v);

					break;
				}
				case 0x44: {
					auto v = *(uint64_t*)(bytecode);
					bytecode += 8;

					WriteOperation(asmout, opcode, "ClearVar") << hashutils::ExtractTmp("hash", v) << "\n";
					//params.push_back(v); // todo?

					break;
				}
				case 0x98: { // create params
					auto p = *(bytecode++);
					if (p < exp.param_count) {
						asmout << "Bad param count, excepted " << (int)exp.param_count << " had " << (int)p << "\n";
						error = true;
						break;
					}

					WriteOperation(asmout, opcode, "AllocateParams") << std::dec << (int)p << " names:\n";

					for (size_t i = 0; i < p; i++) {
						auto varname = *reinterpret_cast<uint64_t*>(bytecode);
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
					asmout << "Unknown bytecode operation at location 0x" << (int)opcode << "\n";
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
			LOG_ERROR("Bad magic for {}", path.string());
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


		LOG_INFO("Parsing {} to {}...", path.string(), outFile.string());

		std::filesystem::create_directories(outFile.parent_path());

		std::ofstream asmout{ outFile };

		if (!asmout) {
			LOG_ERROR("Can't create file {}", outFile.string());
			return tool::BASIC_ERROR;
		}

		if (ctx.opt.m_header) {
			asmout
				<< "// Name:     " << ctx.ExtractScript(file->name) << "(" << path << ")\n"
				<< "// magic: 0x" << std::hex << file->GetMagic() << " Size:     0x" << std::hex << size << "\n"
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


		auto* includes = reinterpret_cast<uint64_t*>(file->magic + file->include_table);

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

				char* cstr = reinterpret_cast<char*>(&file->magic[str->string]);

				ctx.opt.AddString(file->name, cstr, str->num_address, str->type);

				asmout << '"' << cstr << "\"\n";

				asmout << "location(s): ";

				const auto* strings = reinterpret_cast<const uint32_t*>(&str[1]);
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

				const auto* imports = reinterpret_cast<const uint32_t*>(&imp[1]);
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
						<< "str: " << reinterpret_cast<const char*>(file->magic + section2c->address) << "\n"
						<< "add:";
					auto* addresses = reinterpret_cast<uint32_t*>(section2c + 1);

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
					asmout << reinterpret_cast<const char*>(file->magic + section30->address1) << "%" << reinterpret_cast<const char*>(file->magic + section30->address2) << "\n"
						<< "add:";
					auto* addresses = reinterpret_cast<uint32_t*>(section30 + 1);

					for (size_t i = 0; i < section30->count; i++) {
						asmout << " " << std::hex << addresses[i];
					}
					asmout << "\n";
					section30 = reinterpret_cast<Unk30*>(addresses + section30->count);
				}
			}
		}

		uint64_t nsp = 0;

		if (file->export_count) {
			//asmout << "--- Export (" << std::dec << file->export_count << ") -- - \n";

			auto* exports = reinterpret_cast<GSC_EXPORT_ITEM*>(file->magic + file->export_offset);

			for (size_t i = 0; i < file->export_count; i++) {
				const auto& exp = exports[i];

				if (nsp != exp.name_space) {
					asmout << "\n#namespace " << ctx.Extract("namespace", exp.name_space) << ";\n\n";
					nsp = exp.name_space;
				}

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
					LOG_ERROR("Bad param count");
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
			opt.PrintHelp();
			return tool::OK;
		}

		std::vector<std::filesystem::path> files{};

		for (const auto* filePath : opt.m_inputFiles) {
			utils::GetFileRecurse(filePath, files);
		}

		if (files.empty()) {
			LOG_ERROR("No file");
			return tool::BASIC_ERROR;
		}

		hashutils::ReadDefaultFile();
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

			void* bufferAligned{};
			size_t bufferSizeAligned = 0;

			void* buffer{};
			size_t bufferSize = 0;

			if (!utils::ReadFileAlign(path, buffer, bufferAligned, bufferSize, bufferSizeAligned)) {
				LOG_ERROR("Can't read file {}", path.string());
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

		LOG_INFO("Done");

		return tool::OK;
	}

}

// use gscinfo for jup
//ADD_TOOL("gscinfo23", "mwiii", " [file]", "gsc info 23", nullptr, gscinfo);
