#include <includes.hpp>
#include <extension/acts_extension.hpp>
#include <tools/utils/compress_utils.hpp>
#include <tools/compatibility/acts_acef.hpp>
#include <core/bytebuffer.hpp>
#include <rapidcsv.h>
#include "gsc_iw.hpp"

namespace tool::gsc::iw {
	const char* GetOpaqueStringForVm(uint64_t vm, uint32_t str) {
		static thread_local struct {
			// cache that to avoid having to load them multiple times for the same VM
			uint64_t last{};
			acts::extension::AcefArray* arr{};
			size_t count{};
		} lastVmData;

		if (lastVmData.last != vm) {
			acts::extension::GetExtensionData(utils::va("acts.gscbinopaque.%llx", vm), &lastVmData.arr, &lastVmData.count);
			lastVmData.last = vm;
		}

		for (size_t i = 0; i < lastVmData.count; i++) {
			core::bytebuffer::ByteBuffer reader{ (byte*)lastVmData.arr[i].data, lastVmData.arr[i].len };
			uint32_t count{ reader.Read<uint32_t>() };
			if (str >= count) continue; // string too high
			uint32_t loc{ reader.ReadPtr<uint32_t>(count)[str]};
			if (!loc) continue; // not defined in this ext

			reader.Skip(loc);
			return reader.ReadString();
		}

		return utils::va("ref_%04x", str);
	}
}

namespace {
	int acef_gscopaque(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 3)) return tool::BAD_USAGE;

		tool::gsc::opcode::VMId vmid{ tool::gsc::opcode::VMOf(argv[2]) };
		std::filesystem::path tsv{ argv[3] };
		std::filesystem::path output{ argv[4] };
		utils::compress::CompressionAlgorithm alg{ tool::NotEnoughParam(argc, 4) ? compatibility::acts_acef::DEFAULT_COMPRESS : utils::compress::GetConfigName(argv[5]) };

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
		block.name = hash::Hash64(utils::va("acts.gscbinopaque.%llx", vmid));
		block.data = blockData.data();
		block.size = blockData.size();

		compatibility::acts_acef::CompressACEFBlock(&block, 1, alg, output);

		return tool::OK;
	}
	ADD_TOOL(acef_gscopaque, "gsc", " [vm] [strings] [output] (compress=zstd)", "compile gscbin opaque strings tsv file to acef file", acef_gscopaque);
}