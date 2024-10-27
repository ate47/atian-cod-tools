#include <includes.hpp>
#include "scobalula_csi.hpp"

namespace compatibility::scobalula::csi {

	const char* CordycepGameName(CordycepGame game) {
		switch (game) {
		case CG_GHOSTS: return "Ghosts";
		case CG_AW: return "Advanced Warfare";
		case CG_IW: return "Infinite Warfare";
		case CG_MWR: return "Modern Warfare Remastered";
		case CG_MW2R: return "Modern Warfare 2 Remastered";
		case CG_MW4: return "Modern Warfare 2019";
		case CG_MW5: return "Modern Warfare II";
		case CG_MW6: return "Modern Warfare III";
		case CG_VANGUARD: return "Vanguard";
		case CG_BO6: return "Black Ops 6";
		default: return "<unknown>";
		}
	}

	bool CordycepProc::ReadCsi(const std::filesystem::path& path) {
		std::string dbBuff{};

		if (!utils::ReadFile(path, dbBuff)) {
			LOG_ERROR("Can't read Cordycep database: {}", path.string());
			return false;
		}

		LOG_DEBUG("Loading Cordycep CSI...");
		CordycepCsiHeader* header{ (CordycepCsiHeader*)dbBuff.data() };

		const char* buffer{ dbBuff.data() };
		size_t idx{ offsetof(CordycepCsiHeader, gameDir) };

		gameDir.resize(header->stringSize);
		memcpy(gameDir.data(), buffer + idx, header->stringSize);
		idx += header->stringSize;


		LOG_DEBUG("Game Id .. {} (0x{:x})", CordycepGameName(header->gameId), (uint64_t)header->gameId);
		LOG_DEBUG("Pools .... 0x{:x}", header->poolsAddress);
		LOG_DEBUG("Strings .. 0x{:x}", header->stringsAddress);
		LOG_DEBUG("Game dir . '{}'", gameDir);

		gameId = header->gameId;
		poolsAddress = header->poolsAddress;
		stringsAddress = header->stringsAddress;

		return true;
	}


}