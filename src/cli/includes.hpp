#pragma once

#define _WINSOCKAPI_
#include <includes_shared.hpp>
#include <zlib.h>

#pragma warning(push)
#pragma warning(disable:4455)
#include <PS4DBG.hpp>
#pragma warning(pop)

#include "hashutils.hpp"

#include "mods/mods.hpp"

#include "tools/tools.hpp"
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include "tools/gsc_opcodes.hpp"
#include "tools/pool.hpp"

#include "compatibility/serious.hpp"

#if __has_include("tools/ps4/ps4_includes.hpp")

#include "tools/ps4/ps4_includes.hpp"

#endif

#if __has_include("tools/cw/cw.hpp")

#include "tools/cw/cw.hpp"

#endif

extern LPCCH g_progPath;