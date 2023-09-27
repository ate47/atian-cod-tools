#pragma once

#include <dll_includes.hpp>

namespace error_handler {

	struct error_info {
		UINT32 code;
		LPCCH description;
	};

	extern std::vector<error_info> errors;

	/*
	 * Find the description of an error code
	 * @param code code
	 * @return Description or NULL
	 */
	LPCCH FindDesc(UINT32 code);
}