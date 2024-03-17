#pragma once
#include "actslib/profiler.hpp"

namespace actscli {
	struct ActsOptions {
		bool showTitle{ true };
		bool showHelp{};
		LPCCH defaultHashFile{};
		LPCCH packFile{};
		bool noDefaultHash{};
		bool noTreyarchHash{};
		bool noIWHash{};
		LPCCH dumpHashmap{};
		LPCCH wniFiles{};
		LPCCH saveProfiler{};
	};

	ActsOptions& options();
	actslib::profiler::Profiler& GetProfiler();

	bool LoadPackFile(LPCCH packFile);
}