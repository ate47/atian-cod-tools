#include <includes.hpp>

int tool::hash::lookuptool(const Process& proc, int argc, const char* argv[]) {
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

int tool::hash::hash32(const Process& proc, int argc, const char* argv[]) {
	std::cout << std::hex;
	for (int i = 2; i < argc; i++) {
		std::cout << argv[i] << "=" << hashutils::Hash32(argv[i]) << "\n";
	}
	return 0;
}

int tool::hash::hash64(const Process& proc, int argc, const char* argv[]) {
	std::cout << std::hex;
	for (int i = 2; i < argc; i++) {
		std::cout << argv[i] << "=" << hashutils::Hash64(argv[i]) << "\n";
	}
	return 0;
}

int tool::hash::collisiontool(const Process& proc, int argc, const char* argv[]) {
	std::cout << "Searching...\n";
	int issues = hashutils::LoadMap(hashutils::DEFAULT_HASH_FILE, false);

	if (issues) {
		std::cerr << "Found " << issues << " collision(s)\n";
		return tool::BASIC_ERROR;
	}

	std::cout << "No collision.\n";
	return OK;
}

