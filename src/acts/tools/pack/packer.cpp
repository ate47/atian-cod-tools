#include <includes.hpp>
#include <numeric>

namespace tools::packer {

	enum PackAssetType {
		TYPE_UNKNOWN = 0,
		TYPE_TABLE_FILE,
		TYPE_CSV,
		TYPE_GSC,
		TYPE_CONFIG,
		TYPE_DDL,

	};

	void ReadFiles(const std::filesystem::path& path) {
		std::vector<std::filesystem::path> files{};
		std::vector<std::filesystem::path> tables{};

		utils::GetFileRecurse(path, files);
		auto it = files.begin();

		while (it != files.end()) {
			std::filesystem::path rel = std::filesystem::relative(*it, path);

			auto it2 = rel.begin();

			if (it2 == rel.end()) {
				it++;
				continue; // bad file?
			}

			PackAssetType type{};

			if (*it2 == L"tables") {
				it = files.erase(it);
				it2++;
				if (it2 == rel.end()) {
					continue; // bad file?
				}

				std::string typeName = utils::WStrToStr(*it2);

				it2++;
				if (it2 == rel.end()) {
					continue; // bad file?
				}

				// todo: fetch the right type
				type = TYPE_TABLE_FILE;
				std::filesystem::path fileName{ std::accumulate(it2, rel.end(), std::filesystem::path{}, std::divides{}) };
				LOG_INFO("file {} -> table {} : {}", rel.string(), typeName, fileName.string());

				continue;
			}

			
			std::filesystem::path extP = rel.extension();
			std::string ext = extP.string();

			bool parsed{};
			if (!_strcmpi(".csv", ext.data())) {


				type = TYPE_CSV;
			}
			else if (!_strcmpi(".gsc", ext.data()) || !_strcmpi(".csc", ext.data())) {


				type = TYPE_GSC;

			}
			else if (!_strcmpi(".afg", ext.data())) {


				type = TYPE_CONFIG;

			}
			else if (!_strcmpi(".ddl", ext.data())) {


				type = TYPE_DDL;
			}

			if (!parsed) {
				it++;
				continue; // unknown type
			}
			

			LOG_INFO("rel: {} {}", rel.string(), (int)type);
			it++;
		}
	}

	int packer(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		ReadFiles(argv[2]);

		return tool::OK;
	}
#ifndef CI_BUILD
	ADD_TOOL("packer", "dev", " [dir]", "", nullptr, packer);
#endif
}