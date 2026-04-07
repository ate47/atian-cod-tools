#pragma once
#include "api.hpp"
/*
 * Internal API used by Atian Tools
 */

// init cli
ACTS_COMMON_API int InitActsAPI(bool cli, int* argc, const char*** argv, uint32_t version);