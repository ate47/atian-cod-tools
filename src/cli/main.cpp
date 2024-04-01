#include <includes.hpp>
#include "hashutils.hpp"
#include "compatibility/scobalula_wni.hpp"
#include "actscli.hpp"
#include "actslib/logging.hpp"

namespace {
	inline bool ShouldHandleACTSOptions(int argc, const char* argv[]) {
		return argc != 1 && argv[1][0] == '-'; // at least one param
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
			else if (!strcmp("-s", arg) || !_strcmpi("--strings", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				opt.defaultHashFile = argv[++i];
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
				case 't':
				case 'T':
					alogs::setlevel(alogs::LVL_TRACE);
					actslib::logging::SetLevel(actslib::logging::LEVEL_TRACE);
					break;
				case 'd':
				case 'D':
					alogs::setlevel(alogs::LVL_DEBUG);
					actslib::logging::SetLevel(actslib::logging::LEVEL_DEBUG);
					break;
				case 'i':
				case 'I':
					alogs::setlevel(alogs::LVL_INFO);
					actslib::logging::SetLevel(actslib::logging::LEVEL_INFO);
					break;
				case 'w':
				case 'W':
					alogs::setlevel(alogs::LVL_WARNING);
					actslib::logging::SetLevel(actslib::logging::LEVEL_WARNING);
					break;
				case 'e':
				case 'E':
					alogs::setlevel(alogs::LVL_ERROR);
					actslib::logging::SetLevel(actslib::logging::LEVEL_ERROR);
					break;
				default:
					LOG_ERROR("Invalid log value for param: {}/{}", arg, val);
					return false;
				}
				alogs::setbasiclog(false);
				actslib::logging::SetBasicLog(false);
			}
			else if (!strcmp("-L", arg) || !_strcmpi("--log-file", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				alogs::setbasiclog(false);
				actslib::logging::SetBasicLog(false);
				alogs::setfile(argv[++i]);
				actslib::logging::SetLogFile(argv[i]);
			}
			else if (!strcmp("-x", arg) || !_strcmpi("--extracted", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				opt.dumpHashmap = argv[++i];
			}
			else if (!strcmp("-p", arg) || !_strcmpi("--pack", arg)) {
				if (i + 1 == argc) {
					LOG_ERROR("Missing value for param: {}!", arg);
					return false;
				}
				opt.packFile = argv[++i];
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
		LOG_INFO("Usage: {} (OPTIONS) [TOOL] (TOOL ARGS)", argv0);
		LOG_INFO("General tools:");
		LOG_INFO("- list : the tools list");
		LOG_INFO("- search (query) : search for a tool");
		LOG_INFO("");
		LOG_INFO("Options:");
		LOG_INFO(" -? --help -h       : Help");
		LOG_INFO(" -l --log [l]       : Set log level t(race)/d(ebug)/i(nfo)/w(arn)/e(rror), default: i");
		LOG_INFO(" -L --log-file [f]  : Set the log file");
		LOG_INFO(" -x --extracted [f] : Write the extracted hashes into a file after the process");
		LOG_INFO(" -t --no-title      : Hide ACTS title at start");
		LOG_INFO(" -p --pack [f]      : Load ACTS pack file");
		LOG_INFO(" -P --profiler [f]  : Save profiler file after tool usage");
		LOG_INFO(" -N --no-hash       : No default hash");
		LOG_INFO(" -H --no-install    : No install hashes");
		LOG_INFO(" -T --no-treyarch   : No Treyarch hash (ignored with -N)");
		LOG_INFO(" -I --no-iw         : No IW hash (ignored with -N)");
		LOG_INFO(" -s --strings [f]   : Set default hash file, default: '{}' (ignored with -N)", hashutils::DEFAULT_HASH_FILE);
		LOG_INFO(" -D --db2-files [f] : Load DB2 files at start, default: '{}'", compatibility::scobalula::wni::packageIndexDir);
		LOG_INFO(" -w --wni-files [f] : Load WNI files at start, default: '{}'", compatibility::scobalula::wni::packageIndexDir);
	}
}

int main(int argc, const char *_argv[]) {
	auto& profiler = actscli::GetProfiler();
	g_progPath = _argv[0];
	// by default we don't display heavy logs
	alogs::setbasiclog(true);
	actslib::logging::SetBasicLog(true);

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

	auto& opt = actscli::options();

	if (opt.showTitle) {
		LOG_INFO("Atian tools {}", actsinfo::VERSION);
	}

	if (opt.showHelp || argc == 1) {
		PrintACTSHelp(argv[0]);
		return 0;
	}

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
			return -1;
		}
	}

	const auto& tool = tool::findtool(argv[1]);

	if (!tool) {
		LOG_ERROR("Error: Bad tool name. {} list for the tools list", *argv);
		bool find{};
		const char* query[]{ argv[1] };
		tool::search(query, 1, [&find](const tool::toolfunctiondata* tool) {
			if (!find) {
				LOG_ERROR("Similar tool name(s):");
				find = true;
			}
			LOG_ERROR("- {}", tool->m_name);
		});

		return -1;
	}

	Process proc(tool.m_game);

	if (tool.m_game) {

		if (!proc) {
			LOG_ERROR("Can't find game process: {}", utils::WStrToStr(tool.m_game));
			return -1;
		}
		LOG_INFO("Find process {} {}", utils::WStrToStr(tool.m_game), proc);

		if (!proc.Open()) {
			LOG_ERROR("Can't open game process: 0x{:x}", GetLastError());
			return -1;
		}
	}

	hashutils::SaveExtracted(opt.dumpHashmap != nullptr);

	const clock_t beginTime = clock();

	int output;
	{
		actslib::profiler::ProfiledSection ps{ profiler, tool.m_name ? tool.m_name : "no-tool-name" };
#ifndef DEBUG
		try {
#endif
			output = tool.m_func(proc, argc, argv);
#ifndef DEBUG
		}
		catch (std::exception& e) {
			LOG_ERROR("Unhandled exception: {}", e.what());
			output = tool::BASIC_ERROR;
		}
#endif
	}

	LOG_TRACE("Tool took {}s to run with output {}{}", (double)(clock() - beginTime) / CLOCKS_PER_SEC, output, 
		(output == tool::OK ? " (OK)" : output == tool::BAD_USAGE ? " (BAD_USAGE)" : output == tool::BASIC_ERROR ? " (BASIC_ERROR)" : "")
	);

	hashutils::WriteExtracted(opt.dumpHashmap);

	if (output == tool::BAD_USAGE) {
		LOG_ERROR("Error: Bad tool usage: {} {} {}", *argv, argv[1], tool.m_usage);
	}

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

	return output;
}