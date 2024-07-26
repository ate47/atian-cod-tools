#include <includes.hpp>


#ifndef CI_BUILD
#include <jsonrpccxx/server.hpp>
#include <core/config.hpp>

namespace {
	int test(Process& proc, int argc, const char* argv[]) {

		return tool::OK;
	}

	int cfgtest(Process& proc, int argc, const char* argv[]) {
		using namespace core::config;
		SyncConfig(true);

		LOG_INFO("file: {}", GetMainConfig().configFile.string());

		LOG_INFO("{}", GetInteger("test", 64));
		LOG_INFO("{}", GetBool("test2.bool.val", true));
		LOG_INFO("{}", GetInteger("test2.int", 64));
		LOG_INFO("{}", GetDouble("zqdzqd.dzqdzq.zdqzdh.hqzdz", 42.69));
		LOG_INFO("{}", GetString("str", "dzqzqdzdqzdq"));

		LOG_INFO("{}", GetString("ui.injector.path"));

		//main["test3"] = 42;
		//
		//LOG_INFO("Key:");
		//for (auto& [key, val] : main.GetObj()) {
		//	LOG_INFO("- {}", key.GetString());
		//}

		SetString("zdzdq", "qzddqzqzdqzd");

		SaveConfig();

		return tool::OK;
	}
}

ADD_TOOL("test", "dev", "", "Tests", nullptr, test);
ADD_TOOL("cfgtest", "dev", "", "", nullptr, cfgtest);
#endif