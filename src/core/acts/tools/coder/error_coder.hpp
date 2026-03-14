#pragma once

namespace error_coder {
	typedef const char* ErrorCode[4];

	/*
	 * Encode an error from a value
	 * @param code code to encode
	 * @param val error value to encode
	 * @param alternative use alternative
	 */
	void Encode(ErrorCode& code, uint32_t val, bool alternative);
	/*
	 * Convert an error code to a string
	 * @param code to string
	 * @return value
	 */
	const char* ToStr(const ErrorCode& code);
	/*
	 * Decode an error to a value
	 * @param code code to decode
	 * @return error code
	 */
	uint32_t Decode(const ErrorCode& code);
}