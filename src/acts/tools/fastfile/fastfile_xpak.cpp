#include <includes.hpp>
#include <core/bytebuffer_file.hpp>
#include <tools/utils/data_utils.hpp>

namespace {
	constexpr uint32_t XPAK_MAGIC = 0x4950414B;

	enum XPakVersion : uint16_t {
		XPAK_BO4 = 0x0b,
		XPAK_BO6 = 0x17,
	};

	struct XPakSectionBO4 {
		int64_t itemCount;
		int64_t offset;
		int64_t size;
	};
	struct XPakIndexEntryBO4 {
		uint64_t key;
		int64_t offset;
		int64_t size;
	};
	static_assert(sizeof(XPakIndexEntryBO4) == 0x18);


	struct XPakHeader {
		uint32_t magic;
		uint16_t unk4;
		XPakVersion version;
	};

	struct XPakHeaderBO4 {
		uint32_t magic;
		uint16_t unk4;
		XPakVersion version;
		uint32_t revision;
		uint32_t reserved0;
		int64_t size;
		XPakSectionBO4 dataSection;
		XPakSectionBO4 indexSection;
		XPakSectionBO4 referenceSection;
		XPakSectionBO4 metaSection;
		char comment[0x188];
	};
	static_assert(sizeof(XPakHeaderBO4) == 0x200);

	struct XPakHeaderBO6 {
		uint32_t magic;
		uint16_t unk4;
		XPakVersion version;
		uint64_t unk8;
		uint32_t unk10;
		uint32_t unk14;
		uint64_t fileSize;
		uint64_t name;
	};

	void HandleHeader(std::ostream& os, core::bytebuffer::FileReader& reader, XPakHeaderBO4& header) {
		auto PrintSection = [&os](XPakSectionBO4& sec, const char* name) {
			os << "section " << name << ": 0x" << std::hex << sec.offset << "+0x" << sec.size << "=0x" << (sec.offset + sec.size) << " (count:0x" << sec.itemCount << ")\n";
			};
		os << "size: 0x" << std::hex << header.size << "\n";

		PrintSection(header.dataSection, "dataSection");
		PrintSection(header.indexSection, "indexSection");
		PrintSection(header.referenceSection, "referenceSection");
		PrintSection(header.metaSection, "metaSection");

		if (header.referenceSection.size) {
			os << "referenceSection:\n";
			if ((header.referenceSection.size / header.referenceSection.itemCount) == 8) {
				reader.Goto(header.referenceSection.offset);
				for (size_t i = 0; i < header.referenceSection.itemCount; i++) {
					uint64_t hash{ reader.Read<uint64_t>() };
					os << "#" << std::setw(21) << std::setfill(' ') << hashutils::ExtractTmp("hash", hash);
					if (i % 8 == 7) {
						os << "\n";
					}
					else {
						os << " ";
					}
				}
			}
			else {
				os << "invalid size\n";
			}
		}
		if (header.indexSection.size) {
			os << "indexSection:\n";
			if ((header.indexSection.size / header.indexSection.itemCount) == sizeof(XPakIndexEntryBO4)) {
				reader.Goto(header.indexSection.offset);
				XPakIndexEntryBO4 entry;
				for (size_t i = 0; i < header.indexSection.itemCount; i++) {
					reader.Read(&entry, sizeof(entry));
					os << "#" << hashutils::ExtractTmp("hash", entry.key) << " 0x" << std::hex << entry.offset << "+0x" << entry.size << "=0x" << (entry.offset + entry.size) << "\n";
				}
			}
			else {
				os << "invalid size\n";
			}
		}
		
	}

	bool PrintInfo(std::ostream& os, XPakVersion v, core::bytebuffer::FileReader& reader) {
		switch (v) {
		case XPakVersion::XPAK_BO4: {
			XPakHeaderBO4 header;
			reader.Read(&header, sizeof(header));
			HandleHeader(os, reader, header);
			break;
		}
		case XPakVersion::XPAK_BO6: {
			XPakHeaderBO6 header;
			reader.Read(&header, sizeof(header));
			os << "size: 0x" << std::hex << header.fileSize << "\n";
			os << "id: " << hashutils::ExtractTmp("hash", header.name) << "\n";
			break;
		}
		default:
			os << "unknown header";
			return false;
		}
		return true;
	}



	int ffxpakr(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) {
			return tool::BAD_USAGE;
		}

		std::vector<std::filesystem::path> paths{};
		utils::GetFileRecurseExt(argv[2], paths, ".xsub\0.xpak\0");

		std::filesystem::path outDir{ argv[3] };
		std::filesystem::create_directories(outDir);

		for (const std::filesystem::path& p : paths) {
			utils::InFileCE is{ p, true, std::ios::binary };
			core::bytebuffer::FileReader reader{ is };

			XPakHeader header{};

			reader.Read(&header, sizeof(header));
			reader.Goto(0);

			std::filesystem::path outFile{ outDir / p.filename() };
			outFile.replace_extension(".txt");
			LOG_INFO("Read {} to {}", p.string(), outFile.string());

			if (header.magic != XPAK_MAGIC) {
				LOG_ERROR("Bad magic");
				continue;
			}

			utils::OutFileCE os{ outFile, true };

			if (!PrintInfo(os, header.version, reader)) {
				LOG_ERROR("Bad header");
				continue;
			}

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