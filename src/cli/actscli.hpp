#pragma once

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
	};

	ActsOptions& options();

	bool LoadPackFile(LPCCH packFile);
}