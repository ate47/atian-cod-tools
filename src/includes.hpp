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

#include "compiler/gsc_compiler.hpp"

#include "mods/t8_custom_ee.hpp"

#include "tools/tools.hpp"
#include "tools/decrypt.hpp"
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include "tools/gsc_opcodes.hpp"
#include "tools/hash.hpp"
#include "tools/pool.hpp"
#include "tools/process.hpp"
#include "tools/vm_debug.hpp"

#include "offsets.hpp"
