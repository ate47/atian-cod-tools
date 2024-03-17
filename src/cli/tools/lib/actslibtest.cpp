#include <actslib/actslib.hpp>
#include <actslib/profiler.hpp>
#include <includes.hpp>
#include <actslib/crc.hpp>
#include <actslib/hdt.hpp>
#include <mio.hpp>

namespace {
	int actslibtest(Process& proc, int argc, const char* argv[]) {
		const char* type;
		if (argc < 3) {
			type = "prof";
		}
		else {
			type = argv[2];
		}

		using namespace actslib::profiler;

		if (!_strcmpi(type, "prof")) {
			Profiler profiler{ "test" };

			{
				ProfiledSection test1{ profiler , "test 1" };
				Sleep(500);
			}
			{
				ProfiledSection test2{ profiler , "test 2" };
				{
					ProfiledSection test2{ profiler , "test 2-1" };
					Sleep(300);
				}
				{
					ProfiledSection test2{ profiler , "test 2-2" };
					Sleep(400);
				}
			}

			profiler.Stop();
			profiler.WriteToStr(std::cout);
		}
		else if (!_strcmpi(type, "cookie")) {

			actslib::hdt::HDTCookie cookie{ actslib::hdt::HCT_UNKNOWN, "http://atesab.fr/#actsTest" };

			cookie["test"] = "test2";
			cookie["test1"] = "test3";

			LOG_INFO("Type:   {}", actslib::hdt::FormatName(cookie.GetType()));
			LOG_INFO("Format: {}", cookie.GetFormat());

			LOG_INFO("Props:");
			for (const auto& [key, val] : cookie) {
				LOG_INFO("'{}' = {}", key, val);
			}

			LOG_INFO("===============");
			cookie.Save("testcookie.cookie");

			std::ifstream is{ "testcookie.cookie", std::ios::binary};

			if (!is) {
				LOG_ERROR("Can't open {}", "testcookie.cookie");
				return tool::BASIC_ERROR;
			}

			try {
				actslib::hdt::HDTCookie cookie{ is };
				is.close();
				LOG_INFO("Type:   {}", actslib::hdt::FormatName(cookie.GetType()));
				LOG_INFO("Format: {}", cookie.GetFormat());

				LOG_INFO("Props:");
				for (const auto& [key, val] : cookie) {
					LOG_INFO("'{}' = {}", key, val);
				}
			}
			catch (std::runtime_error& err) {
				LOG_ERROR("Can't read cookie {}", err.what());
			}
			is.close();
			std::filesystem::remove("testcookie.cookie");
		}
		return tool::OK;
	}

	int actslibprofiler(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		std::ifstream is{ argv[2], std::ios::binary };

		if (!is) {
			LOG_ERROR("Can't open {}", argv[2]);
			return tool::BASIC_ERROR;
		}

		try {
			actslib::profiler::Profiler prof{ is };
			is.close();

			prof.WriteToStr(std::cout);
		}
		catch (std::runtime_error& err) {
			LOG_ERROR("Can't read profiler {}", err.what());
			is.close();
			return tool::BASIC_ERROR;
		}
		is.close();

		return tool::OK;
	}

	int actslibhdtcookie(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		std::ifstream is{ argv[2], std::ios::binary };

		if (!is) {
			LOG_ERROR("Can't open {}", argv[2]);
			return tool::BASIC_ERROR;
		}

		try {
			actslib::hdt::HDTCookie cookie{ is };
			is.close();
			LOG_INFO("Type:   {}", actslib::hdt::FormatName(cookie.GetType()));
			LOG_INFO("Format: {}", cookie.GetFormat());

			LOG_INFO("Props:");
			for (const auto& [key, val] : cookie) {
				LOG_INFO("'{}' = {}", key, val);
			}
		}
		catch (std::runtime_error& err) {
			LOG_ERROR("Can't read profiler {}", err.what());
			is.close();
			return tool::BASIC_ERROR;
		}
		is.close();

		return tool::OK;
	}

}

#ifndef CI_BUILD
ADD_TOOL("actslibtest", "", "Acts lib test", nullptr, actslibtest);

ADD_TOOL("actslibhdt", " [hdt]", "Read hdt cookie", nullptr, actslibhdtcookie);
#endif

ADD_TOOL("actslibprofiler", " [profile file]", "Read profiler", nullptr, actslibprofiler);