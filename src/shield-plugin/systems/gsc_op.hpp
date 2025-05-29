#pragma once

namespace systems::gsc::op {
	constexpr uint16_t MOD_TOOL_FLAG = 0x1000;

	enum OP_CODE : uint16_t {
		OP_LazyLink = 0x16,
	};

	enum OP_CODE_MOD_TOOL : uint16_t {
		OPMT_LazyLink = 0x18,
	};
}