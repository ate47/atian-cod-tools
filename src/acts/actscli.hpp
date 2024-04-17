#pragma once
#include "actslib/profiler.hpp"

namespace actscli {
	struct ActsOptions {
		bool showTitle{ true };
		bool showHelp{};
		const char* defaultHashFile{};
		const char* packFile{};
		bool noDefaultHash{};
		bool noTreyarchHash{};
		bool noIWHash{};
		const char* dumpHashmap{};
		const char* wniFiles{};
		bool installDirHashes{ true };
		const char* saveProfiler{};
		const char* seriousDBFile{};
	};

	ActsOptions& options();
	actslib::profiler::Profiler& GetProfiler();

	bool LoadPackFile(const std::filesystem::path& packFile);
}