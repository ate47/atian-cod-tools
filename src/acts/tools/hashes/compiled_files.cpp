#include <includes.hpp>
#include "compiled_files.hpp"
#include <regex>

namespace {
	using namespace tool::hashes::compiled_files;

	int cfd(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}
		const char* input{ argv[2] };

		const char* output{ argc < 4 ? "output_raw" : argv[3] };

		std::vector<std::filesystem::path> files{};

		utils::GetFileRecurse(input, files, [](const std::filesystem::path& p) {
			auto s = p.string();
			return s.ends_with(".cf");
		});

		hashutils::ReadDefaultFile();

		std::string buff{};
		std::filesystem::path outputDir{ output };
		for (const std::filesystem::path& file : files) {
			if (!utils::ReadFile(file, buff)) {
				LOG_ERROR("Can't read file {}", file.string());
				continue;
			}

			const CompiledFileHeader& header = *reinterpret_cast<CompiledFileHeader*>(buff.data());

			if (header.magic != MAGIC) {
				LOG_ERROR("Can't read file {}: invalid magic", file.string());
				continue;
			}

			const char* n = hashutils::ExtractPtr(header.name);

			std::filesystem::path out;
			if (n) {
				// script bundle thing
				n = utils::MapString(utils::CloneString(n), [](char c) -> char { return c == ':' ? '/' : c; });
				if (header.isSpecial) {
					out = outputDir / header.type / n;
				} else{
					out = outputDir / n;
				}
			}
			else {
				char* fileName;
				if (*header.preferedExtension) {
					fileName = utils::va("file_%llx.%s", header.name, header.preferedExtension);
				}
				else {
					fileName = utils::va("file_%llx", header.name);
				}
				if (header.isSpecial) {
					out = outputDir / header.type / fileName;
				}
				else {
					if (*header.type) {
						out = outputDir / "hashed" / header.type / fileName;
					}
					else {
						out = outputDir / fileName;
					}
				}
			}

			std::filesystem::create_directories(out.parent_path());


			if (header.isString) {
				std::string strBuff{ buff.data() + sizeof(header), buff.size() - sizeof(header) };

				static std::regex pattern{ "hash_([0-9a-fA-F]{1,16})" };

				size_t idx{};
				while (idx < strBuff.size()) {
					auto rbegin = std::sregex_iterator(strBuff.begin() + idx, strBuff.end(), pattern);

					if (rbegin == std::sregex_iterator()) {
						break; // nothing else
					}

					std::smatch match = *rbegin;
					size_t mstart = match.position() + idx;
					size_t mlen = match.length();

					try {
						uint64_t hash = std::stoull(match[1].str(), nullptr, 16);
						const char* ptr = hashutils::ExtractPtr(hash);
						if (ptr) {
							std::string before = strBuff.substr(0, mstart);
							std::string after = strBuff.substr(mstart + mlen, strBuff.size() - mstart - mlen);

							strBuff = before + ptr + after;
							mlen = strlen(ptr);
							LOG_TRACE("{} REPLACED {:x} -> {}", out.string(), hash, ptr);
						}
					}
					catch (std::runtime_error& e) {
						LOG_WARNING("Ignored bad hash: {}", e.what());
					}
					//LOG_TRACE("{} old {} -> {}", idx, mstart + mlen, out.string());
					idx = mstart + mlen;
				}


				if (!utils::WriteFile(out, strBuff.data(), strBuff.size())) {
					LOG_ERROR("Can't write file {}", out.string());
				}
				else {
					LOG_INFO("Write {} -> {}", file.string(), out.string());
				}
			}
			else {
				if (!utils::WriteFile(out, buff.data() + sizeof(header), buff.size() - sizeof(header))) {
					LOG_ERROR("Can't write file {}", out.string());
				}
				else {
					LOG_INFO("Write {} -> {}", file.string(), out.string());
				}
			}

		}
		return tool::OK;
	}

	ADD_TOOL("cfd", "hash", " [dir] (output)", "decompile raw compiled file (.cf)", nullptr, cfd);

}