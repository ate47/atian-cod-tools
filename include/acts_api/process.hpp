#pragma once
#include "api.hpp"
/*
 * Atian Tools Process API
 */

namespace acts::api {

	// DumpProcessExe options
	struct DumpProcessOption {
		// rebuild the IAT table when mixed by AC
		bool rebuildIAT{};
		// out message buffer
		char* outMessage{};
		// out message buffer size
		size_t outMessageSize{};
	};

	class ActsAPIProcess {
	public:
		/*
		 * Dump a process executable file
		 * @param in input file
		 * @param out output file (if nullptr, deps/[in]_dump.exe will be used)
		 * @param opt dump options
		 * @return success or failure, message added to outMessage if configured
		 */
		virtual bool DumpProcessExe(const char* in, const char* out, DumpProcessOption* opt = nullptr) = 0;
	};

}

// @return process api
ACTS_COMMON_API acts::api::ActsAPIProcess& ActsAPIProcess();