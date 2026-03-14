#pragma once

#include <dll_includes.hpp>

namespace error_handler {

	extern std::unordered_map<uint64_t, const char*> errors;

	/*
	 * Find the description of an error code
	 * @param code code
	 * @return Description or NULL
	 */
	const char* FindDesc(uint64_t code);
}