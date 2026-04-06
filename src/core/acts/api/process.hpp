#pragma once
#include <acts_api.hpp>

namespace acts::api {
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
		virtual bool DumpProcessExe(const char* in, const char* out, DumpProcessOption* opt = nullptr) = 0;
	};

}

ACTS_COMMON_API acts::api::ActsAPIProcess& ActsAPIProcess();