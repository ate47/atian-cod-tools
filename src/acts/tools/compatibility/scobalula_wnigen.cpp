#include <includes.hpp>
#include <lz4.h>
#include <rapidcsv.h>
#include "actscli.hpp"
#include "compatibility/scobalula_wni.hpp"

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

		compatibility::scobalula::wni::ReadWNIFiles(in, [&os, type](uint64_t hash, const char* str) {
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

		ALG_ALL = static_cast<Algorithms>(-1),
	};

	int wni_gen_csv(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) {
			return tool::BAD_USAGE;
		}
		std::filesystem::path csv{ argv[2] };
		std::filesystem::path out{ argv[3] };
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

		uint32_t count{};

		std::vector<byte> rawdata{};
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

			utils::WriteValue(rawdata, hashVal);
			utils::WriteString(rawdata, value.c_str());
			count++;
		}


		std::ofstream os{ out, std::ios::binary };
		utils::CloseEnd osce{ os };

		if (!os) {
			LOG_ERROR("Can't open output {}", out.string());
			return tool::BASIC_ERROR;
		}

		if (rawdata.size() >= LZ4_MAX_INPUT_SIZE) {
			LOG_ERROR("File too big.");
			return tool::BASIC_ERROR;
		}

		int bound = LZ4_compressBound((int)rawdata.size());

		auto comp = std::make_unique<char[]>(bound);

		LOG_DEBUG("Compressing...");

		int compressedSize = LZ4_compress_default((const char*)&rawdata[0], &comp[0], (int)rawdata.size(), bound);
		if (compressedSize <= 0) {
			LOG_ERROR("Failed to compress, abort.");
			return tool::BASIC_ERROR;
		}

		// magic
		auto magic = compatibility::scobalula::wni::WNI_MAGIC;
		os.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
		// version
		auto version = compatibility::scobalula::wni::WNI_VERSION;
		os.write(reinterpret_cast<const char*>(&version), sizeof(version));

		uint32_t tmp;

		// entries
		tmp = count;
		LOG_INFO("Entries: {}", tmp);
		os.write(reinterpret_cast<const char*>(&tmp), sizeof(tmp));
		// compressedSize
		tmp = compressedSize;
		LOG_INFO("Compressed Size: {}", tmp);
		os.write(reinterpret_cast<const char*>(&tmp), sizeof(tmp));
		// decompressedSize
		tmp = (uint32_t)rawdata.size();
		LOG_INFO("Decompressed Size: {}", tmp);
		os.write(reinterpret_cast<const char*>(&tmp), sizeof(tmp));
		// buffer
		os.write(reinterpret_cast<const char*>(&comp[0]), compressedSize + 1);

		LOG_INFO("Created into {}", out.string());

		return tool::OK;
	}


	int wni_gen(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) {
			return tool::BAD_USAGE;
		}

		const char* in = argv[2];
		const char* out = argv[3];
		uint64_t alg{};
		if (4 == argc) {
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
					alg |= ALG_SCR_T7; 
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
			LOG_ERROR("Can't open input {}", in);
			return tool::BASIC_ERROR;
		}

		std::ofstream os{ out, std::ios::binary };

		if (!os) {
			LOG_ERROR("Can't open output {}", out);
			is.close();
			return tool::BASIC_ERROR;
		}
		auto& opt = actscli::options();

		std::string line;
		uint32_t count{};

		std::vector<byte> rawdata{};

		LOG_DEBUG("Loading strings...");

		while (is.good() && std::getline(is, line)) {
			if (alg & ALG_SCR_T89) {
				utils::WriteValue(rawdata, (uint64_t)hash::HashT89Scr(line.c_str()));
				utils::WriteString(rawdata, line.c_str());
			}

			if (alg & ALG_FNV) {
				utils::WriteValue(rawdata, hash::Hash64(line.c_str()));
				utils::WriteString(rawdata, line.c_str());
			}

			if (alg & ALG_SCR_T7) {
				utils::WriteValue(rawdata, hash::HashT7(line.c_str()));
				utils::WriteString(rawdata, line.c_str());
			}

			if (alg & ALG_FNV_IW_RES) {
				utils::WriteValue(rawdata, hash::HashIWRes(line.c_str()));
				utils::WriteString(rawdata, line.c_str());
			}

			if (alg & ALG_SCR_JUP) {
				utils::WriteValue(rawdata, hash::HashJupScr(line.c_str()));
				utils::WriteString(rawdata, line.c_str());
			}

			if (alg & ALG_FNV32) {
				utils::WriteValue(rawdata, hash::Hash64(line.c_str(), 0x811C9DC5, 0x1000193) & 0xFFFFFFFF);
				utils::WriteString(rawdata, line.c_str());
			}

			if (alg & ALG_DVAR) {
				utils::WriteValue(rawdata, hash::HashIWDVar(line.c_str()));
				utils::WriteString(rawdata, line.c_str());
			}

			if (alg & ALG_SCR_T10) {
				utils::WriteValue(rawdata, hash::HashT10Scr(line.c_str()));
				utils::WriteString(rawdata, line.c_str());
			}

			if (alg & ALG_SCR_T10_SP) {
				utils::WriteValue(rawdata, hash::HashT10ScrSP(line.c_str()));
				utils::WriteString(rawdata, line.c_str());
			}
			count++;
		}
		LOG_DEBUG("strings loaded: {}", count);

		if (rawdata.size() >= LZ4_MAX_INPUT_SIZE) {
			LOG_ERROR("File too big.");
			is.close();
			os.close();
			return tool::BASIC_ERROR;
		}

		int bound = LZ4_compressBound((int)rawdata.size());

		auto comp = std::make_unique<char[]>(bound);

		LOG_DEBUG("Compressing...");

		int compressedSize = LZ4_compress_default((const char*)&rawdata[0], &comp[0], (int)rawdata.size(), bound);
		if (compressedSize <= 0) {
			LOG_ERROR("Failed to compress, abort.");
			is.close();
			os.close();
			return tool::BASIC_ERROR;
		}

		// magic
		auto magic = compatibility::scobalula::wni::WNI_MAGIC;
		os.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
		// version
		auto version = compatibility::scobalula::wni::WNI_VERSION;
		os.write(reinterpret_cast<const char*>(&version), sizeof(version));

		uint32_t tmp;

		// entries
		tmp = count;
		LOG_INFO("Entries: {}", tmp);
		os.write(reinterpret_cast<const char*>(&tmp), sizeof(tmp));
		// compressedSize
		tmp = compressedSize;
		LOG_INFO("Compressed Size: {}", tmp);
		os.write(reinterpret_cast<const char*>(&tmp), sizeof(tmp));
		// decompressedSize
		tmp = (uint32_t)rawdata.size();
		LOG_INFO("Decompressed Size: {}", tmp);
		os.write(reinterpret_cast<const char*>(&tmp), sizeof(tmp));
		// buffer
		os.write(reinterpret_cast<const char*>(&comp[0]), compressedSize + 1);


		is.close();
		os.close();
		LOG_INFO("Created into {}", out);

		return tool::OK;
	}

	ADD_TOOL(wni_r, "compatibility", " [input] [output] [type=csv,txt]", "Read WNI file/dir", wni_r);
	ADD_TOOL(wni_gen_csv, "compatibility", " [input] [output]", "Gen WNI file from csv", wni_gen_csv);
	ADD_TOOL(wni_gen, "compatibility", " [input] [output] [algorithms=all]+", "Gen WNI file with algo", wni_gen);
}