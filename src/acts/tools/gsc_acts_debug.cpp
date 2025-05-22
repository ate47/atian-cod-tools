#include <includes.hpp>
#include <gsc/gsc_acts_debug.hpp>

namespace {

	int acts_debug_decomp(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;
		using namespace shared::gsc::acts_debug;

		std::string data{ utils::ReadFile<std::string>(argv[2]) };

		GSC_ACTS_DEBUG* dbg{ (GSC_ACTS_DEBUG*)data.data() };

		if (dbg->GetMagic() != MAGIC) {
			LOG_ERROR("Bad magic");
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Version . 0x{:x}", dbg->version);
		LOG_INFO("ACTS .... 0x{:x}", dbg->actsVersion);

		if (dbg->HasFeature(ADF_CHECKSUM)) {
			LOG_INFO("CRC ..... 0x{:x}", dbg->checksum);
		}
		if (dbg->HasFeature(ADF_CRC_LOC) && dbg->crc_offset) {
			LOG_INFO("CRCOFF .. 0x{:x}", dbg->crc_offset);
		}

		if (dbg->HasFeature(ADF_LINES)) {
			GSC_ACTS_LINES* lines{ dbg->GetLines() };
			GSC_ACTS_LINES* linesend{ dbg->GetLinesEnd() };
			for (; lines != linesend; lines++) {
				LOG_INFO("line 0x{:x}->0x{:x} line {}", lines->start, lines->end, lines->lineNum);
			}
		}

		if (dbg->HasFeature(ADF_FILES)) {
			GSC_ACTS_FILES* files{ dbg->GetFiles() };
			GSC_ACTS_FILES* filesend{ dbg->GetFilesEnd() };
			for (; files != filesend; files++) {
				LOG_INFO("line {}->{} line {}", files->lineStart, files->lineEnd, dbg->GetString(files->filename));
			}
		}

		return tool::OK;
	}
	ADD_TOOL(acts_debug_decomp, "gsc", " [in]", "decomp acts gsc debug files", acts_debug_decomp);
}