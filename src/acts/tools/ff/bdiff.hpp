#pragma once

namespace fastfile::bdiff {
	struct BDiffState {
		bool headerRead{};
		bool error{};
		bool eof{};
		unsigned int features{};
		uint32_t unk08;
		uint32_t checksum;
	};
}