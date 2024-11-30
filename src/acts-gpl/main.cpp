#include <includes_shared.hpp>
#include <acts.hpp>
#include <tools/tools_data.hpp>
#include <core/shared_cfg_data.hpp>

int main(int argc, const char* argv[]) {
	SetActsSharedConfig(core::shared_cfg::GetSharedConfigPtr());

	RegisterActsTool("gpltest", "dev", "", "shit test", [](int argc, const char* argv[]) -> int {
		LOG_INFO("test");

		return tool::OK;
	});

	return MainActs(argc, argv, nullptr);
}