#pragma once
#include <tools/gsc.hpp>

namespace tool::gsc {
	int DecompileGsc(byte* data, size_t size, std::filesystem::path fsPath, GscDecompilerGlobalContext& gdctx);
}