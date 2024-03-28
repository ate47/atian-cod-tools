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


extern const char* g_progPath;