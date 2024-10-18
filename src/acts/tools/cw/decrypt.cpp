#include <includes.hpp>
#include "tools/gsc.hpp"


namespace {

	byte* DecryptStringCW(byte* str) {
		if ((*str & 0xC0) != 0x80u) {
			return str;
		}

		return str + 3;
	}


	void DecryptStrHandle(const std::filesystem::path& path, std::ostream& out, tool::gsc::T9GSCOBJ* obj) {

		auto* str = reinterpret_cast<tool::gsc::T8GSCString*>(obj->magic + obj->string_offset);
		for (size_t i = 0; i < obj->string_count; i++) {
			byte* string = obj->magic + str->string;

			byte type = ((*string & 0xC0) != 0x80u) ? 0 : *string;
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
			str = reinterpret_cast<tool::gsc::T8GSCString*>(reinterpret_cast<uint32_t*>(str + 1) + str->num_address);
		}


	}

	int cwdecrypt(Process& proc, int argc, const char* argv[]) {
		std::vector<std::filesystem::path> files{};

		utils::GetFileRecurse("scriptparsetree_cw", files);


		std::filesystem::path outDir{ "output_cw" };
		std::filesystem::create_directories(outDir);

		std::ofstream of{ outDir / "string_decrypt.txt" };

		if (!of) {
			LOG_ERROR("Can't open output");
			return tool::BASIC_ERROR;
		}

		for (const auto& file : files) {

			void* buffer = NULL;
			size_t size;
			void* bufferNoAlign = NULL;
			size_t sizeNoAlign;
			if (!utils::ReadFileAlign(file, bufferNoAlign, buffer, sizeNoAlign, size)) {
				LOG_ERROR("Can't read file data for {}", file.string());
				continue;
			}

			if (size < sizeof(tool::gsc::T9GSCOBJ) || *reinterpret_cast<uint64_t*>(buffer) != 0x38000a0d43534780) {
				std::free(bufferNoAlign);
				continue;
			}


			DecryptStrHandle(file, of, reinterpret_cast<tool::gsc::T9GSCOBJ*>(buffer));

			LOG_INFO("Parsed {}", file.string());
			std::free(bufferNoAlign);
		}

		of.close();
		LOG_INFO("Done");

		return tool::OK;
	}

}
#ifndef CI_BUILD

ADD_TOOL(cwdecrypt, "cw", "", "decrypt tests", nullptr, cwdecrypt);

#endif