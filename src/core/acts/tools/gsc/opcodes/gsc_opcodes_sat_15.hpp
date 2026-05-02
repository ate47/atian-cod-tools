#pragma once

namespace tool::gsc::opcode {
	class OpCodeSat15Registry {
	public:
		static void OpCode() {
			VmInfo* t1015 = RegisterVM(VMI_SAT_15, "Call of Duty: Black Ops 7 (15)", "t11", "bo7_15", VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW_KEYS | VmFlags::VMF_EXPORT_CRC32 | VmFlags::VMF_HASH64 | VmFlags::VMF_HASH_CER | VmFlags::VMF_HASH_PATH_IW | VmFlags::VMF_CALL_NO_PARAMS | VmFlags::VMF_IW_LIKE | VmFlags::VMF_ISTRING_HASHED); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS | VmFlags::VMF_NO_PLATFORM
			t1015->RegisterVmName("sat15", "blackops7_15", "bo7_15");
			t1015->AddPlatform(PLATFORM_PC);
			t1015->RegisterVMGlobalVariable("level", OPCODE_IW_GetLevel);
			t1015->RegisterVMGlobalVariable("game", OPCODE_IW_GetGame);
			t1015->RegisterVMGlobalVariable("anim", OPCODE_IW_GetAnim);
			t1015->RegisterVMOperatorFunction("waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
			t1015->RegisterVMOperatorFunction("isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
			t1015->RegisterVMOperatorFunction("notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
			t1015->RegisterVMOperatorFunction("endon", "<caller> endon(event+)", OPCODE_EndOn, VPFD_SELF_PARAM | VPFD_USE_COUNT, 1);
			t1015->RegisterVMOperatorFunction("wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
			t1015->RegisterVMOperatorFunction("waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
			t1015->RegisterVMOperatorFunction("getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
			t1015->RegisterVMOperatorFunction("istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
			t1015->RegisterVMOperatorFunction("flat_args", "flat_args(array, count) -> bool", OPCODE_T10_FlatArgs, VPFD_RETURN_VALUE, 2, 2);
			t1015->RegisterVMOperatorFunction("waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
			t1015->RegisterVMHashOPCode('#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
			t1015->RegisterVMHashOPCode('@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hash::HashIWDVar(str); });
			t1015->RegisterVMHashOPCode('%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hash::HashIWAsset(str); });
			t1015->RegisterVMHashOPCode('t', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hash::HashX32(str); });
			t1015->RegisterVMHashOPCode('s', OPCODE_T10_GetScrHash, 8, [](const char* str) { return hash::HashT10Scr(str); });
			t1015->RegisterVMHashOPCode('o', OPCODE_SAT_GetOmnVarHash, 8, [](const char* str) { return hash::HashT10OmnVar(str); });
			t1015->RegisterDevCall("assert", "assertmsg", "projectplanning_assertmsgcerberusorlater", "assertex", "println", "print");
			t1015->RegisterDatatype("builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector", "xhash", "xhashasset", "istring");
		}
	};
}