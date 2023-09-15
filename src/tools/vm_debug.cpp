#include <includes.hpp>

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

enum ScrVarType : UINT32 {
	TYPE_UNDEFINED = 0x0,
	TYPE_POINTER = 0x1,
	TYPE_STRING = 0x2,
	TYPE_VECTOR = 0x3,
	TYPE_HASH = 0x4,
	TYPE_FLOAT = 0x5,
	TYPE_INTEGER = 0x6,
	TYPE_UINTPTR = 0x7,
	TYPE_ENTITY_OFFSET = 0x8,
	TYPE_CODEPOS = 0x9,
	TYPE_PRECODEPOS = 0xA,
	TYPE_API_FUNCTION = 0xB,
	TYPE_SCRIPT_FUNCTION = 0xC,
	TYPE_STACK = 0xD,
	TYPE_THREAD = 0xE,
	TYPE_NOTIFY_THREAD = 0xF,
	TYPE_TIME_THREAD = 0x10,
	TYPE_FRAME_THREAD = 0x11,
	TYPE_CHILD_THREAD = 0x12,
	TYPE_CLASS = 0x13,
	TYPE_SHARED_STRUCT = 0x14,
	TYPE_STRUCT = 0x15,
	TYPE_REMOVED_ENTITY = 0x16,
	TYPE_ENTITY = 0x17,
	TYPE_ARRAY = 0x18,
	TYPE_REMOVED_THREAD = 0x19,
	TYPE_FREE = 0x1a,
	TYPE_THREAD_LIST = 0x1b,
	TYPE_ENT_LIST = 0x1c
};

struct WeaponInfo {
	UINT64 weaponIdx : 9;
	UINT64 attachment1 : 6;
	UINT64 attachment2 : 6;
	UINT64 attachment3 : 6;
	UINT64 attachment4 : 6;
	UINT64 attachment5 : 6;
	UINT64 attachment6 : 6;
	UINT64 attachment7 : 6;
	UINT64 attachment8 : 6;
	UINT64 padding : 7;
};


struct ScrVarRef {
	UINT64 nameIndex;
	UINT32 _anon_0;
	UINT32 nextSibling;
	UINT32 prevSibling;
	UINT32 parentId;
	UINT32 nameSearchHashList;
	UINT32 pad0;
};

struct ScrVarEntityInfo {
	UINT16 classnum;
	UINT16 clientNum;
};


union UnionAll32 {
	UINT32 ui;
	INT64 ll;
	INT32 i;
	FLOAT f;
	ScrVarEntityInfo entVar;
};

union UnionAll {
	UINT64 ull;
	UINT32 ui;
	INT64 ll;
	INT32 i;
	DOUBLE d;
	FLOAT f;
	uintptr_t ptr;
	ScrVarEntityInfo entVar;
	WeaponInfo weaponInfo;
	UnionAll32 b32;
};

struct ScrVar {
	UnionAll value;
	ScrVarType type;
	UINT32 pad0;
};

struct __declspec(align(8)) FunctionStack {
	// BYTE*
	uintptr_t bytecodeLocation;
	// ScrVar*
	uintptr_t top;
	// ScrVar*
	uintptr_t startTop;
	UINT32 threadId;
	uint16_t localVarCount;
	uint16_t profileInfoCount;
};

struct __declspec(align(8)) scrVmPub {
	uintptr_t unknown1;
	uintptr_t unknown2;
	uintptr_t executionQueueHeap;
	uintptr_t timeByValueQueue; // VmExecutionQueueData*
	uintptr_t timeByThreadQueue[1024]; // VmExecutionQueue*
	uintptr_t frameByValueQueue; // VmExecutionQueueData*
	uintptr_t frameByThreadQueue[1024]; // VmExecutionQueue*
	uintptr_t timeoutByValueList; // VmExecutionQueueData*
	uintptr_t timeoutByThreadList[1024]; // VmExecutionQueue*
	uintptr_t notifyByObjectQueue[1024]; // VmExecutionNotifyQueue*
	uintptr_t notifyByThreadQueue[1024]; // VmExecutionNotifyQueue*
	uintptr_t endonByObjectList[1024]; // VmExecutionNotifyQueue*
	uintptr_t endonByThreadList[1024]; // VmExecutionNotifyQueue*
	uintptr_t localVars; // UINT32*
	uintptr_t maxstack; // ScrVar*
	uintptr_t function_frame; // function_frame*
	uintptr_t top; // ScrVar*
	FunctionStack function_frame_start[64];
	ScrVar stack[2048];
	uint32_t time;
	uint32_t frame;
	int function_count;
	int callNesting;
	unsigned int inparamcount;
	bool showError;
	bool systemInitialized;
	bool vmInitialized;
	bool isShutdown;
};

union ScrVarObjectInfo1 {
	UINT64 ull;
	UINT32 ui;
};

enum LocalClientNum : UINT32 {
	INVALID_LOCAL_CLIENT = 0xFFFFFFFF,
	LOCAL_CLIENT_0 = 0x0,
	LOCAL_CLIENT_FIRST = 0x0,
	LOCAL_CLIENT_KEYBOARD_AND_MOUSE = 0x0,
	LOCAL_CLIENT_1 = 0x1,
	LOCAL_CLIENT_2 = 0x2,
	LOCAL_CLIENT_3 = 0x3,
	LOCAL_CLIENT_COUNT = 0x4,
};


enum ClassNum : UINT16 {
	CLASSNUM_ENTITY = 0,
	CLASSNUM_HUD_ELEM = 1,
	CLASSNUM_PATHNODE = 2,
	CLASSNUM_VEHICLENODE = 3,
	CLASSNUM_TACPOINT = 4,
	CLASSNUM_DYNENT = 5,
	CLASSNUM_WEAPON = 6,
	CLASSNUM_COUNT = 0x7,
};

struct ScrEntityRef {
	UnionAll u;
	ClassNum classnum;
	LocalClientNum client;
};

enum FieldType : UINT32
{
	F_INT = 0x0,
	F_SHORT = 0x1,
	F_BYTE = 0x2,
	F_BOOL = 0x2,
	F_FLOAT = 0x3,
	F_LSTRING = 0x4,
	F_STRING = 0x5,
	F_VECTOR = 0x6,
	F_ENTITY = 0x7,
	F_ENTHANDLE = 0x8,
	F_ACTOR = 0x9,
	F_SENTIENT = 0xA,
	F_SENTIENTHANDLE = 0xB,
	F_CLIENT = 0xC,
	F_PATHNODE = 0xD,
	F_ACTORGROUP = 0xE,
	F_OBJECT = 0xF,
	F_XMODEL_INDEX = 0x10,
	F_XMODEL = 0x11,
	F_BITFLAG = 0x12,
	F_BITFLAG64 = 0x13,
	F_FX = 0x14,
	F_WEAPON = 0x15,
	F_RUMBLE = 0x16,
	F_SCRIPTBUNDLE = 0x17,
	F_COUNT = 0x18,
};

enum WeaponFieldType : UINT32
{
	F_WEAPON_INVALID = 0x0,
	F_WEAPON_DEF = 0x1,
};


struct ScrWeaponField
{
	UINT32 canonId;
	FieldType type;
	bool isreadonly;
	int ofs;
	int size;
	WeaponFieldType weaponType;
	uintptr_t getter; // void (*ScriptCallbackWeapon)(scriptInstance_t, Weapon, const scr_weapon_field_t *)
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

struct __declspec(align(8)) ScrVarStackBuffer {
	byte* pos;
	UINT16 size;
	UINT16 bufLen;
	UINT32 threadId; // ScrVarIndex
	byte buf[1]; // depends on the allocated size
};

struct __declspec(align(4)) ScrVarGlobalVars {
	UINT32 name;
	UINT32 id; // ScrVarIndex
	bool persist;
};


struct __declspec(align(8)) scrVarPub {
	uintptr_t fieldBuffer; // const char*
	uintptr_t error_message; // const char*
	uintptr_t programBuffer; // byte*
	uintptr_t endScriptBuffer; // byte*
	uintptr_t programHunkUser; // HunkUser*
	ScrVarGlobalVars globalVars[16];
	UINT64 entFieldNameIndex; // ScrVarNameIndex_t
	UINT32 freeEntList; // ScrVarIndex_t
	UINT32 tempVariable; // ScrVarIndex_t
	UINT32 checksum;
	UINT32 entId;
	UINT32 varHighWatermark;
	UINT32 numScriptThreads;
	UINT32 numVarAllocations;
	INT32 varHighWatermarkId;
};


struct ByteCodeLocationInfo {
	tool::gsc::T8GSCExport exp;
	tool::gsc::T8GSCOBJ obj;
	tool::gsc::T8GSCOBJ* script;
	BYTE* scriptData;
	UINT32 functionRLoc;
	UINT32 scriptRLoc;
};

LPCCH ScrVarTypeName(ScrVarType type) {
	switch (type) {
	case TYPE_UNDEFINED: return "UNDEFINED";
	case TYPE_POINTER: return "POINTER";
	case TYPE_STRING: return "STRING";
	case TYPE_VECTOR: return "VECTOR";
	case TYPE_HASH: return "HASH";
	case TYPE_FLOAT: return "FLOAT";
	case TYPE_INTEGER: return "INTEGER";
	case TYPE_UINTPTR: return "UINTPTR";
	case TYPE_ENTITY_OFFSET: return "ENTITY_OFFSET";
	case TYPE_CODEPOS: return "CODEPOS";
	case TYPE_PRECODEPOS: return "PRECODEPOS";
	case TYPE_API_FUNCTION: return "API_FUNCTION";
	case TYPE_SCRIPT_FUNCTION: return "SCRIPT_FUNCTION";
	case TYPE_STACK: return "STACK";
	case TYPE_THREAD: return "THREAD";
	case TYPE_NOTIFY_THREAD: return "NOTIFY_THREAD";
	case TYPE_TIME_THREAD: return "TIME_THREAD";
	case TYPE_FRAME_THREAD: return "FRAME_THREAD";
	case TYPE_CHILD_THREAD: return "CHILD_THREAD";
	case TYPE_CLASS: return "CLASS";
	case TYPE_SHARED_STRUCT: return "SHARED_STRUCT";
	case TYPE_STRUCT: return "STRUCT";
	case TYPE_REMOVED_ENTITY: return "REMOVED_ENTITY";
	case TYPE_ENTITY: return "ENTITY";
	case TYPE_ARRAY: return "ARRAY";
	case TYPE_REMOVED_THREAD: return "REMOVED_THREAD";
	case TYPE_FREE: return "FREE";
	case TYPE_THREAD_LIST: return "THREAD_LIST";
	default: return "<unknown type>";
	}
}


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

int vmdebug(const Process& proc, int argc, const char* argv[]) {
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
				auto opcode = tool::gsc::opcode::LookupOpCode(info.obj.GetVm(), opcodeVal);

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

ADD_TOOL("dbg", "", "vm debuger", true, vmdebug);
