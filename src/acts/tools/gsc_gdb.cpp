#include <includes.hpp>
#include <utils.hpp>
#include "gsc_gdb.hpp"

namespace tool::gsc::gdb {
	namespace {
		int gscgdbinfo(Process& proc, int argc, const char* argv[]) {
			std::vector<std::filesystem::path> paths{};

			for (size_t i = 2; i < argc; i++) {
				utils::GetFileRecurse(argv[i], paths, [](const std::filesystem::path& path) -> bool {
					auto str = path.string();
					return str.ends_with(".gdb");
				});
			}

			if (paths.empty()) {
				LOG_ERROR("Can't find gdb any file");
				return tool::BASIC_ERROR;
			}

			std::string buffer{};


			std::unordered_set<std::string> cannon{};
			for (auto& path : paths) {
				if (!utils::ReadFile(path, buffer)) {
					LOG_ERROR("Can't read {}", path.string());
					continue;
				}

				auto* obj = reinterpret_cast<GSC_GDB*>(buffer.data());

				if (*reinterpret_cast<decltype(MAGIC)*>(obj->magic) != MAGIC) {
					LOG_ERROR("Can't read {}: Bad magic", path.string());
					continue;
				}

				if (obj->magic != utils::Aligned<uint64_t>(obj->magic)) {
					LOG_ERROR("Not aligned, todo");
					continue;
				}

				LOG_INFO("Read {}...", path.string());

				//LOG_INFO("crc: {:x}, version: {}", obj->source_crc, obj->version);

				auto* lineInfo = reinterpret_cast<GSC_LINEINFO*>(utils::Aligned<uint64_t>(obj->magic + obj->lineinfo_offset));

				for (size_t i = 0; i < obj->lineinfo_count; i++) {
					//LOG_INFO("Line {} -> 0x{:x}", i, lineInfo[i].offset);
				}

				char* strings = reinterpret_cast<char*>(obj->magic + obj->stringtable_offset);

				for (size_t i = 0; i < obj->stringtable_count; i++) {
					//LOG_INFO("Str {:x} {}", hashutils::Hash32(strings), strings);

					cannon.insert(strings);
					strings += strlen(strings) + 1;
				}
			}

			std::ofstream outstr{ "gdbstrings.txt" };

			if (outstr) {
				for (auto& can : cannon) {
					outstr << can << "\n";
				}
				outstr.close();
			}

			return tool::OK;
		}

	}


	ADD_TOOL("gscgdbinfo", "gsc", "", "reading GSC GDB file", nullptr, gscgdbinfo);
}