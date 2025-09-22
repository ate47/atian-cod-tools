#include <includes_shared.hpp>
#include <utils/utils.hpp>
#include <utils/hash_mini.hpp>
#include "acts_vm.hpp"
#include "acts_vm_opcodes.hpp"

namespace acts::vm {
	const char* VmVarTypeNames[] {
		"UNDEFINED",
		"THREAD",
		"POINTER",
		"INTEGER",
		"FLOAT",
		"HASH",
		"PRECALL",
		"ARRAY",
		"VECTOR",
		"STRUCT",
	};

	const char* VmVarTypeName(VmVarType type) {
		if (type < ARRAYSIZE(VmVarTypeNames)) {
			return VmVarTypeNames[type];
		}
		return utils::va("UNKNOWN:%d", (int)type);
	}
	ActsVm::ActsVm(ActsVmConfig cfg) : cfg(cfg) {
		*(uint64_t*)defaultScript.magic = acts::vm::ACTSCRIPT_MAGIC;
	}

	void ActsVm::AssertThreadStarted() {
		if (!currentThread) throw std::runtime_error("No thread started");
	}

	VmExecutionThread* ActsVm::AllocThread(byte* codePos) {
		for (VmExecutionThread& thread : threads) {
			if (thread.running) continue;

			thread.stack[0].type = VT_THREAD;
			thread.stack[0].val.ref = thread.threadId;
			thread.stack[1].type = VT_LOCATION;
			thread.stack[1].val.loc = nullptr;
			thread.top = &thread.stack[2];
			thread.waitTime = 0;
			thread.waitFrameTime = 0;
			thread.running = true;
			thread.codePos = codePos;
			thread.frame = thread.frames;
			return currentThread;
		}
		Error("Can't alloc thread", true);
		return nullptr;
	}

	void ActsVm::Execute() {
		AssertThreadStarted();
		if (currentThread->waitFrameTime > 0) {
			if (--currentThread->waitFrameTime) {
				return; // wait using waitframe()
			}
		}
		else if (currentThread->waitFrameTime) {
			if (currentThread->waitFrameTime <= utils::GetTimestamp()) {
				return; // wait using wait()
			}
			else {
				currentThread->waitFrameTime = 0;
			}
		}

		bool terminated{};

		while (!terminated) {
			opcodes::OpCode* base{ currentThread->SetData<opcodes::OpCode>() };
			opcodes::OpCode op{ *base };
			currentThread->codePos = (byte*)(base + 1);

			opcodes::HandleOpCode(op, this, currentThread, &terminated);
		}
	}

	void ActsVm::IncRef(VmVar* var) {
		switch (var->type) {
		case VT_ARRAY: {
			VmArray* v{ (VmArray*)alloc.DataByRef(var->val.ref) };
			v->ref++;
			break;
		}
		case VT_VECTOR: {
			VmVector* v{ (VmVector*)alloc.DataByRef(var->val.ref) };
			v->ref++;
			break;
		}
		case VT_STRING: {
			VmString* v{ (VmString*)alloc.DataByRef(var->val.ref) };
			v->ref++;
			break;
		}
		}
	}

	void ActsVm::DecRef(VmVar* var) {
		switch (var->type) {
		case VT_ARRAY: {
			VmArray* v{ (VmArray*)alloc.DataByRef(var->val.ref) };

			if (!(--v->ref)) {
				// cleanup all nodes

				VmRef node{ v->start };
				while (node) {
					VmArrayNode* v{ (VmArrayNode*)alloc.DataByRef(node) };
					ReleaseVariable(&v->idx);
					ReleaseVariable(&v->var);
					VmRef next = v->next;
					alloc.FreeRef(node);
					node = next;
				}
				alloc.FreeRef(var->val.ref);
			}
			break;
		}
		case VT_STRING: {
			VmString* v{ (VmString*)alloc.DataByRef(var->val.ref) };
			if (!(--v->ref)) {
				alloc.FreeRef(var->val.ref);
			}
			break;
		}
		case VT_VECTOR: {
			VmVector* v{ (VmVector*)alloc.DataByRef(var->val.ref) };

			if (!(--v->ref)) {
				alloc.FreeRef(var->val.ref);
			}
			break;
		}
		}
	}

	void ActsVm::ReleaseVariable(VmVar* var) {
		DecRef(var);
		var->val.i = 0;
		var->type = VT_UNDEFINED;
	}

	VmString* ActsVm::GetStringEntry(VmRef ref) {
		return (VmString*)alloc.DataByRef(ref);
	}

	const char* ActsVm::GetString(VmRef ref) {
		return ActsVm::GetStringEntry(ref)->Str();
	}

	VmRef ActsVm::CreateString(const char* str, size_t numRefs) {
		uint64_t h{ hash::Hash64A(str) };
		size_t idx{ h & (ARRAYSIZE(stringStart) - 1) };

		VmRef current{ stringStart[idx] };
		VmString* prev{};

		// search in the memory the current string
		while (current != INVALID_STR_REF) {
			prev = (VmString*)alloc.DataByRef(current);
			if (prev->hash == h && strcmp(GetString(current), str) == 0) {
				// this string is our string, we can increase the number of refs and return it
				prev->ref += numRefs;
				return current;
			}

			current = prev->next;
		}

		if (!numRefs) {
			// no refs, we can't create a new entry
			Error("Can't create string with no refs,", false);
			return INVALID_STR_REF; 
		}

		// we didn't find our entry, we can allocate a new one
		size_t len{ std::strlen(str) + 1 };
		VmRef ref{ alloc.AllocRef(sizeof(VmString) + len) };
		VmString* entry{ (VmString*)alloc.DataByRef(ref) };
		std::memcpy(entry->Str(), str, len);
		entry->hash = h;
		entry->ref = numRefs;
		entry->next = INVALID_STR_REF;
		entry->prev = current;


		// set the new entry to the previous entry
		if (prev) {
			prev->next = ref;
		}

		return ref;
	}

	bool ActsVm::GetFunctionInfo(byte* codePos, ScriptExport** exp, ActScript** script) {
		for (size_t i = 0; i < linkedScriptsCount; i++) {
			ScriptInfo& nfo{ linkedScripts[i] };

			if (!nfo.script->IsInScript(codePos)) {
				if (script) *script = nfo.script;
				uint32_t rloc = (uint32_t)(codePos - nfo.script->magic);

				if (exp) {
					*exp = nullptr;
					ScriptExport* e{ nfo.script->Exports() };
					ScriptExport* ee{ nfo.script->ExportsEnd() };

					while (e != ee) {
						if (
							e->address > rloc // too far
							&& (!*exp || (*exp)->address >= e->address) // before previous
							) {
							*exp = e;
						}
					}

					return *exp;
				}

				return true;
			}
		}

		return false;
	}
	void ActsVm::CleanupThread(VmExecutionThread* thread) {
		// todo
	}

	bool ActsVm::CastToBool(VmVar* var) {
		if (var->type == VT_INTEGER) {
			var->val.i = var->val.i != 0;
			return var->val.i;
		}
		if (var->type == VT_FLOAT) {
			var->type = VT_INTEGER;
			var->val.i = var->val.f != 0;
			return var->val.i;
		}
		Error(utils::va("Can't cast var to bool: type %s", VmVarTypeName(var->type)), false);
		ReleaseVariable(var);
		return false;
	}

	void ActsVm::Error(const char* msg, bool terminate) {
		if (currentThread) {
			ScriptExport* exp{};
			ActScript* script{};
			if (GetFunctionInfo(currentThread->codePos, &exp, &script)) {
				msg = utils::va("\n[%s<%s>::%s]", this->cfg.hashToString(exp->name_space), script->GetName(), cfg.hashToString(exp->name));
			}
		}
		if (terminate) {
			throw std::runtime_error(msg);
		}
		else {
			LOG_WARNING("{}", msg);
		}
	}

	void ActsVm::RegisterBuiltin(BuiltinCallback callback, uint64_t name, uint32_t minArgs, uint32_t maxArgs, bool isMethod) {
		BuiltinCall* call;
		if (isMethod) {
			if (callMethodsCount == ARRAYSIZE(callMethods)) {
				throw std::runtime_error("Can't register more function builtin, increase the limit in acts_vm.hpp");
			}
			call = &callMethods[callMethodsCount++];
		}
		else {
			if (callFuncsCount == ARRAYSIZE(callFuncs)) {
				throw std::runtime_error("Can't register more method builtin, increase the limit in acts_vm.hpp");
			}
			call = &callFuncs[callFuncsCount++];
		}
		call->callback = callback;
		call->minArgs = minArgs;
		call->maxArgs = maxArgs;
		call->name = name;
	}
	
	uint32_t ActsVm::FindScriptFunction(const char* file, uint64_t namesp, uint64_t name) {
		ScriptInfo* si{};
		if (LinkScript(file, &si) < 0) {
			return BAD_SCRIPTFUNC_ID;
		}

		ScriptExport* exps{ si->script->Exports() };
		for (size_t i = 0; i < si->script->exports_count; i++) {
			if (exps[i].flags & ScriptExportFlags::SEF_PRIVATE) {
				continue; // ignore private exports
			}
			if (exps[i].name_space == namesp && exps[i].name == name) {
				size_t id{ GetScriptInfoId(si) };
				return (uint32_t)(id << 16 | i);
			}
		}

		return BAD_SCRIPTFUNC_ID;
	}
	
	BuiltinCall* ActsVm::GetBuiltin(uint16_t builtin) {
		if (builtin == BAD_SCRIPTFUNC_ID) {
			return nullptr;
		}
		if (builtin & FLAG_BUILTIN_METHOD) {
			return &callMethods[builtin & ~FLAG_BUILTIN_METHOD];
		}
		else {
			return &callFuncs[builtin];
		}
	}

	uint16_t ActsVm::FindBuiltin(uint64_t name) {
		uint16_t id{ FindBuiltinMethod(name) };
		if (id != BAD_BUILTIN_ID) {
			return id;
		}
		return FindBuiltinFunction(name);
	}
	
	uint16_t ActsVm::FindBuiltinFunction(uint64_t name) {
		return BAD_BUILTIN_ID;
	}

	uint16_t ActsVm::FindBuiltinMethod(uint64_t name) {
		for (size_t i = 0; i < callMethodsCount; i++) {
			if (callMethods[i].name == name) return (uint16_t)(i | FLAG_BUILTIN_METHOD);
		}
		return BAD_BUILTIN_ID;
	}

	VmVar* ActsVm::PushStack(size_t count) {
		AssertThreadStarted();
		if (!count) return currentThread->top;
		if (currentThread->top + count > &currentThread->stack[ARRAYSIZE(currentThread->stack)]) {
			Error("Invalid push: too much data", true);
		}
		VmVar* ptr = currentThread->top;
		currentThread->top += count;
		return ptr;
	}

	void ActsVm::PopStack(size_t count) {
		AssertThreadStarted();
		if (!count) return;
		if (currentThread->top - count < &currentThread->stack[0]) {
			Error("Invalid pop: not enough data", true);
		}

		currentThread->top -= count;
	}

	void ActsVm::AddInt(int64_t val) {
		VmVar* ptr = PushStack();
		ptr->type = VT_INTEGER;
		ptr->val.i = val;
	}

	void ActsVm::AddFloat(float val) {
		VmVar* ptr = PushStack();
		ptr->type = VT_FLOAT;
		ptr->val.f = val;
	}

	void ActsVm::AddHash(uint64_t val) {
		VmVar* ptr = PushStack();
		ptr->type = VT_HASH;
		ptr->val.hash = val;
	}

	void ActsVm::AddArray() {
		VmRef ref{ alloc.AllocRef(sizeof(VmArray)) };
		VmVar* ptr = PushStack();
		ptr->type = VT_ARRAY;
		ptr->val.ref = ref;
		IncRef(ptr);
	}

	void ActsVm::AddStruct() {
		AddArray(); // wip?
	}

	void ActsVm::AddVector(float* vec) {
		VmRef ref{ alloc.AllocRef(sizeof(VmVector)) };

		VmVector* v{ (VmVector*)alloc.DataByRef(ref) };
		std::memcpy(v->vec, vec, sizeof(v->vec));

		VmVar* ptr = PushStack();
		ptr->type = VT_VECTOR;
		ptr->val.ref = ref;
		IncRef(ptr);
	}

	void ActsVm::AddString(const char* str) {
		AddStringRef(CreateString(str));
	}

	void ActsVm::AddStringRef(VmRef ref) {
		VmVar* ptr = PushStack();
		ptr->type = VT_STRING;
		ptr->val.ref = ref;
		IncRef(ptr);
	}

	void ActsVm::LoadScript(const char* name) {
		linkGroup++;

		if (LinkScript(name, nullptr) < 0) {
			throw std::runtime_error(utils::va("Can't load script %s", name));
		}

		while (topScripts < linkedScriptsCount) {
			ScriptInfo& nfo{ linkedScripts[topScripts++] };


			ScriptExport* exports = nfo.script->Exports();

			while (exports != nfo.script->ExportsEnd()) {
				if (exports->flags & ScriptExportFlags::SEF_AUTOEXEC) {
					currentThread = AllocThread(nfo.script->magic + exports->address);
					Execute();
				}
			}
		}
	}


	size_t ActsVm::GetScriptInfoId(const ScriptInfo* info) {
		return info - linkedScripts;
	}

	int ActsVm::LinkScript(const char* name, ScriptInfo** oInfo) {
		return LinkActsScript(GetActsScript(name), oInfo);
	}

	ActScript* ActsVm::GetActsScript(const char* name) {
		ActScript* script = cfg.getterFunction(name);

		if (!script) {
			Error(utils::va("Can't find script %s", name), true);
			return &defaultScript;
		}

		return script;
	}

	uint32_t ActsVm::GetScriptExport(ScriptInfo* origin, uint64_t nsp, uint64_t name) {
		ScriptExport* exp{ origin->script->Exports() };
		// search inside the origin script
		for (size_t i = 0; i < origin->script->exports_count; i++) {
			if (exp->name == name && exp->name_space == nsp) {
				size_t id{ GetScriptInfoId(origin) };
				return (uint32_t)(id << 16 | i);
			}
		}

		// search inside the includes
		uint32_t* includes{ origin->script->Includes() };
		for (size_t i = 0; i < origin->script->includes_count; i++) {
			uint32_t ref{ FindScriptFunction(origin->script->GetString(includes[i]), nsp, name) };
			if (ref != BAD_SCRIPTFUNC_ID) {
				return ref;
			}
		}

		return BAD_SCRIPTFUNC_ID;

	}

	int ActsVm::LinkActsScript(ActScript* script, ScriptInfo** oInfo) {
		if (*(uint64_t*)script != ACTSCRIPT_MAGIC) {
			LOG_ERROR("Invalid magic for script");
			return ActsVmLinkOutput::VMLO_CANT_FIND;
		}

		// search if this script is already linked, assume that each script is unique
		for (size_t i = 0; i < linkedScriptsCount; i++) {
			if (linkedScripts[i].script == script) {
				if (oInfo) *oInfo = &linkedScripts[i];
				return ActsVmLinkOutput::VMLO_NOTHING;
			}
		}

		// it is not linked, we need to allocate a new entry
		if (linkedScriptsCount == ARRAYSIZE(linkedScripts)) {
			LOG_ERROR("Too many linked scripts");
			return ActsVmLinkOutput::VMLO_ERROR_TOO_MANY;
		}

		ScriptInfo& nfo{ linkedScripts[linkedScriptsCount++] };
		if (oInfo) *oInfo = &nfo;
		nfo.script = script;
		nfo.group = linkGroup;

		// link script
		uint32_t* includes{ script->Includes() };
		for (size_t i = 0; i < script->includes_count; i++) {
			if (LinkScript(script->GetString(includes[i]), nullptr) < 0) {
				return ActsVmLinkOutput::VMLO_ERROR_INCLUDES;
			}
		}

		// link strings
		ScriptString* strings{ (ScriptString*)&script->magic[script->strings_table] };
		for (size_t i = 0; i < script->strings_count; i++) {
			uint32_t* refs{ (uint32_t*)&strings[1] };

			VmRef ref{ CreateString(script->GetString(strings->address), strings->num_address) };

			for (size_t j = 0; j < strings->num_address; j++) {
				*(VmRef*)&script->magic[refs[j]] = ref;
			}
			
			strings = (ScriptString*)&refs[strings->num_address];
		}

		// link functions
		ScriptImport* imports{ (ScriptImport*)&script->magic[script->imports_table] };
		for (size_t i = 0; i < script->imports_count; i++) {
			uint32_t* refs{ (uint32_t*)&imports[1] };

			switch (imports->flags & ScriptImportFlags::SIF_TYPE_MASK) {
			case SIF_GET:
			case SIF_FUNCTION:
			case SIF_FUNCTION_THREAD:
			case SIF_FUNCTION_CHILDTHREAD:
			case SIF_METHOD:
			case SIF_METHOD_THREAD:
			case SIF_METHOD_CHILDTHREAD: {
				uint32_t ref{ GetScriptExport(&nfo, imports->name_space, imports->name) };
				if (ref == BAD_SCRIPTFUNC_ID) {
					Error(utils::va("Can't find script export %x::%x included by %s", imports->name_space, imports->name, script->GetName()), true);
				}

				for (size_t j = 0; j < imports->num_address; j++) {
					*(uint32_t*)&script->magic[refs[j]] = ref;
				}
				break;
			}
			case SIF_GET_BUILTIN: {
				bool method{};
				uint16_t ref{ FindBuiltin(imports->name) };
				if (ref == BAD_BUILTIN_ID) {
					Error(utils::va("Can't find builtin %x included by %s", imports->name, script->GetName()), true);
				}

				for (size_t j = 0; j < imports->num_address; j++) {
					*(uint16_t*)&script->magic[refs[j]] = ref;
				}
				break;
			}
			case SIF_FUNCTION_BUILTIN: {
				uint16_t ref{ FindBuiltinFunction(imports->name) };
				if (ref == BAD_BUILTIN_ID) {
					Error(utils::va("Can't find builtin function %x included by %s", imports->name, script->GetName()), true);
				}

				for (size_t j = 0; j < imports->num_address; j++) {
					*(uint16_t*)&script->magic[refs[j]] = ref;
				}
				break;
			}
			case SIF_METHOD_BUILTIN: {
				uint16_t ref{ FindBuiltinMethod(imports->name) };
				if (ref == BAD_BUILTIN_ID) {
					Error(utils::va("Can't find builtin method %x included by %s", imports->name, script->GetName()), true);
				}

				for (size_t j = 0; j < imports->num_address; j++) {
					*(uint16_t*)&script->magic[refs[j]] = ref;
				}
				break;
			}
			default:
				Error(utils::va("Can't link import with bad flags, nsp_%x:::func_%x", imports->name, imports->name_space), true);
				break;
			}

			imports = (ScriptImport*)&refs[imports->num_address];
		}

		return 1 + script->includes_count;
	}
}
