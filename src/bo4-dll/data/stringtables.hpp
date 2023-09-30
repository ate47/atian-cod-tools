#pragma once
#include <dll_includes.hpp>

namespace stringtables {
	void SyncTables();

	LPVOID GetTable(UINT64 name);
}
