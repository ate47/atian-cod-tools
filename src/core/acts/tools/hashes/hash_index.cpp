#include <includes.hpp>
#include <utils/io_utils.hpp>
#include <cli/cli_options.hpp>
#include <deps/miniz.hpp>
#include <deps/scobalula_wni.hpp>
#include <deps/dzporter_cdb.hpp>
#include <tools/compatibility/acts_acef.hpp>

namespace {
	// for now the cdb loading has the lowest memory footprint, but none are any faster
	constexpr const char* HashIndexURL = "https://github.com/ate47/HashIndex/releases/download/release-cdb/hashes-all.zip";

	int merge_hash_index(int argc, const char* argv[]);

	const char* knownExtensions[]{
		".acef", ".wni", ".cdb"
	};

	struct HashIndex {
		const char* id;
		const char* desc;
		const char* url;
		bool ignored{};
		const char* path{};
	} hashIndexes[]{
		{ "acts", "default HashIndex", HashIndexURL, true },
		{ "acts-wni", "HashIndex wni format", "https://github.com/ate47/HashIndex/releases/download/release/hashes-all.zip", true },
		{ "acts-cdb", "HashIndex cdb format", "https://github.com/ate47/HashIndex/releases/download/release-cdb/hashes-all.zip" },
		{ "acts-acef", "HashIndex acef format", "https://github.com/ate47/HashIndex/releases/download/release-acef/hashes-all.zip", true },
		{ "echo000-cndb", "echo000/cod-name-db package", "https://github.com/echo000/cod-name-db/releases/latest/download/hash_pkg.zip", false, "package_index"},
	};

	HashIndex* FindIndex(const char* name) {
		for (HashIndex& hi : hashIndexes) {
			if (!_strcmpi(name, hi.id)) {
				return &hi;
			}
		}
		return nullptr;
	}

	void CleanupPrevious(std::filesystem::path path) {
		std::filesystem::path curr{ path };
		for (const char* ke : knownExtensions) {
			path.replace_extension(ke);
			if (std::filesystem::remove(path) && curr != path) {
				LOG_INFO("remove previous file: {}", path.string());
			}
		}
	}

	void DownloadIndex(const char* idx, const char* path) {
		LOG_INFO("Downloading {}...", idx);

		std::vector<byte> buff{};

		if (!utils::io::DownloadFile(idx, buff)) {
			throw std::runtime_error(std::format("Can't download {}", idx));
		}

		LOG_INFO("Downloaded {}B, extracting...", utils::FancyNumber(buff.size()));

		miniz_cpp::zip_file zf{ buff };

		std::filesystem::path dir{ utils::GetProgDir() };
		if (path) {
			dir = dir / path;
		}

		for (miniz_cpp::zip_info& member : zf.infolist()) {
			std::filesystem::path out{ dir / member.filename };
			std::filesystem::create_directories(out.parent_path());
			CleanupPrevious(out);
			LOG_INFO("Extracting into {}", out.string());

			std::string hashFileData{ zf.read(member.filename) };

			if (!utils::WriteFile(out, hashFileData)) {
				LOG_ERROR("Can't write file {}", out.string());
				continue;
			}
		}
	}

	int download_hash_index(int argc, const char* argv[]) {
		cli::options::CliOptions opts{};
		bool showHelp{};
		bool showIndexes{};
		bool mergeAfter{};
		bool allIndexes{};
		const char* compressionFormat{};
		const char* path{};
		opts.addOption(&showHelp, "show help", "--help", "", "-h");
		opts.addOption(&showIndexes, "print know hash indexes", "--list", "", "-l");
		opts.addOption(&allIndexes, "download all indexes", "--all", "", "-a");
		opts.addOption(&mergeAfter, "merge hashes after download", "--merge", "", "-m");
		opts.addOption(&compressionFormat, "compression to merge", "--merge-comp", "", "-c");
		opts.addOption(&path, "path to extract", "--path", "", "-p");

		if (!opts.ComputeOptions(2, argc, argv) || showHelp) {
			opts.PrintOptions();
			return showHelp ? tool::OK : tool::BAD_USAGE;
		}

		if (showIndexes) {
			LOG_INFO("hash indexes: ({})", ACTS_ARRAYSIZE(hashIndexes));
			for (HashIndex& hi : hashIndexes) {
				LOG_INFO("- {} : {}", hi.id, hi.desc);
			}
			return tool::OK;
		}



		if (allIndexes) {
			for (HashIndex& hi : hashIndexes) {
				if (hi.ignored) {
					continue;
				}
				DownloadIndex(hi.url, hi.path);
			}
		}
		else if (opts.NotEnoughParam(1)) {
			DownloadIndex(HashIndexURL, nullptr);
		}
		else {
			for (size_t i = 0; i < opts.ParamsCount(); i++) {
				HashIndex* idx{ FindIndex(opts[i]) };
				if (idx) {
					DownloadIndex(idx->url, idx->path);
				}
				else {
					DownloadIndex(opts[i], nullptr);
				}
			}
		}
		LOG_INFO("Index updated");
		
		if (mergeAfter) {
			const char* argv2[4]{ argv[0], "merge_hash_index", compressionFormat, nullptr };

			return merge_hash_index(compressionFormat ? 3 : 2, argv2);
		}
		return tool::OK;
	}

	int merge_hash_index(int argc, const char* argv[]) {
		LOG_INFO("Merging hash index...");

		std::filesystem::path in{ utils::GetProgDir() / "package_index" };
		std::filesystem::path out{ in / "merged_hash.acef" };
		std::filesystem::create_directories(in);
		std::filesystem::remove(out);

		core::memory_allocator::MemoryAllocator alloc{};
		std::map<std::string, std::unordered_set<uint64_t>> hashMap{};

		std::vector<std::filesystem::path> wnis{};
		std::vector<std::filesystem::path> cdbs{};
		utils::GetFileRecurseExt(in, wnis, ".wni\0");
		utils::GetFileRecurseExt(in, cdbs, ".cdb\0");

		LOG_INFO("Loading wni files...");

		for (const std::filesystem::path& p : wnis) {
			LOG_INFO("Load {}", p.string());
			if (!deps::scobalula::wni::ReadWNIFile(p, [&hashMap](uint64_t hash, const char* str) {
				hashMap[str].insert(hash);
				})) {
				return tool::BASIC_ERROR;
			}
		}
		for (const std::filesystem::path& p : cdbs) {
			LOG_INFO("Load {}", p.string());
			if (!deps::dzporter::cdb::ReadCDBFile(p, [&hashMap](uint64_t hash, const char* str) {
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
		for (const std::filesystem::path& p : cdbs) {
			std::filesystem::remove(p);
			LOG_INFO("Deleted {}", p.string());
		}

		LOG_INFO("done.");

		return tool::OK;
	}

	ADD_TOOL(download_hash_index, "hash", " (url)", "Download the latest hash index", download_hash_index);
	ADD_TOOL(merge_hash_index, "hash", "", "Merge hash index", merge_hash_index);
}