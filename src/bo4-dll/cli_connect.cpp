#include <dll_includes.hpp>

clisync::CliSyncData g_cliData{};


EXPORT void SyncCLI(clisync::CliSyncData* data) {
	// sync data
	g_cliData = *data;

	Beep(700, 300);
}