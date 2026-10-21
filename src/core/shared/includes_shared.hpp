#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <array>
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
#include <cstdint>
#include <cstdarg>

#include <core/logs.hpp>

typedef uint8_t byte;

template<typename T, size_t N>
constexpr char (*ActsArraySizeCounter(T (&)[N]))[N];

#define ACTS_ARRAYSIZE(A) (sizeof(*ActsArraySizeCounter(A)))

#define ROTL8(v, n) (uint8_t)(((uint8_t)(v) << ((n) & 7)) | ((uint8_t)(v) >> (8 - ((n) & 7))))
#define ROTR8(v, n) (uint8_t)(((uint8_t)(v) >> ((n) & 7)) | ((uint8_t)(v) << (8 - ((n) & 7))))

#define ROTL16(v, n) (uint16_t)(((uint16_t)(v) << ((n) & 15)) | ((uint16_t)(v) >> (16 - ((n) & 15))))
#define ROTR16(v, n) (uint16_t)(((uint16_t)(v) >> ((n) & 15)) | ((uint16_t)(v) << (16 - ((n) & 15))))

#define B_LOWORD(v) (uint16_t)((v) & 0xFFFFu)
#define B_HIWORD(v) (uint16_t)(((v) >> 16) & 0xFFFFu)

#define B_LOBYTE(v) (uint8_t)((v) & 0xFFu)
#define B_HIBYTE(v) (uint8_t)(((v) >> 8) & 0xFFu)

#if __has_include(<xorstr.hpp>)
#include <xorstr.hpp>

#ifdef ACTS_SECURE_STRINGS
#define actssec(str) xorstr_(str)
#else
#define actssec(str) str
#endif
#else
#define actssec(str) str
#endif

#ifndef DECLSPEC_NORETURN
#if defined(_MSC_VER)
#define DECLSPEC_NORETURN __declspec(noreturn)
#elif defined(__GNUC__) || defined(__clang__)
#define DECLSPEC_NORETURN __attribute__((noreturn))
#else
#define DECLSPEC_NORETURN
#endif
#endif

#include <platform/platform.hpp>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#ifdef ERROR
#undef ERROR
#endif