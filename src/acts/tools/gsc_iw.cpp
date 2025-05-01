#include <includes.hpp>
#include <extension/acts_extension.hpp>
#include <tools/utils/compress_utils.hpp>
#include <tools/compatibility/acts_acef.hpp>
#include <core/bytebuffer.hpp>
#include <rapidcsv.h>
#include "gsc_iw.hpp"

namespace tool::gsc::iw {
	struct StoreCache {
		// cache that to avoid having to load them multiple times for the same VM
		uint64_t last{};
		acts::extension::AcefArray* arr{};
		size_t count{};
	};

	static const char* GetDataFromCache(StoreCache& cache, uint64_t vm, const char* name, uint32_t str) {
		if (cache.last != vm) {
			const char* f{ utils::va("acts.gscbin%s.%llx", name, vm) };
			acts::extension::GetExtensionData(f, &cache.arr, &cache.count);
			cache.last = vm;
			if (!cache.count) {
				LOG_WARNING("Can't find any data for file {}", f);
			}
		}
		for (size_t i = 0; i < cache.count; i++) {
			core::bytebuffer::ByteBuffer reader{ (byte*)cache.arr[i].data, cache.arr[i].len };
			uint32_t count{ reader.Read<uint32_t>() };
			if (str >= count) continue; // string too high
			uint32_t loc{ reader.ReadPtr<uint32_t>(count)[str] };
			if (!loc) continue; // not defined in this ext

			reader.Skip(loc);
			return reader.ReadString();
		}
		return nullptr;
	}

	const char* GetOpaqueStringForVm(uint64_t vm, uint32_t str, bool useDefault) {
		static thread_local StoreCache lastVmData;
		const char* s{ GetDataFromCache(lastVmData, vm, "opaque", str) };
		return !s && useDefault ? utils::va("ref_%04x", str) : s;
	}
	const char* GetMethodForVm(uint64_t vm, uint32_t str, bool useDefault) {
		static thread_local StoreCache lastVmData;
		const char* s{ GetDataFromCache(lastVmData, vm, "method", str) };
		return !s && useDefault ? utils::va("method_%04x", str) : s;
	}
	const char* GetFunctionForVm(uint64_t vm, uint32_t str, bool useDefault) {
		static thread_local StoreCache lastVmData;
		const char* s{ GetDataFromCache(lastVmData, vm, "function", str) };
		return !s && useDefault ? utils::va("function_%04x", str) : s;
	}
}

namespace {
	int acef_gscopaque(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 4)) return tool::BAD_USAGE;

		tool::gsc::opcode::VMId vmid{ tool::gsc::opcode::VMOf(argv[2]) };
		std::filesystem::path tsv{ argv[3] };
		std::filesystem::path output{ argv[4] };
		const char* type{ argv[5] };
		utils::compress::CompressionAlgorithm alg{ tool::NotEnoughParam(argc, 5) ? compatibility::acts_acef::DEFAULT_COMPRESS : utils::compress::GetConfigName(argv[6]) };

		if (!vmid) {
			LOG_ERROR("Can't find vm for name {}", argv[2]);
			return tool::BASIC_ERROR;
		}

		std::string buffer{};

		if (!utils::ReadFile(tsv, buffer)) {
			LOG_WARNING("Can't read hash csv {}", tsv.string());
			return tool::BASIC_ERROR;
		}

		rapidcsv::Document doc{};

		std::stringstream stream{ buffer };

		doc.Load(stream, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams('\t'));

		if (doc.GetColumnCount() < 2) {
			LOG_WARNING("Can't read string tsv {}: invalid file", tsv.string());
			return tool::BASIC_ERROR;
		}

		std::vector<byte> strmemory{};
		std::vector<uint32_t> locs{};

		strmemory.push_back(0xFF); // a string can't have the loc 0

		for (size_t i = 0; i < doc.GetRowCount(); i++) {
			const std::string id{ doc.GetCell<std::string>(0, i) };
			const std::string value{ doc.GetCell<std::string>(1, i) };

			try {
				uint32_t idx{ (uint32_t)utils::ParseFormatInt(id.c_str()) };

				if (locs.size() <= idx) {
					locs.resize((size_t)idx + 1);
				}

				// set loc
				locs[idx] = (uint32_t)utils::WriteString(strmemory, value.data());
			}
			catch (...) {}
		}

		std::vector<byte> blockData{};

		utils::WriteValue<uint32_t>(blockData, (uint32_t)locs.size()); // count
		utils::WriteValue(blockData, locs.data(), (uint32_t)(sizeof(locs[0]) * locs.size())); // locs
		utils::WriteValue(blockData, strmemory.data(), (uint32_t)(strmemory.size())); // string memory

		compatibility::acts_acef::AcefBlock block{};
		block.name = hash::Hash64(utils::va("acts.gscbin%s.%llx", type, vmid));
		block.data = blockData.data();
		block.size = blockData.size();

		compatibility::acts_acef::CompressACEFBlock(&block, 1, alg, output);

		return tool::OK;
	}
	ADD_TOOL(acef_gscopaque, "gsc", " [vm] [strings] [output] [type] (compress=zstd)", "compile gscbin strings tsv file to acef file", acef_gscopaque);
}