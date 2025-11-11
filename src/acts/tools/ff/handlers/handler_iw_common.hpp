#pragma once

namespace fastfile::handlers {
	enum StreamPointerFlag : uint64_t {
		SPF_CREATE_REF = 1ull << 61, // create a offset with the data as name
		SPF_NEXT = 2ull << 61, // load the data after, otherwise data is name of offset
		SPF_UNK = 4ull << 61, // unk

		SPF_DATA_MASK = ~(SPF_CREATE_REF | SPF_NEXT | SPF_UNK)
	};
}