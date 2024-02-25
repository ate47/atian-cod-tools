#include <includes.hpp>
#include "tools/gsc.hpp"
#include "tools/dump.hpp"
#include <t8_internal.hpp>
#include <decrypt.hpp>
#include <offsets.hpp>
#include <scriptinstance.hpp>

using namespace t8internal;

class VmDebugOption {
public:
	bool m_help = false;
	bool m_stack = false;
	bool m_gvars = false;
	bool m_pretty = false;
	bool m_dumpstring = false;
	bool m_vars = false;
	bool m_archived = false;
	int m_deep_struct = 0;
	int m_deep_array = 0;

	bool Compute(LPCCH* args, INT startIndex, INT endIndex) {

		// default values
		for (size_t i = startIndex; i < endIndex; i++) {
			LPCCH arg = args[i];

			if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
				m_help = true;
			}
			else if (!strcmp("-s", arg) || !_strcmpi("--stack", arg)) {
				m_stack = true;
			}
			else if (!strcmp("-v", arg) || !_strcmpi("--vars", arg)) {
				m_vars = true;
			}
			else if (!strcmp("-g", arg) || !_strcmpi("--gvars", arg)) {
				m_gvars = true;
			}
			else if (!strcmp("-p", arg) || !_strcmpi("--pretty", arg)) {
				m_pretty = true;
			}
			else if (!_strcmpi("--dstr", arg)) {
				m_dumpstring = true;
			}
			else if (!strcmp("-a", arg) || !_strcmpi("--archived", arg)) {
				m_archived = true;
			}
			else if (!strcmp("-S", arg) || !_strcmpi("--struct-deep", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				m_deep_struct = std::atoi(args[++i]);
			}
			else if (!strcmp("-A", arg) || !_strcmpi("--array-deep", arg)) {
				if (i + 1 == endIndex) {
					std::cerr << "Missing value for param: " << arg << "!\n";
					return false;
				}
				m_deep_array = std::atoi(args[++i]);
			}
		}
		return true;
	}
	void PrintHelp(std::ostream& out) {
		out << "-h --help            : Print help\n"
			<< "-s --stack           : Print stack\n"
			<< "-v --vars            : Print local variables\n"
			<< "-g --gvars           : Print global variables\n"
			<< "-p --pretty          : Format struct/array output\n"
			<< "-S --struct-deep [d] : Struct max depth\n"
			<< "-A --array-deep [d]  : Array max depth\n"
			<< "-a --archived        : Show archived stacks\n"
			<< "--dstr               : Dump strings\n"
			;
	}
};

struct __declspec(align(8)) scrVarGlob {
	uintptr_t scriptNameSearchHashList;//ScrVarIndex*
	uintptr_t scriptVariables;//ScrVarRef*
	uintptr_t scriptVariablesObjectInfo1;//ScrVarObjectInfo1* 8 bytes
	uintptr_t scriptVariablesObjectInfo2;//ScrVarObjectInfo2* 4 bytes
	uintptr_t scriptValues;//ScrVarValue_t*


	bool GetEntityRef(ScrEntityRef& ref, const Process& proc, UINT32 entId) {
		UnionAll32 all;
		if (!proc.ReadMemory(&all, scriptVariablesObjectInfo2 + entId * 4, 4)) {
			return false;
		}
		ref.u.b32 = all;
		ref.classnum = (ClassNum)all.entVar.classnum;
		ref.client = (LocalClientNum)all.entVar.clientNum;
		return true;
	}
};


struct ByteCodeLocationInfo {
	tool::gsc::T8GSCExport exp;
	tool::gsc::T8GSCOBJ obj;
	tool::gsc::T8GSCOBJ* script;
	BYTE* scriptData;
	UINT32 functionRLoc;
	UINT32 scriptRLoc;
};


bool FindByteCodeLocationInfo(int inst, const Process& proc, ByteCodeLocationInfo& info, uintptr_t loc, bool allocScript = false) {
	using namespace tool::dump;
	using namespace tool::gsc;

	T8ObjFileInfo buffer[650];
	UINT32 bufferCount = proc.ReadMemory<UINT32>(proc[offset::gObjFileInfoCount] + sizeof(bufferCount) * inst);

	if (!bufferCount || !proc.ReadMemory(&buffer[0], proc[offset::gObjFileInfo] + (inst * (sizeof(*buffer) * 650)), sizeof(*buffer) * 650)) {
		std::cerr << "Can't read linked data " << bufferCount << "\n";
		return false;
	}

	uintptr_t candidate = 0;

	for (size_t i = 0; i < bufferCount; i++) {
		auto& nfo = buffer[i];
		if (nfo.activeVersion > loc) {
			continue; // after our location
		}

		if (nfo.activeVersion > candidate) {
			// after the smaller candidate, it can be this script
			candidate = nfo.activeVersion;
		}
	}
	if (!candidate) {
		std::cerr << "Can't find script candidate\n";
		return false;
	}

	if (!proc.ReadMemory(&info.obj, candidate, sizeof(info.obj))) {
		std::cerr << "Can't read header\n";
		return false;
	}

	if (candidate + info.obj.script_size < loc) {
		std::cerr << "Not inside candidate\n";
		return false; // not a good location?
	}

	info.scriptData = (BYTE*) std::malloc((size_t) info.obj.script_size + 0xF);
	info.script = reinterpret_cast<T8GSCOBJ*>(utils::Aligned<UINT64>(info.scriptData));

	if (!info.script || !proc.ReadMemory(info.script, candidate, info.obj.script_size)) {
		std::cerr << "Can't read GSC file at " << std::hex << candidate << "\n";
		std::free(info.script);
		return false;
	}
	
	auto* exports = reinterpret_cast<T8GSCExport*>(&info.script->magic[info.obj.export_table_offset]);

	size_t exploc = 0;
	for (size_t i = 1; i < info.obj.exports_count; i++) {
		const auto& exp = exports[i];
		auto startLoc = candidate + exp.address;
		if (startLoc > loc) {
			continue; // after our location
		}
		if (exports[exploc].address < exp.address) {
			exploc = i; // the previous candidate was before the new one
		}
	}

	info.exp = exports[exploc];
	info.scriptRLoc = (UINT32)(loc - candidate);
	info.functionRLoc = info.scriptRLoc - info.exp.address;

	// we keep the script if asked
	if (!allocScript) {
		std::free(info.scriptData);
		info.script = nullptr;
		info.scriptData = nullptr;
	}

	return true;
}

std::ostream& GetScrVarInfo(std::ostream& out, int inst, const Process& proc, ScrVar& var, scrVarGlob& glob, const VmDebugOption& opt, int deep = 0);

std::ostream& GetScrVarInfoPtr(std::ostream& out, int inst, const Process& proc, ScrVar& ptrvar, UINT32 valId, scrVarGlob& glob, const VmDebugOption& opt, int deep = 0) {
	switch (ptrvar.type) {
	case TYPE_SHARED_STRUCT:
	case TYPE_ENTITY:
	case TYPE_ARRAY:
	case TYPE_STRUCT:
	{
		switch (ptrvar.type) {
		case TYPE_ARRAY:
			out << "[";
			break;
		case TYPE_STRUCT:
			out << "{";
			break;
		case TYPE_SHARED_STRUCT:
			out << "Shared{";
			break;
		case TYPE_ENTITY:
			out << "Entity{";
			break;
		default:
			out << "Error{";
			break;
			break;
		}
		ScrVarObjectInfo1 info;
		ScrVarRef ref;
		UINT32 refLoc = ptrvar.value.ui;
		if (!proc.ReadMemory(&info, glob.scriptVariablesObjectInfo1 + sizeof(ScrVarObjectInfo1) * valId, sizeof(ScrVarObjectInfo1))) {
			out << "error_struct1:" << valId;
		}
		else if (info.ui) { // info.ui = size
			if ((ptrvar.type == TYPE_ARRAY && deep <= opt.m_deep_array) || (ptrvar.type != TYPE_ARRAY && deep <= opt.m_deep_struct)) {
				if (!proc.ReadMemory(&ref, glob.scriptVariables + sizeof(ScrVarRef) * refLoc, sizeof(ScrVarRef))) {
					out << "error_struct2:" << valId;
				}
				ScrVar field;
				while (true) {
					if (opt.m_pretty) {
						out << "\n";
						utils::Padding(out, deep + 1);
					}
					if (ptrvar.type == TYPE_STRUCT) {
						out << "#" << hashutils::ExtractTmp("var", ref.nameIndex);
					}
					else {
						if (ref.nameIndex > 0xFFFFFFFF) {
							out << "#\"" << hashutils::ExtractTmp("hash", ref.nameIndex) << "\"";
						}
						else {
							out << std::dec << ref.nameIndex;
						}
					}
					out << std::flush << ":";
					if (!proc.ReadMemory(&field, glob.scriptValues + sizeof(ScrVar) * refLoc, sizeof(ScrVar))) {
						out << "error_struct3" << std::hex << refLoc;
					}
					else {
						GetScrVarInfo(out, inst, proc, field, glob, opt, deep + 1);
					}

					if (!ref.nextSibling) {
						// end
						break;
					}
					refLoc = ref.nextSibling;
					if (!proc.ReadMemory(&ref, glob.scriptVariables + sizeof(ScrVarRef) * refLoc, sizeof(ScrVarRef))) {
						out << "error_struct4:" << ref.nextSibling << "/" << ptrvar.value.ui;
						break;
					}
					out << ", ";
				}
				if (opt.m_pretty) {
					out << "\n";
					utils::Padding(out, deep);
				}
			}
			else {
				// only write this if we have at least one element
				out << " ... ";
			}
		}
		out << (ptrvar.type == TYPE_ARRAY ? "]" : "}");
	}
	break;
	default:
		return GetScrVarInfo(out, inst, proc, ptrvar, glob, opt, deep + 1);
	}
	return out;
}

std::ostream& GetScrVarInfo(std::ostream& out, int inst, const Process& proc, ScrVar& var, scrVarGlob& glob, const VmDebugOption& opt, int deep) {
	switch (var.type) {
	case TYPE_HASH:
		out << "#\"" << hashutils::ExtractTmp("hash", var.value.ull) << "\"" << std::flush;
		break;
	case TYPE_FLOAT:
		out << var.value.f;
		break;
	case TYPE_UNDEFINED:
		out << "undefined";
		break;
	case TYPE_INTEGER:
		out << std::dec << var.value.ll;
		break;
	case TYPE_STRING:
	{
		static CHAR str_read[0x2001];
		auto strptr = proc.ReadMemory<uintptr_t>(proc[offset::mt_buffer]) + (UINT32)(0x14 * var.value.i);
		if (!proc.ReadMemory<INT16>(strptr) || proc.ReadMemory<BYTE>(strptr + 3) != 7) {
			out << "badstring0:" << (int) proc.ReadMemory<BYTE>(strptr + 3);
		}
		else if (!strptr || proc.ReadString(str_read, strptr + 0x10, 0x2001) < 0) {
			out << "badstring1:" << (strptr + 0x10);
		}
		else {
			auto flag = proc.ReadMemory<CHAR>(strptr + 2);
			LPCCH strDec;
			if ((flag & 0x40) || flag >= 0) {
				// not encrypted
				strDec = str_read;
			}
			else {
				strDec = decrypt::DecryptString(str_read);
			}

			out << "\"" << strDec<< "\"";

			if (opt.m_dumpstring) {
				std::cout << strDec << "\n";
			}
		}
	}
		break;
	case TYPE_UINTPTR:
	case TYPE_API_FUNCTION:
		out << std::hex << ScrVarTypeName(var.type) << ":" << var.value.ull;
		break;
	case TYPE_VECTOR:
	{
		FLOAT vec[3] = {};
		if (!proc.ReadMemory(&vec[0], var.value.ptr, sizeof(FLOAT) * 3)) {
			out << std::hex << "vector_err:" << var.value.ptr;
		}
		else {
			out << "(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
		}
	}
		break;
	case TYPE_CODEPOS:
	case TYPE_PRECODEPOS:
	case TYPE_SCRIPT_FUNCTION:
	{
		ByteCodeLocationInfo info;
		if (!FindByteCodeLocationInfo(inst, proc, info, var.value.ull)) {
			out << ScrVarTypeName(var.type) << ":" << std::hex << var.value.ull;
		}
		else {
			if (var.type != TYPE_SCRIPT_FUNCTION) {
				out << ScrVarTypeName(var.type) << ":";
			}
			out << hashutils::ExtractTmp("namespace", info.exp.name_space) << "" << std::flush
				<< "<" << hashutils::ExtractTmpScript(info.obj.name) << ">::" << std::flush
				<< hashutils::ExtractTmp("function", info.exp.name) << std::flush;
			if (var.type != TYPE_SCRIPT_FUNCTION) {
				out << "+" << std::hex << info.functionRLoc;
			}
		}
	}
		break;
	case TYPE_POINTER:
	{
		ScrVar ptrvar;
		if (!proc.ReadMemory(&ptrvar, glob.scriptValues + sizeof(ScrVar) * var.value.ui, sizeof(ScrVar))) {
			out << "ptrerror_err:" << std::hex << var.value.ui;
		}
		else {
			return GetScrVarInfoPtr(out, inst, proc, ptrvar, var.value.ui, glob, opt, deep);
		}
	}
		break;
	case TYPE_ENTITY_OFFSET:
	case TYPE_STACK:
	case TYPE_THREAD:
	case TYPE_NOTIFY_THREAD:
	case TYPE_TIME_THREAD:
	case TYPE_FRAME_THREAD:
	case TYPE_CHILD_THREAD:
	case TYPE_CLASS:
	case TYPE_SHARED_STRUCT:
	case TYPE_REMOVED_ENTITY:
	case TYPE_ENTITY:
	case TYPE_REMOVED_THREAD:
	case TYPE_FREE:
	case TYPE_THREAD_LIST:
		out << ScrVarTypeName(var.type) << std::hex << "(" << var.value.ull << ")";
		break;
	default:
		out << ScrVarTypeName(var.type) << std::hex << "(" << var.type << "/" << var.value.ull << ")";
		break;
	}
	return out;
}

int vmdebug(Process& proc, int argc, const char* argv[]) {
	VmDebugOption opt{};

	if (!opt.Compute(argv, 2, argc) || opt.m_help) {
		opt.PrintHelp(std::cout);
		return 0;
	}

	hashutils::ReadDefaultFile();
	auto vms = std::make_unique<scrVmPub[]>(2);
	

	if (!proc.ReadMemory(&vms[0], proc[offset::scrVmPub], sizeof(scrVmPub) * 2)) {
		std::cerr << "Can't read vm\n";
		return tool::BASIC_ERROR;
	}
	auto globs = std::make_unique<scrVarGlob[]>(2);
	if (!proc.ReadMemory(&globs[0], proc[offset::scrVarGlob], sizeof(scrVarGlob) * 2)) {
		std::cerr << "Can't read glob\n";
		return tool::BASIC_ERROR;
	}
	auto pubs = std::make_unique<scrVarPub[]>(2);
	if (!proc.ReadMemory(&pubs[0], proc[offset::gScrVarPub], sizeof(scrVarPub) * 2)) {
		std::cerr << "Can't read pub\n";
		return tool::BASIC_ERROR;
	}

	BYTE vmError = proc.ReadMemory<BYTE>(offset::scrVmError);
	
	if (vmError) {
		UINT32 vmErrorCode = proc.ReadMemory<UINT32>(offset::scrVmErrorCode);

		std::cout
			<< "VM error: " << std::dec << vmErrorCode << " (0x" << std::hex << vmErrorCode << "): ";

		switch (vmErrorCode) {
		case 1661383768:
		case 3559732046:
		case 2782299909:
		case 3456527393:
		case 325402485:
		case 655503056:
			std::cout << "Stack overflow" << "\n";
			break;
		case 754846421:
			std::cout << "Bad origin" << "\n";
			break;
		case 3014377421:
			std::cout << "Error linking script" << "\n";
			break;
		case 1670707254:
			std::cout << "Unresolved external" << "\n";
			break;
		default:
			std::cout << "Unknown error" << "\n";
			break;
		}
	}



	ByteCodeLocationInfo info;
	// sub:27775B0

	for (size_t inst = 0; inst < scriptinstance::SI_COUNT; inst++) {
		auto& vm = vms[inst];
		auto& glob = globs[inst];
		auto& pub = pubs[inst];

		std::cout
			<< std::dec
			<< "--- " << scriptinstance::Name(inst) << " ---\n";

		if (!vm.isShutdown) {
			std::cout << "vm shutdown\n";
			continue;
		}

		std::cout << "time/frame: " << std::dec << vm.time << "/" << vm.frame << "\n";

		std::cout << "maxstack/top: " << std::hex << vm.maxstack << "/" << vm.top << "\n";
		
		if (opt.m_gvars) {


			std::ofstream nullstream;
			nullstream.setstate(std::ios_base::badbit);

			std::ostream& out = (opt.m_dumpstring) ? nullstream : std::cout;

			std::cout << "global vars:\n";

			ScrVar gvarVar;
			for (const auto& gv : pub.globalVars) {
				if (!gv.id) {
					continue;
				}
				out << "- " << hashutils::ExtractTmp("var", gv.name) << " id: " << std::dec << gv.id << ", persist: " << (gv.persist ? "true" : "false");
				if (!proc.ReadMemory(&gvarVar, glob.scriptValues + sizeof(ScrVar) * gv.id, sizeof(ScrVar))) {
					std::cout << "Error reading value\n";
					break;
				}
				GetScrVarInfoPtr(out << " = ", (int)inst, proc, gvarVar, gv.id, glob, opt) << "\n";
			}

			
		}

		if (!vm.function_count) {
			std::cout << "Empty function stack\n";
			continue;
		}

		if (vm.function_count < 0) {
			std::cerr << "Stack Error\n";
			continue;
		}
		if (vm.function_count >= 64) {
			std::cerr << "Stack overflow\n";
			vm.function_count = 63;
		}

		// reading stack trace
		// don't ask me why, but it seems this array starts at the index 1
		std::cout << "Stack trace " << std::dec << vm.function_count
			<< "\n";

		int localvarshift = 0;
		for (size_t i = vm.function_count; i > 0; i--) {
			auto& stack = vm.function_frame_start[i];
			std::cout << "+- " << i << ":" << std::hex << stack.bytecodeLocation << ": ";
			if (!FindByteCodeLocationInfo((int)inst, proc, info, stack.bytecodeLocation, true)) {
				std::cout << "Can't read bytecode info\n";
			}
			else {
				std::cout
					<< std::hex
					<< hashutils::ExtractTmpScript(info.obj.name) << std::flush 
					<< " (" << (stack.bytecodeLocation - info.scriptRLoc) << ") "
					<< hashutils::ExtractTmp("namespace", info.exp.name_space) << std::flush << "::"
					<< hashutils::ExtractTmp("function", info.exp.name) << std::flush
					<< " (" << info.exp.address << "+" << info.functionRLoc << ") "
					<< "\n";

				BYTE* bytecodeStart = utils::Aligned<UINT16>(&info.script->magic[info.exp.address]);

				UINT16 opcodeVal = *reinterpret_cast<UINT16*>(bytecodeStart);
				auto opcode = tool::gsc::opcode::LookupOpCode(info.obj.GetVm(), tool::gsc::opcode::PLATFORM_PC, opcodeVal);

				// dump local variables (if any)
				if (opt.m_vars && opcode && opcode->m_id == tool::gsc::opcode::OPCODE_SafeCreateLocalVariables) {
					BYTE lvars = *(bytecodeStart += 2);
					std::cout << "+-+-- vars (" << std::dec << (int) lvars << ")\n";
					bytecodeStart++;
					ScrVar localvarvalue;
					for (size_t i = 0; i < lvars; i++) {
						bytecodeStart = utils::Aligned<UINT32>(bytecodeStart);
						UINT32 varname = *reinterpret_cast<UINT32*>(bytecodeStart);
						bytecodeStart += 5; // name + flags

						if (opt.m_stack) {
							std::cout << "|";
						}
						else {
							std::cout << " ";
						}

						std::cout << " +- " << hashutils::ExtractTmp("var", varname) << " = " << std::flush;

						//parentID = scrVmPub[inst].localVars[-*localvar_id],
						UINT32 id = proc.ReadMemory<UINT32>(vm.localVars - sizeof(UINT32) * (lvars - i - 1 + localvarshift));
						if (!id) {
							std::cout << "Error reading field id\n";
							continue;
						}
						
						// v5 = &gScrVarGlob[inst].scriptValues[parentID];
						if (!proc.ReadMemory(&localvarvalue, glob.scriptValues + sizeof(ScrVar) * id, sizeof(ScrVar))) {
							std::cout << "Error reading value\n";
							break;
						}
						GetScrVarInfo(std::cout, (int)inst, proc, localvarvalue, glob, opt) << "\n";
					}
					localvarshift += lvars;

				}

				// dump stack, a CODEPOS should be at the bottom
				if (opt.m_stack) {
					std::cout << "+-+-- stack:\n";

					ScrVar top;
					uintptr_t topPtr = stack.top + sizeof(ScrVar);
					int index = 1;
					do {
						topPtr -= sizeof(ScrVar);
						index--;

						std::cout << "  +- " << std::dec << std::setw(2) << std::setfill(' ') << index << ":";
						if (!proc.ReadMemory(&top, topPtr, sizeof(ScrVar))) {
							std::cout << "Error reading\n";
							break;
						}
						GetScrVarInfo(std::cout, (int)inst, proc, top, glob, opt) << "\n";
					} while (top.type != TYPE_CODEPOS);
				}

				// we do it here so we can access the script data
				std::free(info.scriptData);
			}
			std::cout << "\n";
		}

	}

	return tool::OK;
}

ADD_TOOL("dbg", "", "vm debuger", nullptr, vmdebug);
