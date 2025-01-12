#include <includes.hpp>
#include <utils/utils.hpp>
#include "hashutils.hpp"
#include "decryptutils.hpp"
#include "compatibility/scobalula_wni.hpp"
#include "actscli.hpp"
#include <cli/clicolor.hpp>
#include "hook/error.hpp"
#include "actslib/logging.hpp"
#include "acts.hpp"
#include "main_ui.hpp"
#include "tools/tools_nui.hpp"
#include <core/config.hpp>
#include <core/updater.hpp>
#include <core/shared_cfg_data.hpp>

namespace {
	inline bool ShouldHandleACTSOptions(int argc, const char* argv[]) {
		return argc > 1 && argv[1][0] == '-'; // at least one param
	}

	bool HandleACTSOptions(int argc, const char* argv[], std::vector<const char*>& newData) {
		assert(argc > 0);
		newData.reserve((size_t)argc - 1); // remove start because we know we have at least one param
		newData.push_back(argv[0]);

		auto& opt = actscli::options();
		size_t i = 1;
		for (; i < argc; i++) {
			const char* arg = argv[i];
			if (*arg != '-') {
				break; // end of acts params
			}
			if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
				opt.showHelp = true;
			}
			else if (!strcmp("-F", arg) || !_strcmpi("--force-error", arg)) {
				opt.forceError = true;
			}
			else if (!strcmp("-t", arg) || !_strcmpi("--no-title", arg)) {
				opt.showTitle = false;
			}
			else if (!strcmp("-T", arg) || !_strcmpi("--no-treyarch", arg)) {
				opt.noTreyarchHash = true;
			}
			else if (!strcmp("-I", arg) || !_strcmpi("--no-iw", arg)) {
				opt.noIWHash = true;
			}
			else if (!strcmp("-N", arg) || !_strcmpi("--no-hash", arg)) {
				opt.noDefaultHash = true;
			}
			else if (!_strcmpi("--hash0", arg)) {
				opt.show0Hash = true;
			}
			else if (!_strcmpi("--heavy-hashes", arg)) {
				opt.heavyHashes = true;
			}
			else if (!_strcmpi("--no-private", arg)) {
				opt.noPrivate = true;
			}
			else if (!_strcmpi("--decrypt-mod", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				opt.decryptStringExec = argv[++i];
			}
			else if (!_strcmpi("--decrypt-t8", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				
				int64_t s{ utils::ParseFormatInt(argv[++i]) };

				if (s < 0 || s >= acts::decryptutils::T8D_COUNT) {
					LOG_ERROR("Invalid t8 decryption: {}!", argv[i]);
					return false;
				}
				acts::decryptutils::T8Decryption dec{ (acts::decryptutils::T8Decryption)s };
				acts::decryptutils::SetT8OldDecryption(dec);
				LOG_TRACE("Using t8dec {}", acts::decryptutils::GetT8OldDecryptionName(dec));
			}
			else if (!_strcmpi("--hashprefix", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				opt.hashPrefixByPass = argv[++i];
			}
			else if (!strcmp("-d", arg) || !_strcmpi("--debug", arg)) {
				hook::error::EnableHeavyDump();
				opt.debug = true;
			}
			else if (!strcmp("-s", arg) || !_strcmpi("--strings", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				opt.defaultHashFile = argv[++i];
			}
			else if (!_strcmpi("--log-path", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				core::logs::addlogpath(argv[++i]);
			}
			else if (!strcmp("-l", arg) || !_strcmpi("--log", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				auto* val = argv[++i];

				if (!val[0] || val[1]) {
					LOG_ERROR("Invalid log value for param: {}/{}", arg, val);
					return false;
				}

				switch (*val) {
				case 'p':
				case 'P':
					core::logs::setlevel(core::logs::LVL_TRACE_PATH);
					actslib::logging::SetLevel(actslib::logging::LEVEL_TRACE);
					break;
				case 't':
				case 'T':
					core::logs::setlevel(core::logs::LVL_TRACE);
					actslib::logging::SetLevel(actslib::logging::LEVEL_TRACE);
					break;
				case 'd':
				case 'D':
					core::logs::setlevel(core::logs::LVL_DEBUG);
					actslib::logging::SetLevel(actslib::logging::LEVEL_DEBUG);
					break;
				case 'i':
				case 'I':
					core::logs::setlevel(core::logs::LVL_INFO);
					actslib::logging::SetLevel(actslib::logging::LEVEL_INFO);
					break;
				case 'w':
				case 'W':
					core::logs::setlevel(core::logs::LVL_WARNING);
					actslib::logging::SetLevel(actslib::logging::LEVEL_WARNING);
					break;
				case 'e':
				case 'E':
					core::logs::setlevel(core::logs::LVL_ERROR);
					actslib::logging::SetLevel(actslib::logging::LEVEL_ERROR);
					break;
				default:
					LOG_ERROR("Invalid log value for param: {}/{}", arg, val);
					return false;
				}
				core::logs::setbasiclog(false);
				actslib::logging::SetBasicLog(false);
			}
			else if (!strcmp("-L", arg) || !_strcmpi("--log-file", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				core::logs::setbasiclog(false);
				actslib::logging::SetBasicLog(false);
				core::logs::setfile(argv[++i]);
				actslib::logging::SetLogFile(argv[i]);
			}
			else if (!_strcmpi("--mark-hash", arg)) {
				opt.markHash = true;
			}
			else if (!strcmp("-x", arg) || !_strcmpi("--extracted", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				opt.dumpHashmap = argv[++i];
				}
			else if (!_strcmpi("--extracted-unk", arg)) {
				opt.dumpHashmapUnknown = true;
			}
			else if (!strcmp("-p", arg) || !_strcmpi("--pack", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				opt.packFile = argv[++i];
			}
			else if (!strcmp("-W", arg) || !_strcmpi("--work", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				const char* val = argv[++i];
				if (!_strcmpi("repl", val)) {
					opt.type = actscli::ACTS_REPL;
				}
				else if (!_strcmpi("cli", val)) {
					opt.type = actscli::ACTS_CLI;
				}
				else if (!_strcmpi("ui", val)) {
					opt.type = actscli::ACTS_UI;
				}
				else if (!_strcmpi("nui", val)) {
					opt.type = actscli::ACTS_NUI;
				}
				else {
					LOG_ERROR("Invalid param value for param: {}!", arg);
					return false;
				}
			}
			else if (!strcmp("-P", arg) || !_strcmpi("--profiler", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				opt.saveProfiler = argv[++i];
			}
			else if (!strcmp("-w", arg) || !_strcmpi("--wni-files", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				opt.wniFiles = argv[++i];
			}
			else if (!strcmp("-D", arg) || !_strcmpi("--db2-files", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				opt.seriousDBFile = argv[++i];
			}
			else if (!strcmp("-H", arg) || !_strcmpi("--no-install", arg)) {
				opt.installDirHashes = false;
			}
			else {
				LOG_ERROR("Unknown acts option: {}!", arg);
				return false;
			}
		}

		// add end params
		for (; i < argc; i++) {
			newData.push_back(argv[i]);
		}

		newData.shrink_to_fit();

		return true;
	}

	void PrintACTSHelp(const char* argv0) {
		LOG_INFO("Repository: https://github.com/ate47/atian-cod-tools");
		LOG_INFO("Usage: {} (OPTIONS) [TOOL] (TOOL ARGS)", argv0);
		LOG_INFO("General tools:");
		LOG_INFO("- list (category) : list the tools");
		LOG_INFO("- search (query)  : search for a tool");
		LOG_INFO("");
		LOG_INFO("Options:");
		LOG_INFO(" -? --help -h       : Help");
		LOG_INFO(" -l --log [l]       : Set log level p(path)/t(race)/d(ebug)/i(nfo)/w(arn)/e(rror), default: i");
		LOG_INFO(" -L --log-file [f]  : Set the log file");
		LOG_INFO(" --log-path [p]     : Set the log path(s), semicolon separated");
		LOG_INFO(" -d --debug         : Enable debug mode");
		LOG_INFO(" -x --extracted [f] : Write the extracted hashes into a file after the process");
		LOG_INFO(" --extracted-unk    : with -x Extract the unknown values");
		LOG_INFO(" -t --no-title      : Hide ACTS title at start");
		LOG_INFO(" -p --pack [f]      : Load ACTS pack file");
		LOG_INFO(" -P --profiler [f]  : Save profiler file after tool usage");
		LOG_INFO(" -N --no-hash       : No default hash");
		LOG_INFO(" -H --no-install    : No install hashes");
		LOG_INFO(" -T --no-treyarch   : No Treyarch hash (ignored with -N)");
		LOG_INFO(" -I --no-iw         : No IW hash (ignored with -N)");
		LOG_INFO("--decrypt-mod [f]   : Use exe dump to decrypt strings");
		LOG_INFO("--decrypt-t8 [v]    : Set the bo4 decryption algorithm, default 0");
		LOG_INFO(" -s --strings [f]   : Set default hash file, default: '{}' (ignored with -N)", hashutils::DEFAULT_HASH_FILE);
		LOG_INFO(" -D --db2-files [f] : Load DB2 files at start, default: '{}'", compatibility::scobalula::wni::packageIndexDir);
		LOG_INFO(" -w --wni-files [f] : Load WNI files at start, default: '{}'", compatibility::scobalula::wni::packageIndexDir);
		LOG_INFO(" -W --work          : Tell which work to use: repl, cli");
		
		LOG_DEBUG(" --hash0            : Use \"hash_0\" instead of \"\" during lookup");
		LOG_DEBUG("--mark-hash         : Mark the hash default value");
		LOG_DEBUG("--hashprefix [p]    : Ignore the default prefix");
		LOG_DEBUG("--heavy-hashes      : Heavy hashes format");
		LOG_DEBUG("-F --force-error    : Force error");
		LOG_TRACE("--no-private        : No private tools");
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	return TRUE; // ignore
}

void* GetActsSharedConfig() {
	return core::shared_cfg::GetSharedConfigPtr();
}

void SetActsSharedConfig(void* cfg) {
	core::shared_cfg::SetSharedConfigPtr(cfg);
}

int MainActs(int argc, const char* _argv[], HINSTANCE hInstance, int nShowCmd) {
	core::config::SyncConfig(true);
	srand((unsigned int)time(nullptr));
	if (core::updater::CheckUpdate()) {
		return 0;
	}
	bool cli{ hInstance == nullptr };
	auto& profiler = actscli::GetProfiler();

	// by default we don't display heavy logs in cli

	if (cli) {
		core::logs::setbasiclog(true);
		actslib::logging::SetBasicLog(true);
	} else {
		static std::string uiLogs = [] {
			std::filesystem::path path{ utils::GetProgDir() / "acts-ui.log" };
			return path.string();
		}();
		core::logs::setfile(uiLogs.data());
		actslib::logging::SetLogFile(uiLogs.data());

		std::string logLevel = core::config::GetString("ui.logLevel", "INFO");

		if (logLevel != "INFO") {
			if (logLevel == "DEBUG") {
				core::logs::setlevel(core::logs::LVL_DEBUG);
				actslib::logging::SetLevel(actslib::logging::LEVEL_DEBUG);
			}
			else if (logLevel == "TRACE") {
				core::logs::setlevel(core::logs::LVL_TRACE);
				actslib::logging::SetLevel(actslib::logging::LEVEL_TRACE);
			}
			else if (logLevel == "ERROR") {
				core::logs::setlevel(core::logs::LVL_ERROR);
				actslib::logging::SetLevel(actslib::logging::LEVEL_ERROR);
			}
			else if (logLevel == "WARNING") {
				core::logs::setlevel(core::logs::LVL_WARNING);
				actslib::logging::SetLevel(actslib::logging::LEVEL_WARNING);
			}
		}
	}

	const char** argv;
	if (ShouldHandleACTSOptions(argc, _argv)) {
		static std::vector<const char*> newargv{};
		if (HandleACTSOptions(argc, _argv, newargv)) {
			argv = newargv.data();
			argc = (int)newargv.size();
		}
		else {
			return -1;
		}
	}
	else {
		argv = _argv;
	}
	hook::error::InstallErrorHooks();

	auto& opt = actscli::options();

	if (opt.showTitle && !hInstance) {
		LOG_INFO("Atian tools {} {}", core::actsinfo::VERSION, ([&opt]() -> const char* {
			switch (opt.type) {
				case actscli::ACTS_CLI: return "CLI";
				case actscli::ACTS_UI: return "UI";
				case actscli::ACTS_NUI: return "NUI";
				case actscli::ACTS_REPL: return "REPL";
				default: return "";
			}
		})());
	}

	bool useCli = !hInstance && opt.type == actscli::ACTS_CLI && ([]() {
		DWORD pid;
		GetWindowThreadProcessId(GetConsoleWindow(), &pid);
		return GetCurrentProcessId() != pid;
	})();

	if (useCli && (opt.showHelp || argc == 1)) {
		PrintACTSHelp(argv[0]);
		return 0;
	}

	// create default directories
	std::filesystem::create_directories(utils::GetProgDir() / compatibility::scobalula::wni::packageIndexDir);
	std::filesystem::path depsDir{ utils::GetProgDir() / "deps" };
	std::filesystem::create_directories(depsDir);
	std::wstring depsDirStr{ depsDir.wstring() };
	AddDllDirectory(depsDirStr.data());

	std::filesystem::path packFilePath;

	if (opt.packFile) {
		packFilePath = opt.packFile;
	}
	else {
		packFilePath = utils::GetProgDir() / compatibility::scobalula::wni::packageIndexDir;
	}

	std::vector<std::filesystem::path> packFiles{};

	utils::GetFileRecurse(packFilePath, packFiles, [](const std::filesystem::path& p) {
		auto s = p.string();
		return s.ends_with(".acpf");
	});

	for (const auto& acpf : packFiles) {
		if (!actscli::LoadPackFile(acpf)) {
			LOG_ERROR("Error when loading ACTS pack file {}", acpf.string());
			return tool::BASIC_ERROR;
		}
	}

	if (opt.decryptStringExec && !acts::decryptutils::LoadDecrypt(opt.decryptStringExec)) {
		return tool::BASIC_ERROR;
	}

	if (
		opt.type == actscli::ACTS_NUI
		 || (hInstance && core::config::GetBool("nui.force", true))
		) {
		tool::nui::OpenNuiWindow();
		return tool::OK;
	}

	if (hInstance || opt.type == actscli::ACTS_NUI) {
		return tool::ui::MainActsUI(hInstance, nShowCmd); // no tool to run, life's easier if I put that here
	}

	int output;

	if (useCli) {
		opt.type = actscli::ACTS_CLI;
	}
	else {
		opt.type = actscli::ACTS_REPL;
	}

	if (opt.type == actscli::ACTS_REPL) {
		LOG_INFO("'exit' to stop, 'help' for more information");
	}

	std::vector<const char*> params{};
	do {
		if (opt.type == actscli::ACTS_REPL) {
			std::string line{};
			
			if (cli::clicolor::ConsoleAllowColor()) {
				std::cout << cli::clicolor::Color(5, 1, 3) << "acts:" << core::actsinfo::VERSION << "> " << cli::clicolor::Color(5, 5, 5);
				std::getline(std::cin, line);
				std::cout << cli::clicolor::Reset();
			}
			else {
				std::cout << "acts:" << core::actsinfo::VERSION << "> ";
				std::getline(std::cin, line);
			}

			LOG_TRACE("Run line: {}", line);

			argv = tool::ReadParams(line, argc);

			if (!argv || argc < 2) {
				continue;
			}
		}

		output = tool::HandleCommand(argc, argv);
	} while (opt.type == actscli::ACTS_REPL && !opt.exitAfterEnd);

	if (opt.saveProfiler) {
		std::ofstream pout{ opt.saveProfiler, std::ios::binary };
		if (!pout) {
			LOG_ERROR("Can't open profiler output {}", opt.saveProfiler);
		}
		else {
			profiler.Stop();
			profiler.Write(pout);
			pout.close();
			LOG_INFO("Profiling saved into {}", opt.saveProfiler);
		}
	}

	if (opt.type == actscli::ACTS_REPL) {
		LOG_INFO("Goodbye, press return key to exit...");
		std::cin.get();
	}

	return output;
}