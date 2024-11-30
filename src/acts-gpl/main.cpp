#include <includes_shared.hpp>
#include <acts.hpp>
#include <tools/tools_data.hpp>

int main(int argc, const char* argv[]) {

	RegisterActsTool("gpltest", "dev", "", "shit test", [](int argc, const char* argv[]) -> int {
		LOG_INFO("test"); // TODO: we need to sync the configs of the shared lib

		return tool::OK;
	});

	return MainActs(argc, argv, nullptr);
}