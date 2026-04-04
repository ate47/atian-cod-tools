#pragma once
#if __has_include(<curl/curl.h>)
#define ACTS_HAS_IO_UTILS
#endif

namespace utils::io {

	bool DownloadFile(const std::string& url, std::string& buffer, bool append = false);
	bool DownloadFile(const std::string& url, std::vector<byte>& buffer, bool append = false);


}