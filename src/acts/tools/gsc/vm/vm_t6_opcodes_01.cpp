#include <includes.hpp>
#include <tools/gsc/gsc_vm.hpp>
#include <tools/gsc/gsc.hpp>
#include <tools/gsc/gsc_opcodes.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void OpCode() {
		VmInfo* vt = RegisterVM(VMI_T6_01, "Call of Duty: Black ops 2 (01)", "t6", "bo2_01", VmFlags::VMF_CLIENT_VM | VmFlags::VMF_NO_FILE_NAMESPACE | VmFlags::VMF_ALIGN | VmFlags::VMF_ANIMTREE_T7 | VmFlags::VMF_HASH_T7 | VmFlags::VMF_ISTRING | VmFlags::VMF_UNIQUE_HASH); // | VmFlags::VMF_CALL_NO_PARAMS
		vt->RegisterVmName("t6_01", "blackops2_01");
		vt->AddPlatform(PLATFORM_XBOX);
        vt->SetPlatformBigEndian(PLATFORM_XBOX);
		vt->RegisterVMGlobalVariable("level", OPCODE_IW_GetLevel);
		vt->RegisterVMGlobalVariable("game", OPCODE_IW_GetGame);
		vt->RegisterVMGlobalVariable("anim", OPCODE_IW_GetAnim);
		vt->RegisterVMOperatorFunction("isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		//vt->RegisterVMOperatorFunction("notify", "<caller> notify(event, param*)", OPCODE_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		//vt->RegisterVMOperatorFunction("endon", "<caller> endon(event)", OPCODE_IW_SingleEndon, VPFD_SELF_PARAM, 1, 1);
		//vt->RegisterVMOperatorFunction("vectorscale", "vectorscale(vector, factor) -> vector", OPCODE_VectorScale, VPFD_RETURN_VALUE, 2, 2);
		//vt->RegisterVMOperatorFunction("waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		// vt->RegisterVMOperatorFunction("waittillmatch", "<caller> waittillmatch(event, match) -> struct", OPCODE_WaitTillMatch2, VPFD_SELF_PARAM | VPFD_USE_COUNT | VPFD_RETURN_VALUE, 2); // todo
		//vt->RegisterVMOperatorFunction("wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		//vt->RegisterVMOperatorFunction("waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		//vt->RegisterVMHashOPCode('#', OPCODE_GetHash32, 4, [](const char* str) { return hash::HashT7(str); });
		//vt->RegisterVMHashOPCode('s', OPCODE_GetHash32, 4, [](const char* str) { return hash::HashT7(str); });
		//vt->RegisterDevCall("assert", "assertmsg", "errormsg", "throw", "println");
		//vt->RegisterDatatype("functionptr", "string", "array", "weapon", "int", "float", "vec");
		//vt->RegisterDatatypeRenamed("function", "functionptr");
		vt->SetMaxOpCode(0xFF);
        
		vt->RegisterOpCode(PLATFORM_PC, OPCODE_End, 0x0);
		vt->RegisterOpCode(PLATFORM_PC, OPCODE_Return, 0x1);
		vt->RegisterOpCode(PLATFORM_PC, OPCODE_GetUndefined, 0x2);
		vt->RegisterOpCode(PLATFORM_PC, OPCODE_GetZero, 0x3);
		vt->RegisterOpCode(PLATFORM_PC, OPCODE_GetByte, 0x4);
		vt->RegisterOpCode(PLATFORM_PC, OPCODE_GetNegByte, 0x5);
		vt->RegisterOpCode(PLATFORM_PC, OPCODE_GetUnsignedShort, 0x6);
		vt->RegisterOpCode(PLATFORM_PC, OPCODE_GetNegUnsignedShort, 0x7);
		vt->RegisterOpCode(PLATFORM_PC, OPCODE_GetInteger , 0x8);
		vt->RegisterOpCode(PLATFORM_PC, OPCODE_GetFloat, 0x9);
		vt->RegisterOpCode(PLATFORM_PC, OPCODE_GetString, 0xa);
		vt->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetIString, 0xb);
		/*
		
    { 0x0C, opcode::OP_GetVector },
    { 0x0D, opcode::OP_GetLevelObject },
    { 0x0E, opcode::OP_GetAnimObject },
    { 0x0F, opcode::OP_GetSelf },
    { 0x10, opcode::OP_GetLevel },
    { 0x11, opcode::OP_GetGame },
    { 0x12, opcode::OP_GetAnim },
    { 0x13, opcode::OP_GetAnimation },
    { 0x14, opcode::OP_GetGameRef },
    { 0x15, opcode::OP_GetFunction },
    { 0x16, opcode::OP_CreateLocalVariable },
    { 0x17, opcode::OP_SafeCreateLocalVariables },
    { 0x18, opcode::OP_RemoveLocalVariables },
    { 0x19, opcode::OP_EvalLocalVariableCached },
    { 0x1A, opcode::OP_EvalArray },
    { 0x1B, opcode::OP_EvalLocalArrayRefCached },
    { 0x1C, opcode::OP_EvalArrayRef },
    { 0x1D, opcode::OP_ClearArray },
    { 0x1E, opcode::OP_EmptyArray },
    { 0x1F, opcode::OP_GetSelfObject },
    { 0x20, opcode::OP_EvalFieldVariable },
    { 0x21, opcode::OP_EvalFieldVariableRef },
    { 0x22, opcode::OP_ClearFieldVariable },
    { 0x23, opcode::OP_SafeSetVariableFieldCached },
    { 0x24, opcode::OP_SafeSetWaittillVariableFieldCached },
    { 0x25, opcode::OP_ClearParams },
    { 0x26, opcode::OP_CheckClearParams },
    { 0x27, opcode::OP_EvalLocalVariableRefCached },
    { 0x28, opcode::OP_SetVariableField },
    { 0x29, opcode::OP_CallBuiltin },
    { 0x2A, opcode::OP_CallBuiltinMethod },
    { 0x2B, opcode::OP_Wait },
    { 0x2C, opcode::OP_WaitTillFrameEnd },
    { 0x2D, opcode::OP_PreScriptCall },
    { 0x2E, opcode::OP_ScriptFunctionCall },
    { 0x2F, opcode::OP_ScriptFunctionCallPointer },
    { 0x30, opcode::OP_ScriptMethodCall },
    { 0x31, opcode::OP_ScriptMethodCallPointer },
    { 0x32, opcode::OP_ScriptThreadCall },
    { 0x33, opcode::OP_ScriptThreadCallPointer },
    { 0x34, opcode::OP_ScriptMethodThreadCall },
    { 0x35, opcode::OP_ScriptMethodThreadCallPointer },
    { 0x36, opcode::OP_DecTop },
    { 0x37, opcode::OP_CastFieldObject },
    { 0x38, opcode::OP_CastBool },
    { 0x39, opcode::OP_BoolNot },
    { 0x3A, opcode::OP_BoolComplement },
    { 0x3B, opcode::OP_JumpOnFalse },
    { 0x3C, opcode::OP_JumpOnTrue },
    { 0x3D, opcode::OP_JumpOnFalseExpr },
    { 0x3E, opcode::OP_JumpOnTrueExpr },
    { 0x3F, opcode::OP_Jump },
    { 0x40, opcode::OP_JumpBack },
    { 0x41, opcode::OP_Inc },
    { 0x42, opcode::OP_Dec },
    { 0x43, opcode::OP_Bit_Or },
    { 0x44, opcode::OP_Bit_Xor },
    { 0x45, opcode::OP_Bit_And },
    { 0x46, opcode::OP_Equal },
    { 0x47, opcode::OP_NotEqual },
    { 0x48, opcode::OP_LessThan },
    { 0x49, opcode::OP_GreaterThan },
    { 0x4A, opcode::OP_LessThanOrEqualTo },
    { 0x4B, opcode::OP_GreaterThanOrEqualTo },
    { 0x4C, opcode::OP_ShiftLeft },
    { 0x4D, opcode::OP_ShiftRight },
    { 0x4E, opcode::OP_Plus },
    { 0x4F, opcode::OP_Minus },
    { 0x50, opcode::OP_Multiply },
    { 0x51, opcode::OP_Divide },
    { 0x52, opcode::OP_Modulus },
    { 0x53, opcode::OP_SizeOf },
    { 0x54, opcode::OP_WaitTillMatch },
    { 0x55, opcode::OP_WaitTill },
    { 0x56, opcode::OP_Notify },
    { 0x57, opcode::OP_EndOn },
    { 0x58, opcode::OP_VoidCodePos },
    { 0x59, opcode::OP_Switch },
    { 0x5A, opcode::OP_EndSwitch },
    { 0x5B, opcode::OP_Vector },
    { 0x5C, opcode::OP_GetHash },
    { 0x5D, opcode::OP_RealWait },
    { 0x5E, opcode::OP_VectorConstant },
    { 0x5F, opcode::OP_IsDefined },
    { 0x60, opcode::OP_VectorScale },
    { 0x61, opcode::OP_AnglesToUp },
    { 0x62, opcode::OP_AnglesToRight },
    { 0x63, opcode::OP_AnglesToForward },
    { 0x64, opcode::OP_AngleClamp180 },
    { 0x65, opcode::OP_VectorToAngles },
    { 0x66, opcode::OP_Abs },
    { 0x67, opcode::OP_GetTime },
    { 0x68, opcode::OP_GetDvar },
    { 0x69, opcode::OP_GetDvarInt },
    { 0x6A, opcode::OP_GetDvarFloat },
    { 0x6B, opcode::OP_GetDvarVector },
    { 0x6C, opcode::OP_GetDvarColorRed },
    { 0x6D, opcode::OP_GetDvarColorGreen },
    { 0x6E, opcode::OP_GetDvarColorBlue },
    { 0x6F, opcode::OP_GetDvarColorAlpha },
    { 0x70, opcode::OP_FirstArrayKey },
    { 0x71, opcode::OP_NextArrayKey },
    { 0x72, opcode::OP_ProfileStart },
    { 0x73, opcode::OP_ProfileStop },
    { 0x74, opcode::OP_SafeDecTop },
    { 0x75, opcode::OP_Nop },
    { 0x76, opcode::OP_Abort },
    { 0x77, opcode::OP_Object },
    { 0x78, opcode::OP_ThreadObject },
    { 0x79, opcode::OP_EvalLocalVariable },
    { 0x7A, opcode::OP_EvalLocalVariableRef },
    { 0x7B, opcode::OP_DevblockBegin },
    { 0x7C, opcode::OP_DevblockEnd },
		*/

	}


}
REGISTER_GSC_VM_OPCODES(bo2_01, OpCode);