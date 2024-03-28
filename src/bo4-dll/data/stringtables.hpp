#pragma once
#include <dll_includes.hpp>

namespace stringtables {
	void SyncTables();

	void* GetTable(uint64_t name);
}
