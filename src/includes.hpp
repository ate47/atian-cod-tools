#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <mutex>

#include "memapi.hpp"
#include "hashutils.hpp"
#include "scriptinstance.hpp"

#include "tools/hash.hpp"
#include "tools/dump.hpp"

#include "offsets.hpp"

typedef int(*toolfunction)(const process& proc, int argc, const char* argv[]);