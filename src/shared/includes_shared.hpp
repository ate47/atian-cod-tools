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
#include <stack>
#include <mutex>
#include <filesystem>
#include <cassert>
#include <functional>
#include <random>

#if __has_include("asmjit/asmjit.h")
#define ASMJIT_STATIC
#define ASMJIT_NO_AARCH64
#define ASMJIT_BUILD_RELEASE
#define ASMJIT_NO_FOREIGN
#include <asmjit/asmjit.h>
#endif

#include <logs.hpp>
#include <actsinfo.hpp>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#ifdef ERROR
#undef ERROR
#endif