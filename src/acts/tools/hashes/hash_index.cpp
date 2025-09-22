#include <includes.hpp>
#include <utils/io_utils.hpp>
#include <deps/miniz.hpp>
#include <deps/scobalula_wni.hpp>
#include <tools/compatibility/acts_acef.hpp>

namespace {
	constexpr const char* HashIndexURL = "https://github.com/ate47/HashIndex/releases/download/release/hashes-all.zip";

	int merge_hash_index(int argc, const char* argv[]);

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
		/*

		const char* argv2[4]{ argv[0], "merge_hash_index" };
		if (tool::NotEnoughParam(argc, 2)) {
			argv2[2] = argv[3];
		}

		return merge_hash_index(argc - 1, argv2);
		//*/
		return tool::OK;
	}

	int merge_hash_index(int argc, const char* argv[]) {
		LOG_INFO("Merging hash index...");

		std::filesystem::path in{ utils::GetProgDir() / "package_index" };
		std::filesystem::path out{ in / "merged_hash.acef" };

		core::memory_allocator::MemoryAllocator alloc{};
		std::map<std::string, std::unordered_set<uint64_t>> hashMap{};

		std::vector<std::filesystem::path> wnis{};

		utils::GetFileRecurseExt(in, wnis, ".wni\0");

		LOG_INFO("Loading wni files...");

		for (const std::filesystem::path& p : wnis) {
			LOG_INFO("Load {}", p.string());
			if (!deps::scobalula::wni::ReadWNIFile(p, [&hashMap](uint64_t hash, const char* str) {
				hashMap[str].insert(hash);
			})) {
				return tool::BASIC_ERROR;
			}
		}

		utils::compress::CompressionAlgorithm alg;
		if (tool::NotEnoughParam(argc, 1)) {
			alg = compatibility::acts_acef::DEFAULT_COMPRESS;
		}
		else {
			alg = utils::compress::GetConfigName(argv[2]);
		}

		LOG_INFO("Building hash acef block...");
		compatibility::acts_acef::AcefBlock hashBlock{ compatibility::acts_acef::CreateACEFBlockHash(hashMap, alloc) };
		compatibility::acts_acef::CompressACEFBlock(&hashBlock, 1, alg, out);

		LOG_INFO("cleanup old files...");

		for (const std::filesystem::path& p : wnis) {
			std::filesystem::remove(p);
			LOG_INFO("Deleted {}", p.string());
		}

		LOG_INFO("done.");

		return tool::OK;
	}

	ADD_TOOL(download_hash_index, "hash", " (url)", "Download the latest hash index", download_hash_index);
	ADD_TOOL(merge_hash_index, "hash", "", "Merge hash index", merge_hash_index);
}