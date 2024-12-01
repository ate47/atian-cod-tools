#include <includes_shared.hpp>
#include <acts.hpp>
#include <tools/tools_data.hpp>
#include <core/shared_cfg_data.hpp>
#include <Log.h>

namespace {
	// used to set the name of the file, we can't use the main.cpp one
	constexpr const char* cordycepName = "cordycep";
	void PrintCordycep(const char* header, const char* msg, bool inlined) {
		core::logs::log(core::logs::LVL_INFO, header, cordycepName, 0, msg);
	}
	void PrintCordycep2(ps::LogType type, const char* msg, bool inlined) {
		core::logs::loglevel lvl;
		switch (type) {
			case ps::LogType::Normal:
				lvl = core::logs::LVL_INFO;
				break;
			case ps::LogType::Verbose:
				lvl = core::logs::LVL_TRACE;
				break;
			case ps::LogType::Debug:
				lvl = core::logs::LVL_DEBUG;
				break;
			case ps::LogType::Error:
				lvl = core::logs::LVL_ERROR;
				break;
			default:
				lvl = core::logs::LVL_INFO; // unknown???
				break;
		}
		core::logs::log(lvl, core::logs::name(lvl), cordycepName, 0, msg);
	}
}

int main(int argc, const char* argv[]) {
	SetActsSharedConfig(core::shared_cfg::GetSharedConfigPtr());

	ps::log::SetOnPrint(PrintCordycep);
	ps::log::SetOnPrint2(PrintCordycep2);
	ps::log::EnableLogType((ps::LogType)0xFFF);

	RegisterActsTool("gpltest", "dev", "", "shit test", [](int argc, const char* argv[]) -> int {
		LOG_INFO("test");

		ps::log::Log(ps::LogType::Normal, "normal log");
		ps::log::Log(ps::LogType::Verbose, "verbose log");
		ps::log::Log(ps::LogType::Error, "error log");
		ps::log::Log(ps::LogType::Debug, "debug log");
		ps::log::Print("Normal", "test log 2 %d", 5);

		return tool::OK;
	});

	return MainActs(argc, argv, nullptr);
}
