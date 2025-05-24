#include <includes.hpp>
#include <lz4.h>
#include <rapidcsv.h>
#include "actscli.hpp"
#include <deps/scobalula_wni.hpp>
#include <utils/compress_utils.hpp>
#include <tools/compatibility/scobalula_wnigen.hpp>

namespace compatibility::scobalula::wnigen {
	int CompressWNIFile(std::map<std::string, std::unordered_set<uint64_t>>& dataMap, const std::filesystem::path& out) {

		uint32_t count{};

		std::vector<byte> rawdata{};
		for (auto& [str, hashVal] : dataMap) {
			for (uint64_t hash : hashVal) {
				utils::WriteValue(rawdata, hash);
				utils::WriteString(rawdata, str.c_str());
				count++;
			}
		}

		utils::OutFileCE os{ out, true, std::ios::binary };

		utils::compress::CompressionAlgorithm alg{ utils::compress::COMP_LZ4 | utils::compress::COMP_HIGH_COMPRESSION };

		if (rawdata.size() >= LZ4_MAX_INPUT_SIZE) {
			LOG_ERROR("File too big.");
			return tool::BASIC_ERROR;
		}

		size_t bound = utils::compress::GetCompressSize(alg, rawdata.size());

		auto comp = std::make_unique<char[]>(bound + 1);
		comp[bound] = 0;

		LOG_DEBUG("Compressing...");

		if (!utils::compress::Compress(alg, comp.get(), &bound, rawdata.data(), rawdata.size())) {
			LOG_ERROR("Failed to compress, abort.");
			return tool::BASIC_ERROR;
		}

		LOG_INFO(
			"{} hash(es) compressed {}B -> {}B ({}% saved) into {}",
			count, utils::FancyNumber(rawdata.size()), utils::FancyNumber(bound),
			100 - 100 * bound / rawdata.size(),
			out.string()
		);

		// header
		utils::WriteValue<uint32_t>(os, deps::scobalula::wni::WNI_MAGIC);
		utils::WriteValue<uint16_t>(os, deps::scobalula::wni::WNI_VERSION);
		utils::WriteValue<uint32_t>(os, count); // entries
		utils::WriteValue<uint32_t>(os, (uint32_t)bound); // compressedSize
		utils::WriteValue<uint32_t>(os, (uint32_t)rawdata.size()); // decompressedSize

		// buffer
		utils::WriteValue(os, comp.get(), bound + 1);

		return tool::OK;
	}
	int CompressWNIFile(std::unordered_map<uint64_t, const char*>& dataMap, const std::filesystem::path& out) {
		std::map<std::string, std::unordered_set<uint64_t>> rev{};

		for (auto& [h, str] : dataMap) {
			rev[str].insert(h);
		}

		return CompressWNIFile(rev, out);
	}

	namespace {

		enum DumpType {
			DT_INVALID = 0,
			DT_CSV,
			DT_WNI,
			DT_TXT,
		};

		DumpType DumpTypeFromName(const char* name) {
			if (!_strcmpi(name, "csv")) {
				return DT_CSV;
			}
			if (!_strcmpi(name, "wni")) {
				return DT_WNI;
			}
			if (!_strcmpi(name, "txt")) {
				return DT_TXT;
			}

			return DT_INVALID;
		}

		int wni_r(int argc, const char* argv[]) {
			if (argc < 5) {
				return tool::BAD_USAGE;
			}

			auto* in = argv[2];
			auto* out = argv[3];
			auto type = DumpTypeFromName(argv[4]);

			if (!type || type == DT_WNI) {
				LOG_ERROR("Bad type {}", argv[4]);
				return tool::BAD_USAGE;
			}
			std::ofstream os{ out };

			if (!os) {
				LOG_ERROR("Can't open {}", out);
				return tool::BASIC_ERROR;
			}

			deps::scobalula::wni::ReadWNIFiles(in, [&os, type](uint64_t hash, const char* str) {
				if (type == DT_CSV) {
					os << std::hex << hash;
					os << ",";
				}
				os << str << "\n";
				});

			os.close();
			LOG_INFO("Dump into {}", out);

			return tool::OK;
		}

		enum Algorithms : uint64_t {
			ALG_FNV = 1,
			ALG_FNV_IW_RES = 1 << 2,
			ALG_DVAR = 1 << 3,
			ALG_FNV32 = 1 << 4,
			ALG_SCR_T7 = 1 << 5,
			ALG_SCR_T89 = 1 << 6,
			ALG_SCR_JUP = 1 << 7,
			ALG_SCR_T10 = 1 << 8,
			ALG_SCR_T10_SP = 1 << 9,
			ALG_OMNVAR_T10 = 1 << 10,

			ALG_ALL = static_cast<Algorithms>(-1),
		};

		int wni_gen_csv(int argc, const char* argv[]) {
			if (tool::NotEnoughParam(argc, 1)) {
				return tool::BAD_USAGE;
			}
			std::filesystem::path csv{ argv[2] };


			std::filesystem::path out;
			if (tool::NotEnoughParam(argc, 2)) {
				out = csv;
				out.replace_extension(".wni");
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

			return CompressWNIFile(dataMap, out);
		}


		int wni_gen(int argc, const char* argv[]) {
			if (tool::NotEnoughParam(argc, 1)) {
				return tool::BAD_USAGE;
			}

			std::filesystem::path in{ argv[2] };
			std::filesystem::path out;

			if (tool::NotEnoughParam(argc, 2)) {
				out = in;
				out.replace_extension(".wni");
			}
			else {
				out = argv[3];
			}

			uint64_t alg{};
			if (tool::NotEnoughParam(argc, 3)) {
				alg |= ALG_ALL;
			}
			else {
				for (size_t i = 4; i < argc; i++) {
					switch (hash::Hash64(argv[i])) {
					case hash::Hash64("fnv"):
						alg |= ALG_FNV;
						break;
					case hash::Hash64("fnv32"):
						alg |= ALG_FNV32;
						break;
					case hash::Hash64("iwres"):
						alg |= ALG_FNV_IW_RES;
						break;
					case hash::Hash64("t7"):
						alg |= ALG_SCR_T7;
						break;
					case hash::Hash64("t8"):
					case hash::Hash64("t9"):
					case hash::Hash64("t89"):
						alg |= ALG_SCR_T89;
						break;
					case hash::Hash64("t10"):
						alg |= ALG_SCR_T10;
						break;
					case hash::Hash64("t10sp"):
						alg |= ALG_SCR_T10;
						break;
					case hash::Hash64("jup"):
						alg |= ALG_SCR_JUP;
						break;
					case hash::Hash64("dvar"):
						alg |= ALG_DVAR;
						break;
					case hash::Hash64("omnvar"):
						alg |= ALG_OMNVAR_T10;
						break;
					case hash::Hash64("all"):
						alg |= ALG_ALL;
						break;
					default:
						LOG_ERROR("Invalid algorithm {}", argv[i]);
						return tool::BAD_USAGE;
					}

				}
			}

			std::ifstream is{ in };

			if (!is) {
				LOG_ERROR("Can't open input {}", in.string());
				return tool::BASIC_ERROR;
			}
			utils::CloseEnd isce{ is };

			utils::OutFileCE os{ out };

			if (!os) {
				LOG_ERROR("Can't open output {}", out.string());
				return tool::BASIC_ERROR;
			}

			auto& opt = actscli::options();

			std::string line;
			uint32_t count{};

			std::map<std::string, std::unordered_set<uint64_t>> dataMap{};

			LOG_DEBUG("Loading strings...");

			while (is.good() && std::getline(is, line)) {
				std::unordered_set<uint64_t>& i{ dataMap[line] };
				const char* cstr{ line.c_str() };
				if (alg & ALG_SCR_T89) i.insert(hash::HashT89Scr(cstr));
				if (alg & ALG_SCR_T7) i.insert(hash::HashT7(cstr));
				if (alg & ALG_FNV32) i.insert(hash::HashX32(cstr));
				if (alg & ALG_FNV) i.insert(hash::Hash64(cstr));
				if (alg & ALG_FNV_IW_RES) i.insert(hash::HashIWAsset(cstr));
				if (alg & ALG_SCR_JUP) i.insert(hash::HashJupScr(cstr));
				if (alg & ALG_DVAR) i.insert(hash::HashIWDVar(cstr));
				if (alg & ALG_SCR_T10) i.insert(hash::HashT10Scr(cstr));
				if (alg & ALG_SCR_T10_SP) i.insert(hash::HashT10ScrSP(cstr));
				if (alg & ALG_OMNVAR_T10) i.insert(hash::HashT10OmnVar(cstr));
				count++;
			}
			LOG_DEBUG("strings loaded: {}", count);

			return CompressWNIFile(dataMap, out);
		}

		ADD_TOOL(wni_r, "compatibility", " [input] [output] [type=csv,txt]", "Read WNI file/dir", wni_r);
		ADD_TOOL(wni_gen_csv, "compatibility", " [input] (output=input.wni)", "Gen WNI file from csv", wni_gen_csv);
		ADD_TOOL(wni_gen, "compatibility", " [input] (output=input.wni) (algorithms=all)+", "Gen WNI file with algo", wni_gen);
	}
}