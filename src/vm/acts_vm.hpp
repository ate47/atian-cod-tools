#pragma once
#include <core/memory_allocator_static.hpp>
namespace acts::vm {
	constexpr uint64_t ACTSCRIPT_MAGIC = 0x4d565354434124F1;

	enum ScriptExportFlags {
		SEF_AUTOEXEC = 1,
		SEF_PRIVATE = 1 << 1,
	};

	enum ScriptImportFlags {
		SIF_GET = 1,
		SIF_GET_BUILTIN = 2,
		SIF_FUNCTION = 3,
		SIF_FUNCTION_BUILTIN = 4,
		SIF_FUNCTION_THREAD = 5,
		SIF_FUNCTION_CHILDTHREAD = 6,
		SIF_METHOD = 7,
		SIF_METHOD_BUILTIN = 8,
		SIF_METHOD_THREAD = 9,
		SIF_METHOD_CHILDTHREAD = 0xa,
		SIF_TYPE_MASK = 0xf,
	};

	struct ScriptExport {
		uint32_t address;
		uint32_t checksum;
		uint64_t name;
		uint64_t name_space;
		uint64_t data;
		uint8_t param_count;
		uint8_t flags;
	};

	struct ScriptImport {
        uint64_t name;
        uint64_t name_space;
        uint16_t num_address;
        uint8_t param_count;
        uint8_t flags;
	};

	struct ScriptString {
		uint32_t address;
		uint8_t num_address;
	};

	struct ActScript {
		byte magic[8];
		uint32_t name_offset;
		byte flags;
		uint32_t file_size;
		uint32_t checksum;

		uint32_t includes_table;
		uint32_t strings_table;
		uint32_t exports_table;
		uint32_t imports_table;

		uint16_t includes_count;
		uint16_t strings_count;
		uint16_t exports_count;
		uint16_t imports_count;

		uint32_t cseg_offset;
		uint32_t cseg_size;

		constexpr ScriptExport* Exports() const {
			return (ScriptExport*)&magic[exports_table];
		}

		constexpr ScriptExport* ExportsEnd() const {
			return &Exports()[exports_count];
		}

		constexpr uint32_t* Includes() const {
			return (uint32_t*)&magic[includes_table];
		}

		constexpr uint32_t* IncludesEnd() const {
			return &Includes()[includes_count];
		}

		constexpr ScriptImport* Imports() const {
			return (ScriptImport*)&magic[imports_table];
		}

		constexpr ScriptImport* ImportsEnd() const {
			return &Imports()[imports_count];
		}

		constexpr const char* GetName() const {
			return (const char*)&magic[name_offset];
		}

		constexpr const char* GetString(uint32_t ref) const {
			return (const char*)&magic[ref];
		}

		constexpr bool IsInScript(byte* codePos) const {
			return codePos >= magic + cseg_offset && codePos < magic + cseg_offset + cseg_size;
		}
	};

	typedef uint32_t VmRef;
	constexpr VmRef INVALID_STR_REF = static_cast<VmRef>(-1);
	constexpr uint16_t BAD_BUILTIN_ID = static_cast<uint16_t>(-1);
	constexpr uint16_t FLAG_BUILTIN_METHOD = 0x8000;
	constexpr uint32_t BAD_SCRIPTFUNC_ID = static_cast<uint32_t>(-1);

	struct ScriptInfo {
		ActScript* script;
		int group;
	};

	struct ActsVmConfig {
		std::function<ActScript* (const char* name)> getterFunction;
		std::function<const char* (uint64_t hash)> hashToString;
		bool enabledDevBlocks{};
	};

	enum ActsVmLinkOutput : int {
		VMLO_NOTHING = 0,
		VMLO_CANT_FIND = -1,
		VMLO_ERROR_INCLUDES = -2,
		VMLO_ERROR_TOO_MANY = -3,
	};
	enum VmVarType {
		VT_UNDEFINED = 0,
		VT_THREAD,
		VT_POINTER,
		VT_INTEGER,
		VT_FLOAT,
		VT_HASH,
		VT_LOCATION,

		VT_SCRIPT_FUNCTION,
		VT_BUILTIN_FUNCTION,
		VT_ARRAY,
		VT_VECTOR,
		VT_STRUCT,
		VT_STRING,
	};

	const char* VmVarTypeName(VmVarType type);

	union VmVarValue {
		int64_t i;
		float f;
		VmRef ref;
		uint64_t hash;
		bool b;
		byte* loc;
		uint16_t builtinFunc;
		uint32_t scriptFunc;
	};

	struct VmVar {
		VmVarType type;
		VmVarValue val;
	};

	struct VmVector {
		size_t ref;
		float vec[3]{};
	};

	struct VmString {
		size_t ref;
		uint64_t hash;
		VmRef next;
		VmRef prev;

		inline char* Str() {
			return (char*)(&this[1]);
		}
	};

	struct VmArrayNode {
		VmVar idx{};
		VmVar var{};
		VmRef next{};
	};

	struct VmArray {
		size_t ref;
		VmRef start{};
	};

	struct VmFunctionFrame {
		size_t numVars{};
		VmVar* topVars{};
	};

	struct VmExecutionThread {
		VmFunctionFrame frames[0x40];
		bool running{};
		VmRef threadId{};
		utils::Timestamp waitFrameTime{};
		size_t waitTime{};
		VmVar stack[0x1000];
		VmVar* top;
		VmFunctionFrame* frame{};
		byte* codePos;

		template<typename T = byte>
		T* Data() {
			return (T*)utils::Aligned<T>(codePos);
		}

		template<typename T = byte>
		T* SetData() {
			T* d{ Data<T>() };
			codePos = (byte*)d;
			return d;
		}
	};

	class ActsVm;

	union BuiltinCallback {
		void (*func)(ActsVm* vm);
		void (*method)(ActsVm* vm, VmRef obj);
	};

	struct BuiltinCall {
		BuiltinCallback callback;
		uint32_t minArgs;
		uint32_t maxArgs;
		uint64_t name;
	};


	class ActsVm {
		BuiltinCall callFuncs[0x100]{};
		BuiltinCall callMethods[0x100]{};
		size_t callMethodsCount{};
		size_t callFuncsCount{};
		VmExecutionThread threads[0x100]{};
		VmExecutionThread* currentThread{};
		core::memory_allocator::MemoryAllocatorStatic<0x20000, VmRef> alloc{};
		VmRef stringStart[0x200]{};
		ActScript defaultScript{};
		ActsVmConfig cfg;
		int linkGroup{};
		size_t topScripts{};
		ScriptInfo linkedScripts[0x400];
		size_t linkedScriptsCount{};
	public:
		ActsVm(ActsVmConfig cfg);

		VmExecutionThread* AllocThread(byte* codePos);
		bool GetFunctionInfo(byte* codePos, ScriptExport** exp, ActScript** script);
		void CleanupThread(VmExecutionThread* thread);
		VmVar* PushStack(size_t count = 1);
		void PopStack(size_t count = 1);
		void AddInt(int64_t val);
		void AddFloat(float val);
		void AddHash(uint64_t val);
		void AddArray();
		void AddStruct();
		void AddVector(float* vec);
		void AddString(const char* str);
		void AddStringRef(VmRef ref);
		void LoadScript(const char* name);
		void Execute();
		void ReleaseVariable(VmVar* var);
		size_t GetScriptInfoId(const ScriptInfo* info);
		VmRef CreateString(const char* str, size_t numRefs = 1);
		VmString* GetStringEntry(VmRef ref);
		const char* GetString(VmRef ref);
		void IncRef(VmVar* var);
		void DecRef(VmVar* var);
		void Error(const char* msg, bool terminate);
		bool CastToBool(VmVar* var);
		constexpr const ActsVmConfig& Cfg() {
			return cfg;
		}
		void RegisterBuiltin(BuiltinCallback callback, uint64_t name, uint32_t minArgs, uint32_t maxArgs, bool isMethod);
		BuiltinCall* GetBuiltin(uint16_t builtin);
		uint32_t FindScriptFunction(const char* file, uint64_t namesp, uint64_t name);
		uint16_t FindBuiltin(uint64_t name);
		uint16_t FindBuiltinFunction(uint64_t name);
		uint16_t FindBuiltinMethod(uint64_t name);
		uint32_t GetScriptExport(ScriptInfo* origin, uint64_t nsp, uint64_t name);
		ActScript* GetActsScript(const char* name);
	private:
		void AssertThreadStarted();
		int LinkScript(const char* name, ScriptInfo** oInfo);
		int LinkActsScript(ActScript* script, ScriptInfo** oInfo);
	};
}