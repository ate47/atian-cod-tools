#pragma once

#include <includes_shared.hpp>
#include <detours.h>

#include "bo4.hpp"
#include "cli_connect.hpp"
#include "custom_gsc_func.hpp"
#include "hash_lookup.hpp"
#include "error_handler.hpp"

#define EXPORT extern "C" __declspec(dllexport)