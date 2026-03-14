#include <includes.hpp>
#include "scobalula_csi.hpp"
#include <core/bytebuffer.hpp>

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
		default: {
			static char invName[sizeof(game) + 1];
			*reinterpret_cast<CordycepGame*>(&invName[0]) = game;
			invName[sizeof(game)] = 0;
			return invName;
		}
		}
	}
	std::filesystem::path ActsCsiLocation() {
		return utils::GetProgDir() / ACTS_CSI_LOCATION;
	}

	bool CordycepProc::ReadCsi(const std::filesystem::path& path) {
		std::vector<byte> dbBuff{};

		if (!utils::ReadFile(path, dbBuff)) {
			LOG_ERROR("Can't read Cordycep database: {}", path.string());
			return false;
		}

		LOG_DEBUG("Loading Cordycep CSI...");
		core::bytebuffer::ByteBuffer reader{ dbBuff };
		gameId = reader.Read<CordycepGame>();
		poolsAddress = reader.Read<uintptr_t>();;
		stringsAddress = reader.Read<uintptr_t>();;
		stringSize = reader.Read<uint32_t>();
		gameDir = reader.ReadString();

		if (reader.CanRead(sizeof(uint32_t))) {
			uint32_t numFlags{ reader.Read<uint32_t>() };
			for (size_t i = 0; i < numFlags; i++) {
				flags.emplace_back(reader.ReadSizedString<uint32_t>());
			}
		}

		LOG_DEBUG("Game Id .. {} (0x{:x})", CordycepGameName(gameId), (uint64_t)gameId);
		LOG_DEBUG("Pools .... 0x{:x}", poolsAddress);
		LOG_DEBUG("Strings .. 0x{:x}", stringsAddress);
		LOG_DEBUG("Game dir . '{}'", gameDir);
		return true;
	}


	bool CordycepProc::WriteCsi(const std::filesystem::path& path) {
		std::vector<byte> data{};

		utils::WriteValue<uint64_t>(data, gameId);
		utils::WriteValue<uintptr_t>(data, poolsAddress);
		utils::WriteValue<uintptr_t>(data, stringsAddress);
		utils::WriteValue<uint32_t>(data, stringSize);
		utils::WriteString(data, gameDir.c_str());
		utils::WriteValue<uint32_t>(data, (uint32_t)flags.size());
		for (std::string& flag : flags) {
			utils::WriteValue<uint32_t>(data, (uint32_t)flag.size());
			utils::WriteValue(data, flag.data(), flag.size());
		}

		return utils::WriteFile(path, data);
	}


}