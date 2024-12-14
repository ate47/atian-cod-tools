#include <includes_gpl.hpp>
#include <CommandParser.h>
#include <ProcessDumper.h>
#include <Parasyte.h>
#include <GameHandler.h>
#include <CoDMW4Handler.h>
#include <CoDMW5Handler.h>
#include <CoDMW6Handler.h>
#include <CoDVGHandler.h>
#include <hook/module_mapper.hpp>

namespace {
	int cord_dump(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;

		ps::ProcessDumper dumper;

		if (!dumper.Dump(argv[2])) {
			LOG_ERROR("Failed to dump {}", argv[2]);
			return tool::BASIC_ERROR;
		}


		LOG_INFO("Dumped {}", argv[2]);

		return tool::OK;
	}

	void FixupCordy() {
		ps::CoDMW4Handler mw4{};
		ps::CoDMW5Handler mw5{};
		ps::CoDMW6Handler mw6{};
		ps::CoDVGHandler vg{};


		std::wstring currDir{ std::filesystem::current_path().wstring() };
		AddDllDirectory(currDir.data());
		std::wstring cordyDeps{ std::filesystem::absolute("Data/Deps").wstring() };
		AddDllDirectory(cordyDeps.data());
	}


	int cord_dp(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

		ps::Parasyte& parasyte{ ps::Parasyte::Instance() };
		AddDllDirectory(L"Data\\Dumps");
		FixupCordy();

		std::string game{ argv[2] };
		std::string path{ argv[3] };

		ps::GameHandler* handler{};

		for (auto& h : ps::GameHandler::GetHandlers()) {
			if (h->IsValid(game)) {
				handler = h.get();
				break;
			}
		}

		if (!handler) {
			LOG_ERROR("Can't find handler for game: {}", game);
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Using handler {}", handler->GetName());
		// sethandler [game]
		parasyte.SetCurrentHandler(handler);

		// init [path]
		if (!handler->Initialize(path)) {
			LOG_ERROR("Can't init handler {}", handler->GetName());
			return tool::BASIC_ERROR;
		}

		return tool::OK;
	}

	ADD_TOOL(cord_dump, "common", " [cmd] (params)", "Cordycep command handler", cord_dump);
	ADD_TOOL(cord_dp, "common", " [game] [path]", "Cordycep dp", cord_dp);
}
