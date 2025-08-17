#pragma once
#include "bo4.hpp"

namespace bo4 {
	using namespace hook::refs;

	S_ANY Ref<XAssetHeader(bo4::XAssetType type, XHash* name, bool errorIfMissing, int waitTime)> DB_FindXAssetHeader{ 0x2EB75B0_a };
	S_ANY Ref<bool(bo4::XAssetType type, XHash* name)> DB_DoesXAssetExist{ 0x2EB6C90_a };

	// gsc
	S_ANY Ref<void(scriptInstance_t inst, int value)> ScrVm_AddBool{ 0x276E760_a };
	S_ANY Ref<void(scriptInstance_t inst, float value)> ScrVm_AddFloat{ 0x276E9B0_a };
	S_ANY Ref<void(scriptInstance_t inst, XHash* value)> ScrVm_AddHash{ 0x276EAB0_a };
	S_ANY Ref<void(scriptInstance_t inst, int64_t value)> ScrVm_AddInt{ 0x276EB80_a };
	S_ANY Ref<void(scriptInstance_t inst, const char* value) > ScrVm_AddString{ 0x276EE30_a };
	S_ANY Ref<void(scriptInstance_t inst)> ScrVm_AddUndefined{ 0x276F3C0_a };
	S_ANY Ref<void(scriptInstance_t inst, int32_t value)> ScrVm_AddConstString{ 0x276E5F0_a };
	S_ANY Ref<bool(scriptInstance_t inst, unsigned int index)> ScrVm_GetBool{ 0x2772AB0_a };
	S_ANY Ref<float(scriptInstance_t inst, unsigned int index)> ScrVm_GetFloat{ 0x27733F0_a };
	S_ANY Ref<XHash* (XHash* hash, scriptInstance_t inst, unsigned int index)> ScrVm_GetHash{ 0x27738E0_a };
	S_ANY Ref<int64_t(scriptInstance_t inst, unsigned int index)> ScrVm_GetInt{ 0x2773B50_a };
	S_ANY Ref<const char* (scriptInstance_t inst, unsigned int index)> ScrVm_GetString{ 0x2774840_a };
	S_ANY Ref<void(scriptInstance_t inst, unsigned int index, vec3_t* vector)> ScrVm_GetVector{ 0x2774E40_a };
	S_ANY Ref<ScrVarIndex_t(scriptInstance_t inst, unsigned int index)> ScrVm_GetConstString{ 0x2772E10_a };
	S_ANY Ref<uint32_t(scriptInstance_t inst)> ScrVm_GetNumParam{ 0x2774440_a };
	S_ANY Ref<ScrVarType_t(scriptInstance_t inst, unsigned int index)> ScrVm_GetPointerType{ 0x27746E0_a };
	S_ANY Ref<ScrVarType_t(scriptInstance_t inst, unsigned int index)> ScrVm_GetType{ 0x2774A20_a };
	S_ANY Ref<ScrVarValue_t*(scriptInstance_t inst, unsigned int index)> ScrVm_GetValue{ 0x2774D60_a };
	S_ANY Ref<void(scriptInstance_t inst, byte* codePos)> ScrVm_AddScriptFunction{ 0x276EA60_a };
	S_ANY Ref<void(scriptInstance_t inst, void* func)> ScrVm_AddAPIFunction{ 0x276E540_a };
	S_ANY Ref<uint32_t(scriptInstance_t inst)> ScrVm_AddStruct{ 0x276EF00_a };
	S_ANY Ref<void(scriptInstance_t inst, uint32_t structId, uint32_t name)> ScrVm_SetStructField{ 0x2778450_a };
	S_ANY Ref<void(scriptInstance_t inst)> ScrVm_AddToArray{ 0x276F1C0_a };
	S_ANY Ref<void(scriptInstance_t inst, XHash* name)> ScrVm_AddToArrayStringIndexed{ 0x276F230_a };
	S_ANY Ref<void(scriptInstance_t inst, vec3_t* vec)> ScrVm_AddVector{ 0x276F490_a };
	S_ANY Ref<void(scriptInstance_t inst)> ScrVar_PushArray{ 0x2775CF0_a };
	S_ANY Ref<const char* (ScrVarIndex_t index)> ScrStr_ConvertToString{ 0x2759030_a };
	S_ANY Ref<ScrVarIndex_t(scriptInstance_t inst, ScrVarIndex_t parentId, ScrVarNameIndex_t index)> ScrVar_NewVariableByIndex{ 0x2760440_a };
	S_ANY Ref<void(scriptInstance_t inst, ScrVarIndex_t id, ScrVarValue_t* value)> ScrVar_SetValue{ 0x27616B0_a };

	// gsc funcs
	S_ANY Ref<BuiltinFunction(uint32_t canonId, int* type, int* min_args, int* max_args)> CScr_GetFunction{ 0x1F13140_a };
	S_ANY Ref<BuiltinFunction(uint32_t canonId, int* type, int* min_args, int* max_args)> Scr_GetFunction{ 0x33AF840_a };
	S_ANY Ref<void*(uint32_t canonId, int* type, int* min_args, int* max_args)> CScr_GetMethod{ 0x1F13650_a };
	S_ANY Ref<void*(uint32_t canonId, int* type, int* min_args, int* max_args)> Scr_GetMethod{ 0x33AFC20_a };
	S_ANY Ref<bool(byte* func, uint32_t* name, bool* isFunction)> Scr_GetFunctionReverseLookup{ 0x33AF8A0_a };
	S_ANY Ref<bool(byte* func, uint32_t* name, bool* isFunction)> CScr_GetFunctionReverseLookup{ 0x1F132A0_a };

	S_ANY Ref<void(scriptInstance_t inst, byte* codepos, const char** scriptname, int32_t* sloc, int32_t* crc, int32_t* vm)> Scr_GetGscExportInfo{ 0x2748550_a };
	S_ANY Ref<void(uint64_t code, scriptInstance_t inst, char* unused, bool terminal)> ScrVm_Error{ 0x2770330_a };

	// gsc structs
	S_ANY Ref<BO4_scrVarPub> scrVarPub{ 0x8307880_a };
	S_ANY Ref<scrVarGlob_t> scrVarGlob{ 0x8307830_a };
	S_ANY Ref<scrVmPub_t> scrVmPub{ 0x8307AA0_a };
	S_ANY Ref<VM_OP_FUNC> gVmOpJumpTable{ 0x4EED340_a };
	S_ANY Ref<VM_OP_FUNC> gVmOpJumpTableModTool{ 0x4EFD340_a };
	S_ANY Ref<VM_OP_ERR_FUNC> gVmOpJumpTableError{ 0x4EF5340_a };
	S_ANY Ref<VM_OP_ERR_FUNC> gVmOpJumpTableErrorModTool{ 0x4F05340_a };
	S_ANY Ref<uint32_t> gObjFileInfoCount{ 0x82F76B0_a };
	S_ANY Ref<const char*> scrVarTypeNames{ 0x4EED240_a };
	S_ANY Ref<objFileInfo_t[games::bo4::scriptinstance::ScriptInstance::SI_COUNT][650]> gObjFileInfo{ 0x82EFCD0_a };
	S_ANY Ref<void*> scrVmInvalidFuncs{ 0x4EED038_a };

	// lua funcs
	S_ANY Ref<bool(lua_State* luaVM, const char* file)> Lua_CoD_LoadLuaFile{ 0x3962DF0_a };
	S_ANY Ref<void(const char* tname, lua_State* luaVM)> Lua_BeginTableReadOnly{ 0x390D3D0_a };
	S_ANY Ref<void(lua_State* luaVM)> Lua_EndTableReadOnly{ 0x390D650_a };
	S_ANY Ref<void(lua_State* luaVM, XHash* name, int(*func)(lua_State* s))> Lua_Cod_RegisterFunction{ 0x3911980_a };
	S_ANY Ref<const char* (lua_State* luaVM, HksObject* obj, size_t* len)> hks_obj_tolstring{ 0x3755730_a };
	S_ANY Ref<float(lua_State* luaVM, const HksObject* obj)> hks_obj_tonumber{ 0x3755A90_a };
	S_ANY Ref<XHash* (XHash* retval, lua_State* luaVM, int idx)> hks_obj_tolhash{ 0x2EBF270_a };
	S_ANY Ref<void(lua_State* s, int index, const char* k) > hksi_lua_setfield{ 0x22C8E80_a };
	S_ANY Ref<void(lua_State* s, const char* str, size_t l)> hksi_lua_pushlstring{ 0xC49D70_a };
	S_ANY Ref<void(lua_State* s, const char* str)> hksi_lua_pushxhash{ 0x3A84900_a };
	
	// xassets
	S_ANY Ref<XHash* (XHash* hash, const XAsset* asset)> DB_GetXAssetName{ 0x28AAB60_a };
	S_ANY Ref<bool(const char* file)> DB_FileExists{ 0x2EB72C0_a };
	S_ANY Ref<int32_t(const char* file)> Stream_OpenFile49{ 0x2E0E8B0_a };
	S_ANY Ref<void(int32_t fileid)> Stream_CloseFile{ 0x3C4C260_a };
	S_ANY Ref<XAssetPool> s_assetPools{ 0x912A5B0_a };
	
	// common
	S_ANY Ref<const char* (int32_t lang)> SEH_GetLanguageNameAbbr{ 0x3CB1340_a };
	S_ANY Ref<const dvar_t*> dvar_loc_language{ 0xF92B348_a };

	// mutex
	class ScopedCriticalSection;
	S_ANY Ref<void(ScopedCriticalSection* sec, int32_t s, ScopedCriticalSectionType type)> ScopedCriticalSectionConstructor{ 0x289E3C0_a };
	S_ANY Ref<void(ScopedCriticalSection* sec)> ScopedCriticalSectionDestructor{ 0x289E440_a };

	class ScopedCriticalSection {
		int32_t _s;
		bool _hasOwnership;
		bool _isScopedRelease;
		ScopedCriticalSection* _next;
	public:
		ScopedCriticalSection(int32_t s, ScopedCriticalSectionType type) {
			ScopedCriticalSectionConstructor(this, s, type);
		}
		~ScopedCriticalSection() {
			ScopedCriticalSectionDestructor(this);
		}
	};
}