#include <includes.hpp>
#include "actscli.hpp"

namespace {
	int lookuptool(Process& proc, int argc, const char* argv[]) {
		hashutils::ReadDefaultFile();
		for (int i = 2; i < argc; i++) {
			INT64 v = strtoull(argv[i], nullptr, 0x10);
			LPCCH extract = hashutils::ExtractPtr(v);
			if (extract) {
				LOG_INFO("{:x}={}", v, extract);
			}
			else {
				LOG_INFO("{:x}=can't be find", v);
			}
		}
		return 0;
	}

	int hash32(Process& proc, int argc, const char* argv[]) {
		for (int i = 2; i < argc; i++) {
			LOG_INFO("{}={:x}", argv[i], hashutils::Hash32(argv[i]));
		}
		return 0;
	}

	int hash64(Process& proc, int argc, const char* argv[]) {
		UINT64 sur;
		int i = 2;
		if (argc >= 3 && !_strcmpi("--iw", argv[2])) {
			sur = 0x47F5817A5EF961BA;
			i++;
			LOG_INFO("(iw)");
		}
		else {
			sur = 0xcbf29ce484222325LL;
		}
		for (; i < argc; i++) {
			LOG_INFO("{}={:x}", argv[i], hashutils::Hash64(argv[i], sur));
		}
		return 0;
	}

	int collisiontool(Process& proc, int argc, const char* argv[]) {
		LOG_INFO("Searching...");
		const char* file = actscli::options().defaultHashFile;

		if (!file) {
			file = hashutils::DEFAULT_HASH_FILE;
		}
		int issues = hashutils::LoadMap(file, false);

		if (issues) {
			LOG_ERROR("Found {} collision(s)", issues);
			return tool::BASIC_ERROR;
		}

		LOG_INFO("No collision.");
		return tool::OK;
	}
}

ADD_TOOL("lookup", " (string)*", "lookup strings", nullptr, lookuptool);
ADD_TOOL("h32", " (string)*", "hash strings", nullptr, hash32);
ADD_TOOL("h64", " (string)*", "hash strings", nullptr, hash64);
ADD_TOOL("str", "", "check collisions in the string file", nullptr, collisiontool);
