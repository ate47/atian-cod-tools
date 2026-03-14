#include <includes.hpp>
#include <tools/ps4/codtool.hpp>
namespace tool::ps4::codtool {
	namespace {

		void CBuf(CodTool* tool, const char* cbuf) {

		}

		void InjectGSC(CodTool* tool, const std::filesystem::path& path) {


		}

		utils::ArrayAdder<CodToolHandler> impl{ GetHandlers(), "Black Ops 6 Alpha", "eboot.bin", CBuf, InjectGSC };
	}
}