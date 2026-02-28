#include <includes.hpp>
#include <core/bytebuffer.hpp>
#include <utils/compress_utils.hpp>

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
		
		for (size_t i = 0; i < 10; i++) {
			LOG_INFO("{} {:x}={}", i, keys[i], vals[i]);
		}



		return tool::OK;
	}


	ADD_TOOL(dzporter_cdb_read, "compatibility", " [file]", "read cdb file", dzporter_cdb_read);
}