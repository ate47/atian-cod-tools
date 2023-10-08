#pragma once

#define _WINSOCKAPI_
#include <includes_shared.hpp>
#include <zlib.h>

#pragma warning(push)
#pragma warning(disable:4455)
#include <PS4DBG.hpp>
#pragma warning(pop)

#include "memapi.hpp"
#include "hashutils.hpp"

#include "mods/mods.hpp"

#include "tools/tools.hpp"
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include "tools/gsc_opcodes.hpp"
#include "tools/pool.hpp"

extern LPCCH g_progPath;