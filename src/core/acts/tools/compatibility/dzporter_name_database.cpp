#include <includes.hpp>
#include <core/bytebuffer.hpp>
#include <rapidcsv.h>
#include <utils/compress_utils.hpp>
#include <deps/dzporter_cdb.hpp>

namespace {
	constexpr uint32_t NDH_HEADER = 0x42444E50;

	struct NameDatabaseHeader {
		uint32_t magic;
		uint32_t entries;
		uint32_t compressed_size;
		uint32_t decompressed_size;
	};


	int dzporter_cdb_read(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) {
			return tool::BAD_USAGE;
		}

		std::filesystem::path path{ argv[2] };
		std::filesystem::path out;
		if (tool::NotEnoughParam(argc, 2)) {
			out = path;
			out.replace_extension(".csv");
		}
		else {
			out = argv[3];
		}

		std::vector<byte> data{ utils::ReadFile<std::vector<byte>>(path) };
		core::bytebuffer::ByteBuffer reader{ data };


		NameDatabaseHeader header;
		reader.Read(&header, sizeof(header));

		if (header.magic != NDH_HEADER) {
			throw std::runtime_error(std::format("invalid magic for {}: {:x}", path.string(), header.magic));
		}

		LOG_INFO("entries: 0x{:x}", header.entries);
		LOG_INFO("compressed_size: 0x{:x}", header.compressed_size);
		LOG_INFO("decompressed_size: 0x{:x}", header.decompressed_size);

		if (!header.entries) {
			return tool::OK; // nothing to read
		}

		std::unique_ptr<byte[]> decompressed{ std::make_unique<byte[]>(header.decompressed_size) };

		int r{ utils::compress::Decompress2(
			utils::compress::COMP_LZ4,
			decompressed.get(), header.decompressed_size,
			reader.ReadPtr<byte>(header.compressed_size), header.compressed_size) };

		if (r < 0) {
			throw std::runtime_error(std::format("can't decompress {}: {}", path.string(), utils::compress::DecompressResultName(r)));
		}

		core::bytebuffer::ByteBuffer dreader{ decompressed.get(), header.decompressed_size };

		std::unique_ptr<const char*[]> vals{ std::make_unique<const char*[]>(header.entries) };

		for (size_t i = 0; i < header.entries; i++) {
			vals[i] = dreader.ReadString();
		}

		std::unique_ptr<uint64_t[]> keys{ dreader.ReadArray<uint64_t>(header.entries) };
		
		utils::OutFileCE os{ out, true };

		for (size_t i = 0; i < header.entries; i++) {
			os << std::hex << keys[i] << "," << vals[i] << "\n";
		}

		LOG_INFO("dump into {}", out.string());

		return tool::OK;
	}

	int dzporter_cdb_gen_csv(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) {
			return tool::BAD_USAGE;
		}
		std::filesystem::path csv{ argv[2] };


		std::filesystem::path out;
		if (tool::NotEnoughParam(argc, 2)) {
			out = csv;
			out.replace_extension(".cdb");
		}
		else {
			out = argv[3];
		}
		LOG_DEBUG("Reading HASH CSV {}", csv.string());
		std::string buffer{};

		if (!utils::ReadFile(csv, buffer)) {
			LOG_WARNING("Can't read hash csv {}", csv.string());
			return tool::BASIC_ERROR;
		}


		rapidcsv::Document doc{};

		std::stringstream stream{ buffer };

		doc.Load(stream, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(','));

		if (doc.GetColumnCount() < 2) {
			LOG_WARNING("Can't read hash csv {}: invalid file", csv.string());
			return tool::BASIC_ERROR;
		}

		std::map<std::string, std::unordered_set<uint64_t>> dataMap{};

		for (size_t i = 0; i < doc.GetRowCount(); i++) {
			const std::string hash = doc.GetCell<std::string>(0, i);
			const std::string value = doc.GetCell<std::string>(1, i);


			uint64_t hashVal;
			try {
				hashVal = std::strtoull(hash.c_str(), nullptr, 16);
			}
			catch (std::runtime_error& e) {
				LOG_WARNING("Error when reading {}: invalid line {}: {}", csv.string(), i, e.what());
				continue;
			}
			dataMap[value].insert(hashVal);
		}

		deps::dzporter::cdb::CompressCDBFile(dataMap, out);

		return tool::OK;
	}

	int dzporter_cdb_recompress(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) {
			return tool::BAD_USAGE;
		}
		std::filesystem::path in{ argv[2] };
		std::filesystem::path out{ argv[3] };

		std::vector<std::filesystem::path> files{};

		utils::GetFileRecurseExt(in, files, ".cdb\0", true);

		for (std::filesystem::path& file : files) {
			std::filesystem::path o{ out / file };
			std::filesystem::create_directories(o.parent_path());
			std::map<std::string, std::unordered_set<uint64_t>> dataMap{};

			LOG_INFO("computing {}", o.string());

			deps::dzporter::cdb::ReadCDBFile(in / file, [&dataMap](uint64_t hash, const char* str) {
				dataMap[str].insert(hash);
			});

			deps::dzporter::cdb::CompressCDBFile(dataMap, out / file);
		}



		return tool::OK;
	}


	ADD_TOOL(dzporter_cdb_read, "compatibility", " [file]", "read cdb file", dzporter_cdb_read);
	ADD_TOOL(dzporter_cdb_recompress, "compatibility", " [in] [out]", "read cdb file", dzporter_cdb_recompress);
	ADD_TOOL(dzporter_cdb_gen_csv, "compatibility", " [input] (output=input.cbd)", "Gen CDB file from csv", dzporter_cdb_gen_csv);
}