#pragma once

namespace compatibility::xensik::gscbin {
	// .gscbin file
	// https://github.com/xensik/gsc-tool?tab=readme-ov-file#file-format

	constexpr uint32_t GSCBIN_MAGIC = 0x435347;

	struct GscBinHeader {
		byte magic[sizeof(GSCBIN_MAGIC)];
		uint32_t compressedLen;
		uint32_t len;
		uint32_t bytecodeLen;

		constexpr byte* GetBuffer() {
			return magic + sizeof(GscBinHeader);
		}

		constexpr byte* GetByteCode() {
			return magic + sizeof(GscBinHeader) + compressedLen;
		}

		inline uint32_t& Magic() {
			return *reinterpret_cast<uint32_t*>(magic);
		}
	};

}