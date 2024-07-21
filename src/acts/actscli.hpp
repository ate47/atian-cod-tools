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
		bool markHash{};
		const char* dumpHashmap{};
		const char* wniFiles{};
		bool installDirHashes{ true };
		bool show0Hash{ false };
		const char* saveProfiler{};
		const char* seriousDBFile{};
	};

	ActsOptions& options();
	actslib::profiler::Profiler& GetProfiler();

	bool LoadPackFile(const std::filesystem::path& packFile);
}