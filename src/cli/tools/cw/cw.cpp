#include <includes.hpp>

char* cw::DecryptString(char* str) {
	// for now I'm using the DLL to call the decrypt function
	if (*reinterpret_cast<BYTE*>(str) == 0x8b) {
		return str + 3; // encryption ptr
	}
	if ((*str & 0xC0) == 0x80) {
		// encrypted
		str[0] = 0;
	}
	return str;
}

namespace {

	int cwtestchecksum(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		std::vector<std::filesystem::path> paths{};
		hashutils::ReadDefaultFile();

		utils::GetFileRecurse(argv[2], paths, [](const std::filesystem::path& p) {
			auto s = p.string();
			return s.ends_with(".gscc");
		});

		std::unordered_map<UINT32, std::vector<UINT64>> data{};

		for (const auto& p : paths) {
			LPVOID buffer{};
			LPVOID bufferAlign{};
			size_t size{};
			size_t sizeAlign{};
			if (!utils::ReadFileAlign(p, buffer, bufferAlign, size, sizeAlign)) {
				std::cerr << "Can't read " << p << "\n";
				continue;
			}

			auto* header = reinterpret_cast<tool::gsc::T9GSCOBJ*>(bufferAlign);

			if (!header->exports_count) {
				continue;
			}

			auto* exports = reinterpret_cast<tool::gsc::T8GSCExport*>(&header->magic[header->exports_tables]);

			if (exports->flags != tool::gsc::T9GSCExportFlags::T9_EF_CLASS_VTABLE) {
				std::cerr << "Bad first element : " << hashutils::ExtractTmp("function", exports->name) << " for " << p << "\n";
				continue;
			}

			data[exports->name].emplace_back(header->name);
		}

		for (auto& [key, vec] : data) {
			if (vec.size() > 1) {
				std::cout << hashutils::ExtractTmp("function", key) << ": \n";

				for (const auto& p : vec) {
					std::cout << hashutils::ExtractTmpPath("script", p) << "\n";
				}

			}
		}

		std::cout << "done\n";

		return tool::OK;
	}


}
#ifndef CI_BUILD

ADD_TOOL("tcrccw", "", "test crc (cw)", nullptr, cwtestchecksum);

#endif