#include <includes.hpp>
#include <utils/crc.hpp>
#include <core/bytebuffer.hpp>

namespace {
	struct ProfilesData {
		byte unk0;
		byte unk1;
		wchar_t profileName[21];
		bool profileDefault;
		char __pad[1323];
		bool dirty;
		uint32_t checksum;
	};
	static_assert(sizeof(ProfilesData) == 0x560);

	uint32_t ComputeCRC(byte* data, int len) {
		uint32_t crc; // eax
		int v2; // edx

		crc = 0xFFFFFFFF;
		if (data) {
			v2 = len;
			do {
				crc = utils::crc::CRC32TableB::crcTable[*data++ ^ HIBYTE(crc)] ^ ((crc << 8) & 0xFFFFFFFF);
				--v2;
			} while (v2);
		}
		return ~crc;
	}

	int bf2sg(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;



		std::vector<byte> buff{ utils::ReadFile<std::vector<byte>>(argv[2]) };
		core::bytebuffer::ByteBuffer reader{ buff };

		uint32_t unk0{ reader.Read<uint32_t>() };
		ProfilesData* profilesData{ reader.ReadPtr<ProfilesData>() };
		std::filesystem::path out{ argv[2] };
		out.replace_extension(".dec.dat");
		utils::WriteFile(out, profilesData, 0x55C);

		LOG_INFO("Profile name: {}", utils::WStrToStr(profilesData->profileName));
		LOG_INFO("dirty: {}", profilesData->dirty);
		LOG_INFO("unk0 ..... 0x{:x}", unk0);
		LOG_INFO("checksum . 0x{:x}", profilesData->checksum);

		LOG_INFO("comp1 . 0x{:x}", ComputeCRC((byte*)profilesData, 0x55C));


		return tool::OK;
	}


	ADD_TOOL(bf2sg, "dev", " [save]", "read save", bf2sg);

}