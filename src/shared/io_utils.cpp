#include "includes_shared.hpp"
#define CURL_STATICLIB
#include <curl/curl.h>
#include "io_utils.hpp"
#include "utils.hpp"
#include "actsinfo.hpp"

#pragma comment(lib, "wldap32")
#pragma comment(lib, "ws2_32")

namespace utils::io {

	static size_t WriteFunction(void* buffer, size_t size, size_t nmemb, void* userp) {
		std::string& out = *static_cast<std::string*>(userp);

		out.append((char*)buffer, size * nmemb);

		return size * nmemb;
	}
	
	bool DownloadFile(const std::string& url, std::string& buffer, bool append) {
		CURL* curl = curl_easy_init();

		if (!curl) throw std::runtime_error("can't init curl");

		utils::CloseEnd ce{ [curl] {
			curl_easy_cleanup(curl);
		} };

		if (!append) {
			buffer.clear();
		}

		curl_easy_setopt(curl, CURLOPT_URL, url.data());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFunction);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (std::string*)&buffer);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "acts/0.1");

		CURLcode ok = curl_easy_perform(curl);

		if (ok != CURLE_OK) {
			throw std::runtime_error(utils::va("curl error: %s", curl_easy_strerror(ok)));
		}

		long code{};
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

		if (code >= 200 && code < 300) return true;

		throw std::runtime_error(utils::va("http error: %d", code));
	}


}