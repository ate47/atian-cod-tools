#pragma once

#include <Windows.h>
#include <conio.h>
#include <TlHelp32.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <set>
#include <mutex>
#include <filesystem>
#include <cassert>
#include <functional>

#include "memapi.hpp"
#include "hashutils.hpp"
#include "scriptinstance.hpp"
#include "utils.hpp"
#include "logs.hpp"
#include "decrypt.hpp"

#include "mods/mods.hpp"

#include "tools/tools.hpp"
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include "tools/gsc_opcodes.hpp"
#include "tools/pool.hpp"

#include "offsets.hpp"
