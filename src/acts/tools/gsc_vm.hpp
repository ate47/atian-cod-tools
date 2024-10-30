#pragma once
#include <tools/gsc.hpp>

namespace tool::gsc::vm {
	class GscVm {
	public:
		byte vm;
		std::function<std::shared_ptr<GSCOBJHandler>(byte*, size_t)> func;
		GscVm(byte vm, std::function<std::shared_ptr<GSCOBJHandler>(byte*, size_t)> func);
	};
	class GscVmOpCode {
	public:
		const char* id;
		std::function<void()> func;
		bool priv;
		GscVmOpCode(const char* id, std::function<void()> func, bool priv = false);
	};

	std::function<std::shared_ptr<GSCOBJHandler>(byte*, size_t)>* GetGscReader(byte vm);
	void RegisterVmOpCodes();

}
#define REGISTER_GSC_VM(_vm, cls) static tool::gsc::vm::GscVm __GscVm##_vm(_vm, [](byte* file, size_t fileSize) { return std::make_shared<cls>(file, fileSize); })
#define REGISTER_GSC_VM_OPCODES(id, func) static tool::gsc::vm::GscVmOpCode __GscVmOpCode##id(#id, func)
#define REGISTER_GSC_VM_OPCODES_PRIVATE(id, func) static tool::gsc::vm::GscVmOpCode __GscVmOpCode##id(#id, func, true)