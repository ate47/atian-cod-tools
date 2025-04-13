#pragma once

namespace compatibility::data_signature {
	enum DataType {
		DATA_UNKNOWN = 0,
		DATA_GSC,
		DATA_GSCBIN,
		DATA_LUA_HK,
		DATA_LUA_JIT,
		DATA_ZLIB,
		DATA_PNG,
	};

	DataType ExtractType(void* data, size_t len);
}