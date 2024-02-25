#include <includes.hpp>
#include <lz4.h>
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

		if (!type || type== DT_WNI) {
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

	ADD_TOOL("wni_r", " [input] [output] [type=csv,txt]", "Read WNI file/dir", nullptr, wni_r);
}