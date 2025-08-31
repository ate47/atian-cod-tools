#include <includes.hpp>
#include <utils/compress_utils.hpp>
#include <core/bytebuffer.hpp>
#include <deps/oodle.hpp>

namespace {
	enum DTL_CompressType : uint32_t {
		DTL_CT_OODLE = 0x444F4F,
	};

	int dtl_resd(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) {
			return tool::BAD_USAGE;
		}

		std::filesystem::path in{ argv[2] };
		std::filesystem::path out{ argv[3] };

		std::vector<byte> buff{ utils::ReadFile<std::vector<byte>>(in) };
		core::bytebuffer::ByteBuffer reader{ buff };

		if (reader.Read<uint32_t>() != 0x52455304) {
			LOG_ERROR("Bad magic");
			return tool::BASIC_ERROR;
		}

		deps::oodle::Oodle& oodle{ deps::oodle::GetInstance() };

		uint32_t compression{ reader.Read<uint32_t>() };
		uint32_t decLen{ reader.Read<uint32_t>() };

		std::unique_ptr<byte[]> data{ std::make_unique<byte[]>(decLen) };

		if (!oodle.Decompress(reader.Ptr(), reader.Remaining(), data.get(), decLen, deps::oodle::OODLE_FS_NO, deps::oodle::OODLE_CRC_YES, deps::oodle::OODLE_VERB_LOTS)) {
			LOG_ERROR("Bad decomp");
			return tool::BASIC_ERROR;
		}

		//std::vector<byte> uc{ oodle.Decompress(reader.Ptr(), reader.Remaining()) };
		//LOG_INFO("end size: 0x{:x}", uc.size());



		return tool::OK;
	}

	ADD_TOOL(dtl_resd, "deathloop", " [in] [out]", "decompress deathloop resource file", dtl_resd);
}