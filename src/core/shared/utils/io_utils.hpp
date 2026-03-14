#pragma once

namespace utils::io {

	bool DownloadFile(const std::string& url, std::string& buffer, bool append = false);
	bool DownloadFile(const std::string& url, std::vector<byte>& buffer, bool append = false);


}