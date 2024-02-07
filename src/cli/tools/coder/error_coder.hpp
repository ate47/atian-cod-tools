#pragma once

namespace error_coder {
	typedef const char* ErrorCode[4];

	/*
	 * Encode an error from a value
	 * @param code code to encode
	 * @param val error value to encode
	 */
	void Encode(ErrorCode& code, UINT32 val);
	/*
	 * Convert an error code to a string
	 * @param code to string
	 * @return value
	 */
	LPCCH ToStr(const ErrorCode& code);
	/*
	 * Decode an error to a value
	 * @param code code to decode
	 * @return error code
	 */
	UINT32 Decode(const ErrorCode& code);
}