#include <includes.hpp>

static int resolver(Process& unused, int argc, const char* argv[]) {
	if (argc == 2) {
		std::cerr << "Missing config file\n";
		return tool::BAD_USAGE;
	}

	std::filesystem::path f(argv[2]);

	LPVOID buff = NULL;
	size_t size = 0;

	if (!utils::ReadFileNotAlign(f, buff, size, false)) {
		std::cerr << "Can't read file '" << f.string() << "'\n";
		return tool::BASIC_ERROR;
	}

	BYTE unzippedData[5000];

	uLongf sizef = (uLongf)size;
	uLongf sizef2;


	if (uncompress2(unzippedData, &sizef, reinterpret_cast<const Bytef*>(buff), &sizef2) < 0) {
		std::cout << "Too big'\n";
		std::free(buff);
		return tool::BASIC_ERROR;
	}


	auto output = f.parent_path() / (f.filename().string() + "e");
	utils::WriteFile(output, &unzippedData[0], sizef2);

	std::cout << "Extract into '" << output.string() << "'\n";

	std::free(buff);

	return tool::OK;
}

#ifdef DEBUG
ADD_TOOL("zlib", " [file]", "zlib extract", nullptr, resolver);
#endif