#pragma once
#include <core/memory_allocator_static.hpp>
namespace acts::vm {
	constexpr uint64_t ACTSCRIPT_MAGIC = 0xF1000a0d43534780;

	enum ScriptExportFlags {
		SEF_AUTOEXEC = 1,
		SEF_PRIVATE = 1 << 1,
	};

	struct ScriptExport {
		uint32_t address;
		uint32_t name;
		uint32_t name_space;
		uint32_t export_data;
		uint8_t param_count;
		uint8_t flags;
	};

	struct ActScript {
		byte magic[8];
		uint64_t name;
		byte flags;

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
			return (ScriptExport*)(magic + exports_table);
		}

		constexpr ScriptExport* ExportsEnd() const {
			return (ScriptExport*)(magic + exports_table) + exports_count;
		}

		constexpr bool IsInScript(byte* codePos) const {
			return codePos >= magic + cseg_offset && codePos < magic + cseg_offset + cseg_size;
		}
	};

	typedef uint32_t VmRef;

	struct ScriptInfo {
		ActScript* script;
		int group;
	};

	struct ActsVmConfig {
		std::function<ActScript* (uint64_t name)> getterFunction;
		std::function<const char* (uint64_t hash)> hashToString;
	};

	enum ActsVmLinkOutput : int {
		VMLO_NOTHING = 0,
		VMLO_CANT_FIND = -1,
		VMLO_ERROR_INCLUDES = -2,
	};
	enum VmVarType {
		VT_UNDEFINED = 0,
		VT_THREAD,
		VT_POINTER,
		VT_INTEGER,
		VT_FLOAT,
		VT_HASH,
		VT_PRECALL,
	};

	union VmVarValue {
		int64_t i;
		float f;
		VmRef ref;
		uint64_t hash;
		bool b;
	};

	struct VmVar {
		VmVarType type;
		VmVarValue val;
	};

	struct VmExecutionThread {
		bool running{};
		VmRef threadId{};
		utils::Timestamp waitFrameTime{};
		size_t waitTime{};
		VmVar stack[0x1000];
		VmVar* top;
		byte* codePos;

		template<typename T = byte>
		T* AlignedData() {
			return (T*)utils::Aligned<T>(codePos);
		}

		template<typename T = byte>
		T* SetAlignedData() {
			T* d{ AlignedData<T>() };
			codePos = (byte*)d;
			return d;
		}
	};


	class ActsVm {
		VmExecutionThread threads[0x100]{};
		VmExecutionThread* currentThread{};
		core::memory_allocator::MemoryAllocatorStatic<0x20000, VmRef> alloc{};
		ActsVmConfig cfg;
		int linkGroup{};
		size_t topScripts{};
		std::vector<ScriptInfo> scripts{};
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
		void LoadScript(uint64_t name);
		void Execute();
		void ReleaseVariable(VmVar* var);
		void Error(const char* msg, bool terminate);
	private:
		void AssertThreadStarted();
		int LinkScript(uint64_t name);
	};
}