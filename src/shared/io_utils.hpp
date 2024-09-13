#pragma once

namespace utils::io {

	bool DownloadFile(const std::string& url, std::string& buffer, bool append = false);


}