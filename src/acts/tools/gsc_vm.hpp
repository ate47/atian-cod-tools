#pragma once
#include <core/bytebuffer.hpp>
#include <tools/gsc.hpp>

namespace tool::gsc::vm {
	class GscVm {
	public:
		uint64_t vm;
		std::function<std::shared_ptr<GSCOBJHandler>(byte*, size_t)> func;
		GscVm(uint64_t vm, std::function<std::shared_ptr<GSCOBJHandler>(byte*, size_t)> func);
	};
	class GscGdb {
	public:
		uint64_t magic;
		std::function<void(T8GSCOBJContext& ctx, core::bytebuffer::ByteBuffer& dbgReader, std::ostream& asmout)> load;
		std::function<bool(GscDecompilerGDBData* gdb, std::string& data)> saver;
		GscGdb(
			uint64_t magic, 
			std::function<void(T8GSCOBJContext& ctx, core::bytebuffer::ByteBuffer& dbgReader, std::ostream& asmout)> load,
			std::function<bool(GscDecompilerGDBData* gdb, std::string& data)> saver
		);
	};
	class GscVmOpCode {
	public:
		const char* id;
		std::function<void()> func;
		bool priv;
		GscVmOpCode(const char* id, std::function<void()> func, bool priv = false);
	};

	std::function<std::shared_ptr<GSCOBJHandler>(byte*, size_t)>* GetGscReader(uint64_t vm);
	GscGdb* GetGdbReader(uint64_t magic);
	void RegisterVmOpCodes();
}
#define REGISTER_GSC_VM(_vm, cls) static tool::gsc::vm::GscVm __GscVm##_vm(_vm, [](byte* file, size_t fileSize) { return std::make_shared<cls>(file, fileSize); })
#define REGISTER_GDB_HANDLE(magic, func, save) static tool::gsc::vm::GscGdb __GscGdb##magic(magic, func, save)
#define REGISTER_GSC_VM_OPCODES(id, func) static tool::gsc::vm::GscVmOpCode __GscVmOpCode##id(#id, func)
#define REGISTER_GSC_VM_OPCODES_PRIVATE(id, func) static tool::gsc::vm::GscVmOpCode __GscVmOpCode##id(#id, func, true)