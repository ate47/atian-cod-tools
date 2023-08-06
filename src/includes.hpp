#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <mutex>
#include <filesystem>

#include "memapi.hpp"
#include "hashutils.hpp"
#include "scriptinstance.hpp"
#include "utils.hpp"

#include "tools/tools.hpp"
#include "tools/hash.hpp"
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include "tools/decrypt.hpp"
#include "tools/gsc_opcodes.hpp"

#include "offsets.hpp"
