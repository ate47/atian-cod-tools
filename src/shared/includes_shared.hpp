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
#include <unordered_set>
#include <mutex>
#include <filesystem>
#include <cassert>
#include <functional>

#define ASMJIT_STATIC
#define ASMJIT_NO_AARCH64
#define ASMJIT_BUILD_RELEASE
#define ASMJIT_NO_FOREIGN
#include <asmjit/asmjit.h>

#include "utils.hpp"
#include "logs.hpp"
#include "decrypt.hpp"
#include "decryptiw.hpp"
#include "clisync.hpp"
#include "offsets.hpp"
#include "hash.hpp"
#include "pool.hpp"
#include "scriptinstance.hpp"
#include "actsinfo.hpp"
#include "process.hpp"
#include "memapi.hpp"
#include "base64.hpp"

#include "t8_internal.hpp"
