#include <includes.hpp>

int lookuptool(const Process& proc, int argc, const char* argv[]) {
	std::cout << std::hex;
	for (int i = 2; i < argc; i++) {
		INT64 v = strtoull(argv[i], nullptr, 0x10);
		std::cout << v << "=";
		LPCCH extract = hashutils::ExtractPtr(v);
		if (extract) {
			std::cout << extract << "\n";
		}
		else {
			std::cout << "can't be find\n";
		}
	}
	return 0;
}

int hash32(const Process& proc, int argc, const char* argv[]) {
	std::cout << std::hex;
	for (int i = 2; i < argc; i++) {
		std::cout << argv[i] << "=" << hashutils::Hash32(argv[i]) << "\n";
	}
	return 0;
}

int hash64(const Process& proc, int argc, const char* argv[]) {
	std::cout << std::hex;
	UINT64 sur;
	int i = 2;
	if (argc >= 3 && !_strcmpi("--iw", argv[2])) {
		sur = 0x47f5817a5ef961baLL;
		i++;
		std::cout << "iw\n";
	} else{
		sur = 0xcbf29ce484222325LL;
	}
	for (; i < argc; i++) {
		std::cout << argv[i] << "=" << hashutils::Hash64(argv[i], sur) << "\n";
	}
	return 0;
}

int collisiontool(const Process& proc, int argc, const char* argv[]) {
	std::cout << "Searching...\n";
	int issues = hashutils::LoadMap(hashutils::DEFAULT_HASH_FILE, false);

	if (issues) {
		std::cerr << "Found " << issues << " collision(s)\n";
		return tool::BASIC_ERROR;
	}

	std::cout << "No collision.\n";
	return tool::OK;
}

ADD_TOOL("lookup", " (string)*", "lookup strings", nullptr, lookuptool);
ADD_TOOL("h32", " (string)*", "hash strings", nullptr, hash32);
ADD_TOOL("h64", " (string)*", "hash strings", nullptr, hash64);
ADD_TOOL("str", "", "check collisions in the string file", nullptr, collisiontool);
