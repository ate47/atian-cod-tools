#include <includes.hpp>
#include "tools/gsc.hpp"


namespace {

	BYTE* DecryptStringCW(BYTE* str) {
		if ((*str & 0xC0) != 0x80u) {
			return str;
		}

		switch ((char)*str + 0x80) {
		case 16:
		case 32:
		case 48:
			return str;
		case 29:
			return str + 1;
		case 11:
			return str + 3;
		}

		return nullptr;
	}


	void DecryptStrHandle(const std::filesystem::path& path, std::ostream& out, tool::gsc::T9GSCOBJ* obj) {

		auto* str = reinterpret_cast<tool::gsc::T8GSCString*>(obj->magic + obj->string_offset);
		for (size_t i = 0; i < obj->string_count; i++) {
			BYTE* string = obj->magic + str->string;

			BYTE type = ((*string & 0xC0) != 0x80u) ? 0 : *string;
			string = DecryptStringCW(string);


			if (string) {
				if (type) {
					out << std::hex << (int)type;
				}
				else {
					out << "none";
				}
				out << " : " << string << std::endl;
			}

			// skip addresses
			str = reinterpret_cast<tool::gsc::T8GSCString*>(reinterpret_cast<UINT32*>(str + 1) + str->num_address);
		}


	}

	int cwdecrypt(Process& proc, int argc, const char* argv[]) {
		std::vector<std::filesystem::path> files{};

		utils::GetFileRecurse("scriptparsetree_cw", files);


		std::filesystem::path outDir{ "output_cw" };
		std::filesystem::create_directories(outDir);

		std::ofstream of{ outDir / "string_decrypt.txt" };

		if (!of) {
			std::cerr << "Can't open output\n";
			return tool::BASIC_ERROR;
		}

		for (const auto& file : files) {

			LPVOID buffer = NULL;
			size_t size;
			LPVOID bufferNoAlign = NULL;
			size_t sizeNoAlign;
			if (!utils::ReadFileAlign(file, bufferNoAlign, buffer, sizeNoAlign, size)) {
				std::cerr << "Can't read file data for " << file << "\n";
				continue;
			}

			if (size < sizeof(tool::gsc::T9GSCOBJ) || *reinterpret_cast<UINT64*>(buffer) != 0x38000a0d43534780) {
				std::free(bufferNoAlign);
				continue;
			}


			DecryptStrHandle(file, of, reinterpret_cast<tool::gsc::T9GSCOBJ*>(buffer));

			std::cout << "parsed " << file << "\n";
			std::free(bufferNoAlign);
		}

		of.close();
		std::cout << "Done\n";

		return tool::OK;
	}

}
#ifndef CI_BUILD

ADD_TOOL("cwdecrypt", "", "decrypt tests", nullptr, cwdecrypt);

#endif