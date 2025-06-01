#include <includes.hpp>
#include <utils/io_utils.hpp>
#include <deps/miniz.hpp>

namespace {
	constexpr const char* HashIndexURL = "https://github.com/ate47/HashIndex/releases/download/release/hashes-all.zip";

	int download_hash_index(int argc, const char* argv[]) {
		const char* idx{ tool::NotEnoughParam(argc, 1) ? HashIndexURL : argv[2] };

		LOG_INFO("Downloading {}...", idx);

		std::vector<byte> buff{};

		if (!utils::io::DownloadFile(idx, buff)) {
			LOG_ERROR("Can't download {}", idx);
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Downloaded {}B, extracting...", utils::FancyNumber(buff.size()));

		miniz_cpp::zip_file zf{ buff };

		std::filesystem::path dir{ utils::GetProgDir() };

		for (miniz_cpp::zip_info& member : zf.infolist()) {
			std::filesystem::path out{ dir / member.filename };
			std::filesystem::create_directories(out.parent_path());
			LOG_INFO("Extracting into {}", out.string());
			
			std::string wni{ zf.read(member.filename) };

			if (!utils::WriteFile(out, wni)) {
				LOG_ERROR("Can't write file");
				continue;
			}
		}

		LOG_INFO("Index updated");
		
		return tool::OK;
	}

	ADD_TOOL(download_hash_index, "hash", " (url)", "Download the latest hash index", download_hash_index);
}