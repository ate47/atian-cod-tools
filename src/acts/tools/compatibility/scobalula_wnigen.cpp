#include <includes.hpp>
#include <lz4.h>
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

	int wni_r(Process& proc, int argc, const char* argv[]) {
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

	int wni_gen(Process& proc, int argc, const char* argv[]) {
		if (argc < 4) {
			return tool::BAD_USAGE;
		}

		auto* in = argv[2];
		auto* out = argv[3];
		bool canon{};
		if (argc > 4) {
			canon = !_strcmpi(argv[4], "true");
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
		bool useIW = !opt.noIWHash;
		bool useTreyarch = !opt.noTreyarchHash;

		std::string line;
		uint32_t count{};
		
		std::vector<byte> rawdata{};

		LOG_DEBUG("Loading strings...");

		while (is.good() && std::getline(is, line)) {
			if (canon) {
				utils::WriteValue(rawdata, (uint64_t)hash::Hash32(line.c_str()));
				utils::WriteString(rawdata, line.c_str());
			}
			if (useTreyarch) {
				utils::WriteValue(rawdata, hash::Hash64(line.c_str()));
				utils::WriteString(rawdata, line.c_str());
				utils::WriteValue(rawdata, hashutils::HashT7(line.c_str()));
				utils::WriteString(rawdata, line.c_str());
			}
			if (useIW) {
				utils::WriteValue(rawdata, hashutils::HashIWRes(line.c_str()));
				utils::WriteString(rawdata, line.c_str());
				utils::WriteValue(rawdata, hashutils::HashJupScr(line.c_str()));
				utils::WriteString(rawdata, line.c_str());
				utils::WriteValue(rawdata, hashutils::Hash64(line.c_str(), 0x811C9DC5, 0x1000193) & 0xFFFFFFFF);
				utils::WriteString(rawdata, line.c_str());
				utils::WriteValue(rawdata, hashutils::HashIWDVar(line.c_str()));
				utils::WriteString(rawdata, line.c_str());
				utils::WriteValue(rawdata, hashutils::HashT10Scr(line.c_str()));
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

	ADD_TOOL(wni_r, "compatibility", " [input] [output] [type=csv,txt]", "Read WNI file/dir", nullptr, wni_r);
	ADD_TOOL(wni_gen, "compatibility", " [input] [output] [canon=false]", "Gen WNI file", nullptr, wni_gen);
}