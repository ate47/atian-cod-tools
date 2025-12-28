#pragma once
#include <tools/gsc/gsc.hpp>

namespace tool::gsc {
	int DecompileGsc(byte* data, size_t size, std::filesystem::path fsPath, GscDecompilerGlobalContext& gdctx, byte* dbgData = nullptr, size_t dbgSize = 0);
}