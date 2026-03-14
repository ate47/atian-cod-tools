#pragma once

namespace fastfile::dump {
	constexpr uint64_t BIN_MAGIC = 0x3054534c48415842;

	struct BinXAssetListHeader {
		uint64_t magic{ BIN_MAGIC };
		uint64_t assetOffset{};
		uint64_t assetCount{};
		uint64_t stringsOffsetStart{};
		uint64_t stringsOffsetEnd{};
		uint64_t stringsCount{};
	};

	struct BinXAsset {
		uint32_t type;
		uint32_t idx;
		uint64_t start;
		uint64_t size;
	};
}