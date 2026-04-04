#pragma once

#if __has_include(<deps/miniz.hpp>)
#define ACTS_HAS_MINIZ
#ifndef __ACTS_MINIZ_IMPL
#define MINIZ_HEADER_FILE_ONLY
#endif
#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES
#include <zip_file.hpp>
#endif