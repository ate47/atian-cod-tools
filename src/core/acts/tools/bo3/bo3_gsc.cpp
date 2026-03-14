#include <includes.hpp>
#include <deps/miniz.hpp>

namespace {

	int bo3gsctest(Process& proc, int argc, const char* argv[]) {
		int count[2];
		proc.ReadMemoryEx(&count, proc[0x50EFB60], sizeof(count));

		struct objFileInfo_t {
			void* active;
			void* baselineVersion;
			uintptr_t filename;
			void* startAddr;
			void* endAddr;
			uintptr_t lineStartAddr; // byte**
			int lineStartAddrCount;
			uintptr_t source;
			int sourceLen;
			uintptr_t gdb;
		}; static_assert(sizeof(objFileInfo_t) == 0x50);

		static objFileInfo_t files[2][500];
		proc.ReadMemoryEx(&files, proc[0x50DC2E0], sizeof(files));

		const char* zone{ proc.ReadStringTmp(proc[0x17A6B860]) };
		LOG_INFO("zone: {}", zone);

		std::filesystem::path gdb{ std::filesystem::path{zone} / "scriptgdb.zip" };
		LOG_INFO("scriptgdb: {}, ex:{}", gdb.string(), std::filesystem::exists(gdb));

		miniz_cpp::zip_file zf{};
		zf.load(gdb.string());

		for (size_t i = 0; i < 2; i++) {
			LOG_INFO("-- {} ({}) --", i ? "client" : "server", count[i]);
			for (size_t j = 0; j < count[i]; j++) {
				objFileInfo_t& file{ files[i][j] };
				const char* fn{ proc.ReadStringTmp(file.filename) };

				if (!file.gdb) {
					const char* gfn{ utils::va("%s.gdb", fn) };
					if (zf.has_file(gfn)) {
						std::string buff{ zf.read(gfn) };
						uint64_t magic{ *(uint64_t*)buff.data() };
						if (magic == 0x13000A0D42444780) {
						//	LOG_ERROR("{} missing: {} 0x{:x}", fn, gfn, magic);
						}
					}
					continue;
				}
				LOG_INFO("{} ({}/{}) gdb:{:x}, lines:{:x}({}), source:{:x}({})", 
					fn, file.active, file.baselineVersion,
					file.gdb, file.lineStartAddr, file.lineStartAddrCount, file.source, file.sourceLen);
			}
		}

		return tool::OK;
	}


	ADD_TOOL(bo3gsctest, "bo3", "", "bo3 gsc test", L"BlackOps3.exe", bo3gsctest);

}