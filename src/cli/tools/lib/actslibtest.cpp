#include <actslib/actslib.hpp>
#include <actslib/profiler.hpp>
#include <includes.hpp>

namespace {
	int actslibtest(Process& proc, int argc, const char* argv[]) {
		using namespace actslib::profiler;
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

}

#ifndef CI_BUILD
ADD_TOOL("actslibtest", "", "Acts lib test", nullptr, actslibtest);
ADD_TOOL("actslibprofiler", " [profile file]", "Read profiler", nullptr, actslibprofiler);
#endif