#pragma once

#include <dll_includes.hpp>

namespace error_handler {

	extern std::unordered_map<UINT64, LPCCH> errors;

	/*
	 * Find the description of an error code
	 * @param code code
	 * @return Description or NULL
	 */
	LPCCH FindDesc(UINT64 code);
}