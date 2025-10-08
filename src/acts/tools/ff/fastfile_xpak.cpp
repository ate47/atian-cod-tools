#include <includes.hpp>
#include <core/bytebuffer_file.hpp>
#include <tools/utils/data_utils.hpp>

namespace {
	constexpr uint32_t XPAK_MAGIC = 0x4950414B;
	constexpr uint16_t XPAK_VERSION = 0x17;


	struct XPakHeader {
		uint32_t magic;
		uint16_t unk4;
		uint16_t version;
		uint64_t unk8;
		uint32_t unk10;
		uint32_t unk14;
		uint64_t fileSize;
		uint64_t name;
	};

	int ffxpakr(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) {
			return tool::BAD_USAGE;
		}

		std::vector<std::filesystem::path> paths{};
		utils::GetFileRecurseExt(argv[2], paths, ".xsub\0.xpak\0");

		std::filesystem::path outDir{ argv[3] };
		std::filesystem::create_directories(outDir);

		for (const std::filesystem::path& p : paths) {
			LOG_INFO("Read {}", p.string());
			utils::InFileCE is{ p, true, std::ios::binary };
			core::bytebuffer::FileReader reader{ is };

			XPakHeader header{};

			reader.Read(&header, sizeof(header));

			if (header.magic != XPAK_MAGIC) {
				LOG_ERROR("Bad magic");
				continue;
			}
			if (header.version != XPAK_VERSION) {
				LOG_ERROR("Bad version");
				continue;
			}

			std::filesystem::path outFile{ outDir / p.filename() };
			outFile.replace_extension(".txt");
			utils::OutFileCE os{ outFile, true };
			os << "file " << p << "\n";
			os << "size: 0x" << std::hex << header.fileSize << "\n";
			os << "id: " << hashutils::ExtractTmp("hash", header.name) << "\n";

			// 38440000
			// 3843f874 // end data
			// 3839DBC2 a243e
			// 7939
			// 383A8000
		}



		return tool::OK;
	}

	ADD_TOOL(ffxpakr, "fastfile", " [file]", "read xpak file", ffxpakr);
}