#pragma once

#include <dll_includes.hpp>
#include <hook/process.hpp>
#include <games/bo4/t8_internal.hpp>
#include <games/bo4/scriptinstance.hpp>
#include <games/bo4/offsets.hpp>
#include <games/bo4/pool.hpp>

using namespace games::bo4;
namespace bo4 {
    inline byte* Relativise(uintptr_t location) {
        return hook::process::Relativise(location);
    }

	struct Vec3 {
		float x;
		float y;
		float z;
	};

	struct Vec4 {
		float x;
		float y;
		float z;
		float w;
	};

    typedef void (*BuiltinFunction)(scriptinstance::ScriptInstance inst);

    enum BuiltinType : int {
        BUILTIN_DEFAULT = 0,
        BUILTIN_DEV = 1
    };

    struct __declspec(align(8)) Builtin {
        uint32_t name;
        int min_args;
        int max_args;
        BuiltinFunction actionFunc;
        BuiltinType type;
    };


    struct GSCOBJ {
        byte magic[8];
        int32_t crc;
        int32_t pad;
        uint64_t name;
        int32_t include_offset;
        uint16_t string_count;
        uint16_t exports_count;
        int32_t ukn20;
        int32_t string_offset;
        int16_t imports_count;
        uint16_t fixup_count;
        int32_t ukn2c;
        int32_t export_table_offset;
        int32_t ukn34;
        int32_t imports_offset;
        uint16_t globalvar_count;
        int32_t fixup_offset;
        int32_t globalvar_offset;
        int32_t script_size;
        int32_t ukn4c_offset;
        int32_t ukn50;
        int32_t ukn54;
        uint16_t include_count;
        byte ukn5a;
        byte ukn4c_count;
    };

    struct T8GSCFixup {
        uintptr_t offset;
        uintptr_t address;
    };

    struct T8GSCImport {
        uint32_t name;
        uint32_t import_namespace;
        uint16_t num_address;
        uint8_t param_count;
        uint8_t flags;
    };

    struct T8GSCGlobalVar {
        uint32_t name;
        uint32_t num_address;
    };

    struct GSCExport {
        uint32_t checksum;
        uint32_t address;
        uint32_t name;
        uint32_t name_space;
        uint32_t callback_event;
        uint8_t param_count;
        uint8_t flags;
        uint16_t padding;
    };

    enum GSCExportFlags : uint8_t {
        LINKED = 0x01,
        AUTOEXEC = 0x02,
        PRIVATE = 0x04,
        CLASS_MEMBER = 0x08,
        CLASS_DESTRUCTOR = 0x10,
        VE = 0x20,
        EVENT = 0x40,
        CLASS_LINKED = 0x80,
        CLASS_VTABLE = 0x86
    };

    enum GSCImportFlags : uint8_t {
        FUNC_METHOD = 0x1,
        FUNCTION = 0x2,
        FUNCTION_THREAD = 0x3,
        FUNCTION_CHILDTHREAD = 0x4,
        METHOD = 0x5,
        METHOD_THREAD = 0x6,
        METHOD_CHILDTHREAD = 0x7,
        CALLTYPE_MASK = 0xF,
        DEV_CALL = 0x10,
        GET_CALL = 0x20,
    };

    struct GSCString {
        uint32_t string;
        uint8_t num_address;
        uint8_t type;
        uint16_t pad;
    };

    struct ObjFileInfo {
        GSCOBJ* activeVersion;
        int32_t slot;
        int32_t refCount;
        uint32_t groupId;
        int32_t Pad0;
    };

    struct CmdFunction
    {
        CmdFunction* next;
        uint64_t name;
        uint64_t pad0;
        uint64_t pad1;
        uint64_t pad2;
        void (*function)();
    };

    enum offsetbo4 : uintptr_t {
        OFFSET_ScrVm_Error = 0x2770330,
        OFFSET_ScrVm_RuntimeError = 0x27775B0,
        OFFSET_DB_FindXAssetHeader = 0x2EB75B0,
        OFFSET_StringTable_GetAsset = 0x28A2660,
        OFFSET_LogCompilerError = 0x2890470,
        OFFSET_Error = 0x3D36CC0,

        // Scr link functions
        OFFSET_Scr_GetFunctionReverseLookup = 0x33AF8A0,
        OFFSET_CScr_GetFunctionReverseLookup = 0x1F132A0,
        OFFSET_CScr_GetFunction = 0x1F13140,
        OFFSET_CScr_GetMethod = 0x1F13650,
        OFFSET_Scr_GetFunction = 0x33AF840,
        OFFSET_Scr_GetMethod = 0x33AFC20,

        // Scr vm functions
        OFFSET_ScrVm_AddBool = 0x276E760,
        OFFSET_ScrVm_AddFloat = 0x276E9B0,
        OFFSET_ScrVm_AddHash = 0x276EAB0,
        OFFSET_ScrVm_AddInt = 0x276EB80,
        OFFSET_ScrVm_AddString = 0x276EE30,
        OFFSET_ScrVm_AddUndefined = 0x276F3C0,
        OFFSET_ScrVm_AddConstString = 0x276E5F0,
        OFFSET_ScrVm_GetBool = 0x2772AB0,
        OFFSET_ScrVm_GetFloat = 0x27733F0,
        OFFSET_ScrVm_GetHash = 0x27738E0,
        OFFSET_ScrVm_GetInt = 0x2773B50,
        OFFSET_ScrVm_GetNumParam = 0x2774440,
        OFFSET_ScrVm_GetPointerType = 0x27746E0,
        OFFSET_ScrVm_GetString = 0x2774840,
        OFFSET_ScrVm_GetType = 0x2774A20,
        OFFSET_ScrVm_GetVector = 0x2774E40,
        OFFSET_ScrVm_GetConstString = 0x02772E10,

        OFFSET_Cmd_AddCommandInternal = 0x3CDEE80,
	};
    typedef ObjFileInfo ObjFileInfoTable[650];

	const auto ScrVm_AddBool = reinterpret_cast<void(__fastcall*)(scriptinstance::ScriptInstance inst, int value)>(Relativise(OFFSET_ScrVm_AddBool));
	const auto ScrVm_AddFloat = reinterpret_cast<void(__fastcall*)(scriptinstance::ScriptInstance inst, float value)>(Relativise(OFFSET_ScrVm_AddFloat));
	const auto ScrVm_AddHash = reinterpret_cast<void(__fastcall*)(scriptinstance::ScriptInstance inst, XHash *value)>(Relativise(OFFSET_ScrVm_AddHash));
	const auto ScrVm_AddInt = reinterpret_cast<void(__fastcall*)(scriptinstance::ScriptInstance inst, int64_t value)>(Relativise(OFFSET_ScrVm_AddInt));
	const auto ScrVm_AddString = reinterpret_cast<void(__fastcall*)(scriptinstance::ScriptInstance inst, const char* value) > (Relativise(OFFSET_ScrVm_AddString));
	const auto ScrVm_AddUndefined = reinterpret_cast<void(__fastcall*)(scriptinstance::ScriptInstance inst)>(Relativise(OFFSET_ScrVm_AddUndefined));
	const auto ScrVm_AddConstString = reinterpret_cast<void(__fastcall*)(scriptinstance::ScriptInstance inst, int32_t value)>(Relativise(OFFSET_ScrVm_AddConstString));

	const auto ScrVm_GetBool = reinterpret_cast<bool(__fastcall*)(scriptinstance::ScriptInstance inst, unsigned int index)>(Relativise(OFFSET_ScrVm_GetBool));
	const auto ScrVm_GetFloat = reinterpret_cast<float(__fastcall*)(scriptinstance::ScriptInstance inst, unsigned int index)>(Relativise(OFFSET_ScrVm_GetFloat));
	const auto ScrVm_GetHash = reinterpret_cast<XHash * (__fastcall*)(XHash * hash, scriptinstance::ScriptInstance inst, unsigned int index)>(Relativise(OFFSET_ScrVm_GetHash));
	const auto ScrVm_GetInt = reinterpret_cast<int64_t(__fastcall*)(scriptinstance::ScriptInstance inst, unsigned int index)>(Relativise(OFFSET_ScrVm_GetInt));
	const auto ScrVm_GetString = reinterpret_cast<const char*(__fastcall*)(scriptinstance::ScriptInstance inst, unsigned int index)>(Relativise(OFFSET_ScrVm_GetString));
	const auto ScrVm_GetVector = reinterpret_cast<void(__fastcall*)(scriptinstance::ScriptInstance inst, unsigned int index, Vec3 * vector)>(Relativise(OFFSET_ScrVm_GetVector));
	const auto ScrVm_GetConstString = reinterpret_cast<int32_t(__fastcall*)(scriptinstance::ScriptInstance inst, unsigned int index)>(Relativise(OFFSET_ScrVm_GetConstString));

	const auto ScrVm_GetNumParam = reinterpret_cast<uint32_t(__fastcall*)(scriptinstance::ScriptInstance inst)>(Relativise(OFFSET_ScrVm_GetNumParam));
	const auto ScrVm_GetPointerType = reinterpret_cast<t8internal::ScrVarType(__fastcall*)(scriptinstance::ScriptInstance inst, unsigned int index)>(Relativise(OFFSET_ScrVm_GetPointerType));
	const auto ScrVm_GetType = reinterpret_cast<t8internal::ScrVarType(__fastcall*)(scriptinstance::ScriptInstance inst, unsigned int index)>(Relativise(OFFSET_ScrVm_GetType));
	const auto Internal_ScrVm_Error = reinterpret_cast<void(__fastcall*)(uint64_t code, scriptinstance::ScriptInstance inst, char* unk, bool terminal)>(Relativise(OFFSET_ScrVm_Error));

    const auto Scr_GetFunction = reinterpret_cast<BuiltinFunction(__fastcall*)(uint32_t name, BuiltinType* type, int* min_args, int* max_args)> (Relativise(OFFSET_Scr_GetFunction));
    const auto CScr_GetFunction = reinterpret_cast<BuiltinFunction(__fastcall*)(uint32_t name, BuiltinType * type, int* min_args, int* max_args)> (Relativise(OFFSET_CScr_GetFunction));
    const auto Scr_GetMethod = reinterpret_cast<BuiltinFunction(__fastcall*)(uint32_t name, BuiltinType * type, int* min_args, int* max_args)> (Relativise(OFFSET_Scr_GetMethod));
    const auto CScr_GetMethod = reinterpret_cast<BuiltinFunction(__fastcall*)(uint32_t name, BuiltinType * type, int* min_args, int* max_args)> (Relativise(OFFSET_CScr_GetMethod));

    const auto Cmd_AddCommandInternal = reinterpret_cast<void(__fastcall*)(XHash * cmdName, void (*command)(), CmdFunction* alloc)>(Relativise(OFFSET_Cmd_AddCommandInternal));

	extern t8internal::scrVmPub* scrVmPub;
	extern t8internal::scrVarPub* scrVarPub;
    extern ObjFileInfoTable* objFileInfo;
    extern int* objFileInfoCount;


	void ScrVm_Error(scriptinstance::ScriptInstance inst, const char* format, bool terminal, ...);
    bool FindGSCFuncLocation(byte* location, scriptinstance::ScriptInstance& inst, GSCOBJ*& obj, GSCExport*& exp, uint32_t& rloc);
	
}
#define REGISTER_COMMAND(id, name, cmd) static bo4::CmdFunction alloc_func_##id;\
    static bo4::Hash alloc_func_hash##id = { hash::Hash64Pattern(name) };\
    bo4::Cmd_AddCommandInternal(&alloc_func_hash##id, cmd, &alloc_func_##id)