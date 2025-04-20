#include <includes.hpp>
#include <cli/cli_options.hpp>
#include <extension/acts_extension.hpp>
#include <tools/utils/compress_utils.hpp>
#include <core/memory_allocator.hpp>
#include <tools/compatibility/acts_acef.hpp>
#include <rapidcsv.h>
#include <tools/gsc.hpp>
#include <regex>

namespace compatibility::acts_acef {
	using namespace acts::extension;


	struct AcefOption {
		bool help{};
		const char* algName{};
		utils::compress::CompressionAlgorithm alg{ compatibility::acts_acef::DEFAULT_COMPRESS };

	};

	AcefBlock CreateACEFBlockHash(std::map<std::string, std::unordered_set<uint64_t>>& dataMap, core::memory_allocator::MemoryAllocator& alloc) {
		size_t blocksize{ 8 };
		size_t numHashes{};
		for (auto& [k, v] : dataMap) {
			blocksize += (k.size() + 1 + 8) * v.size();
			numHashes += v.size();
		}

		byte* block{ alloc.Alloc<byte>(blocksize) };

		AcefBlock acefblock{};
		acefblock.data = block;
		acefblock.size = blocksize;
		acefblock.name = hash::Hash64("acts.hash");
				
		byte* w{ block };

		*(uint64_t*)w = (uint64_t)numHashes;
		w += 8;
		
		for (auto& [v, hashes] : dataMap) {
			for (uint64_t k : hashes) {
				*(uint64_t*)w = k;
				w += 8;
				size_t len{ v.size() + 1 };
				std::memcpy(w, v.data(), len);
				w += len;
			}
		}

		return acefblock;
	}

	
	void CreateACEFOpCodeBlock(int plts, const tool::gsc::opcode::VmInfo* info, core::memory_allocator::MemoryAllocator& alloc, AcefBlock* blocks, size_t* count) {
		*count = 0;
		for (auto& [plt, opcode] : info->opcodemappltlookup) {
			if (plts && !(plts & (1 << plt))) continue; // not used

			size_t numopcodes{};

			for (auto& [k, v] : opcode) numopcodes += v.size();

			AcefBlock& vmBlock{ blocks[(*count)++] };
			vmBlock.name = hash::Hash64("acts.opcodes");
			vmBlock.size = sizeof(uint64_t) * 2 + sizeof(uint16_t) * 2 * numopcodes;
			vmBlock.data = alloc.Alloc(vmBlock.size);


			uint64_t* s{ (uint64_t*)vmBlock.data };
			s[0] = info->vmMagic;
			s[1] = plt;
			uint16_t* opcodes{ (uint16_t*)(s + 2) };
					
			for (auto& [k, v] : opcode) {
				for (uint16_t op : v) {
					opcodes[0] = k;
					opcodes[1] = op;
					opcodes += 2;
				}
			}

			LOG_INFO("Add VM {}/{} -> {} 0x{:x} opcode(s)", info->name, tool::gsc::opcode::PlatformName(plt), tool::gsc::opcode::VMIdFancyName(info->vmMagic), numopcodes);
		}
	}

	void CompressACEFBlock(AcefBlock* blocks, size_t blocksCount, utils::compress::CompressionAlgorithm alg, const std::filesystem::path& outFile) {
		AcefHeader header{};
		header.magic = ACEF_MAGIC;
		header.version = ACEF_VERSION_LATEST;
		header.flags = 0;
		if (std::endian::native == std::endian::big) header.flags |= ACEF_BIGENDIAN;
		header.headerSize = (uint32_t)sizeof(AcefHeader);
		header.sizeObjectHeader = (uint32_t)sizeof(AcefBlockHeader);
		header.numObjects = (uint32_t)blocksCount;
		header.compress = (uint64_t)alg;
		
		header.compressedSize = 0;
		header.decompressedSize = 0;
		header.windowSize = 0;
		
		std::vector<byte> tmpCompressed{};
		std::vector<byte> data{};

		utils::Allocate<decltype(header)>(data);

		LOG_INFO("Building {} with {} block(s)", outFile.string(), blocksCount);

		for (size_t i = 0; i < blocksCount; i++) {
			AcefBlock& block{ blocks[i] };
			size_t sizeCompressed{ utils::compress::GetCompressSize(alg, block.size) };

			tmpCompressed.resize(sizeCompressed);

			LOG_TRACE("compressing {}B using {}", utils::FancyNumber(block.size), alg);
			utils::Timestamp pre{ utils::GetTimestamp() };
			if (!utils::compress::Compress(alg, tmpCompressed.data(), &sizeCompressed, block.data, block.size)) {
				throw std::runtime_error("Can't compress data block");
			}
			size_t diff{ (size_t)(utils::GetTimestamp() - pre) };
			LOG_TRACE("done in {}ms", diff);

			AcefBlockHeader blockHeader;
			blockHeader.blockSize = (uint32_t)sizeCompressed;
			blockHeader.decompSize = (uint32_t)block.size;
			blockHeader.checkLoc = (uint32_t)data.size();
			blockHeader.dataOffset = header.decompressedSize;
			blockHeader.blockId = block.name;


			size_t oldLen{ data.size() };
			utils::WriteValue(data, &blockHeader, sizeof(blockHeader));
			utils::WriteValue(data, tmpCompressed.data(), sizeCompressed);

			LOG_DEBUG("add new block at h=0x{:x}:len=0x{:x} -> tlen=0x{:x}", blockHeader.checkLoc, data.size() - oldLen, sizeCompressed + sizeof(blockHeader));

			header.decompressedSize += (uint32_t)block.size;
			header.compressedSize += (uint32_t)sizeCompressed;
			if (header.windowSize < sizeCompressed) {
				header.windowSize = (uint32_t)sizeCompressed;
			}
		}

		std::memcpy(data.data(), &header, sizeof(header));

		if (!utils::WriteFile(outFile, data)) {
			throw std::runtime_error(std::format("Can't write file {}", outFile.string()));
		}

		LOG_INFO("done in {} {}B -> {}B ({}% saved)", outFile.string(), 
				 utils::FancyNumber(header.decompressedSize), utils::FancyNumber(header.compressedSize),
				 100 - (int64_t)(100 * header.compressedSize / header.decompressedSize));
	}

	int acts_build_acef(int argc, const char* argv[]) {
		AcefOption opt{};
		cli::options::CliOptions opts{};
		opts
			.addOption(&opt.help, "show help", "--help", "", "-h")
			.addOption(&opt.algName, "compression algorithm name", "--comp", " [comp]", "-c")
			;

		if (!opts.ComputeOptions(2, argc, argv) || opt.help) {
			opts.PrintOptions();
			return tool::OK;
		}

		if (opts.NotEnoughParam(2)) {
			return tool::BAD_USAGE;
		}

		std::filesystem::path outFile{ opts[0] };

		if (opt.algName) {
			opt.alg = utils::compress::GetConfigName(opt.algName);
		}


		core::memory_allocator::MemoryAllocator alloc{};
		std::vector<AcefBlock> blocks{};

		for (size_t i = 1; i < opts.ParamsCount(); i++) {
			std::string_view cfg{ opts[i] };

			size_t idxcut{ cfg.find_first_of(':') };
			if (idxcut == std::string::npos) idxcut = cfg.length();


			std::string_view type{ cfg.begin(), cfg.begin() + idxcut };

			if (type == "hash") {
				hashutils::ReadDefaultFile();

				std::map<std::string, std::unordered_set<uint64_t>> dataMap{};

				for (auto& [k, s] : hashutils::GetMap()) {
					dataMap[s].insert(k);
				}

				blocks.push_back(CreateACEFBlockHash(dataMap, alloc));
			} else if (type == "op") {
				if (idxcut + 1 >= cfg.length()) {
					LOG_WARNING("Missing vm for {}", cfg);
					return tool::BASIC_ERROR;
				}
				size_t idxcut2{ cfg.find_first_of(':', idxcut + 1) };
				if (idxcut2 == std::string::npos) idxcut2 = cfg.length();
				int plts{};
				size_t idxcutplt{ idxcut2 };
				while (idxcutplt + 1 < cfg.length()) {
					size_t idxcutplt2{ cfg.find_first_of(':', idxcutplt + 1) };
					if (idxcutplt2 == std::string::npos) idxcutplt2 = cfg.length();
					
					std::string pltid{ cfg.begin() + idxcutplt + 1, cfg.begin() + idxcutplt2 };
					idxcutplt = idxcutplt2;

					tool::gsc::opcode::Platform p{ tool::gsc::opcode::PlatformOf(pltid.c_str()) };

					if (!p) {
						LOG_WARNING("Invalid platform name: {}", pltid);
						return tool::BASIC_ERROR;
					}

					plts |= 1 << p;
				}
				
				std::string vmid{ cfg.begin() + idxcut + 1, cfg.begin() + idxcut2 };
				
				bool allVms = vmid == "all";
				std::regex pattern{ vmid };

				size_t blockCount{};
				AcefBlock blocksVm[tool::gsc::opcode::Platform::PLATFORM_COUNT];
				for (auto& [k, nfo] : tool::gsc::opcode::GetVMMaps()) {
					if (!allVms) {
						if (!std::regex_match(nfo.codeName, pattern) && !std::regex_match(nfo.internalName, pattern)) {
							continue;
						}
					}
					CreateACEFOpCodeBlock(plts, &nfo, alloc, blocksVm, &blockCount);
					blocks.insert(blocks.end(), blocksVm, &blocksVm[blockCount]);
				}
			} else {
				LOG_WARNING("Unknown acef block type: {}", type);
			}
		}

		CompressACEFBlock(blocks, opt.alg, outFile);

		return tool::OK;
	}

	
	int acts_acef_hash_csv(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) {
			return tool::BAD_USAGE;
		}
		std::filesystem::path csv{ argv[2] };


		std::filesystem::path out;
		if (tool::NotEnoughParam(argc, 2) || !*argv[3]) {
			out = csv;
			out.replace_extension(".acef");
		}
		else {
			out = argv[3];
		}

		utils::compress::CompressionAlgorithm alg;
		if (tool::NotEnoughParam(argc, 3)) {
			alg = compatibility::acts_acef::DEFAULT_COMPRESS;
		} else {
			alg = utils::compress::GetConfigName(argv[4]);
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
		
		core::memory_allocator::MemoryAllocator alloc{};
		AcefBlock hashblock{ CreateACEFBlockHash(dataMap, alloc) };

		CompressACEFBlock(&hashblock, 1, alg, out);

		return tool::OK;
	}
	
	ADD_TOOL(acts_build_acef, "acts", " [out] [cfg]", "Build acef file", acts_build_acef);
	ADD_TOOL(acts_acef_hash_csv, "acts", " [input] (output=input.acef)", "Gen ACEF file from csv", acts_acef_hash_csv);
}