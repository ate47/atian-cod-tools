#pragma once
#include <gsc/gsc_acts_debug.hpp>

namespace shared::gsc::acts_debug {
	void GetGDBInfo(uint32_t rloc, GSC_ACTS_DEBUG* dbg, const char** filename, uint32_t* line);
}