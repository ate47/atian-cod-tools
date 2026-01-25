#pragma once
#include <core/bytebuffer.hpp>
#include <tools/gsc/gsc_decompiler.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_object.hpp>

namespace tool::gsc::vm {
	class GscGdb;
	class GscVm;

	GscVm* GetGscReader(uint64_t vm);
	GscGdb* GetGdbReader(uint64_t magic);
	void RegisterVmOpCodes();

	class GscGdb {
	public:
		uint64_t magic;
		GscGdb(uint64_t magic) : magic(magic) {}

		virtual void DbgLoad(T8GSCOBJContext& ctx, core::bytebuffer::ByteBuffer& dbgReader, std::ostream& asmout) {
			throw std::runtime_error("DbgLoad not implemented for this vm");
		}
		virtual bool DbgSave(GscDecompilerGDBData* gdb, std::string& buffer) {
			throw std::runtime_error("DbgSave not implemented for this vm");
		}
	};
	class GscVm {
	public:
		uint64_t magic;
		GscVm(uint64_t magic) : magic(magic) {}

		virtual std::shared_ptr<GSCOBJHandler> NewHandler(byte* buff, size_t size) const {
			throw std::runtime_error("NewHandler not implemented for this vm");
		}

		inline std::shared_ptr<GSCOBJHandler> operator()(byte* buff, size_t size) {
			return NewHandler(buff, size);
		}
	};
}