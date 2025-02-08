#pragma once

namespace compatibility::scobalula::csi {
	constexpr const char* ACTS_CSI_LOCATION = "acts-pools.csi";
	enum CordycepGame : uint64_t {
		CG_GHOSTS = 0xA0A5354534F4847,
		CG_AW = 0x5241574E41564441,
		CG_IW = 0x4652415749464E49,
		CG_MWR = 0x30305453414D4552,
		CG_MW2R = 0x32305453414D4552,
		CG_MW4 = 0x3931524157444F4D,
		CG_MW5 = 0x3232524157444F4D,
		CG_MW6 = 0x4B4F4D41594D4159,
		CG_VANGUARD = 0x44524155474E4156,
		CG_BO3 = 0x33504f4b43414c42,
		CG_BO4 = 0x34504f4b43414c42,
		CG_BO6 = 0x36504f4b43414c42,
	};

	struct XAsset64 {
		uintptr_t Header;
		uint64_t Temp;
		uintptr_t Next;
		uintptr_t Previous;
		uint64_t ID;
		uint64_t Type;
		int64_t HeaderSize;
	};

	struct XAssetPool64 {
		uintptr_t Root;
		uintptr_t End;
		uint64_t LookupTable;
		uint64_t HeaderMemory;
		uint64_t AssetMemory;
	};

	const char* CordycepGameName(CordycepGame game);
	std::filesystem::path ActsCsiLocation();

	struct CordycepProc {
		CordycepGame gameId;
		uintptr_t poolsAddress{};
		uintptr_t stringsAddress{};
		uint32_t stringSize{};
		std::string gameDir;

		CordycepProc() {};
		CordycepProc(CordycepGame gameId, const std::string& gameDir) : gameId(gameId), gameDir(gameDir) {};

		bool ReadCsi(const std::filesystem::path& path);
		bool WriteCsi(const std::filesystem::path& path);
	};

	struct CordycepCsiHeader {
		CordycepGame gameId;
		uintptr_t poolsAddress;
		uintptr_t stringsAddress;
		uint32_t stringSize;
		byte gameDir[4];
	};

}