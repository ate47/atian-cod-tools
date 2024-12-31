#pragma once
#include "actslib/profiler.hpp"

namespace actscli {
	enum ActsType {
		ACTS_UI,
		ACTS_NUI,
		ACTS_CLI,
		ACTS_REPL,
	};
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
		bool dumpHashmapUnknown{ false };
		const char* wniFiles{};
		bool installDirHashes{ true };
		bool show0Hash{ false };
		const char* saveProfiler{};
		const char* seriousDBFile{};
		const char* hashPrefixByPass{};
		const char* decryptStringExec{};
		bool heavyHashes{};
		bool forceError{};
		bool noPrivate{};
		ActsType type{ ACTS_CLI };
		bool exitAfterEnd{};
		bool debug{};
	};

	ActsOptions& options();
	actslib::profiler::Profiler& GetProfiler();

	bool LoadPackFile(const std::filesystem::path& packFile);
}