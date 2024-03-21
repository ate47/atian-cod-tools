#pragma once
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
#include <stdarg.h>
#include "../mio.hpp"
#include "logging.hpp"

/*
 * General imports and utilities for actslib
 */
namespace actslib {
	constexpr size_t MAX_VA_ALLOC = 0x20;
	constexpr size_t MAX_VA_LEN = 0x200;
	/*
	 * format a string and return it, the string will be allocated until a MAX_VA_ALLOC amount of calls of this function.
	 * The max size is MAX_VA_LEN.
	 * @param fmt format
	 * @return va string
	 */
	const char* va(const char* fmt, ...);

	/*
	 * Basic formatter for std::format, use the operator<< implementation
	 * @param Type type to format
	 */
	template<typename Type>
	struct BasicFormatter {
		template<class ParseContext>
		constexpr ParseContext::iterator parse(ParseContext& ctx) {
			auto it = ctx.begin();
			if (it == ctx.end()) {
				return it;
			}
			if (*it != '}') {
				throw std::format_error("Invalid format args.");
			}

			return it;
		}

		template<class FmtContext>
		FmtContext::iterator format(const Type& p, FmtContext& ctx) const {
			std::ostringstream out;

			out << p;

			return std::ranges::copy(std::move(out).str(), ctx.out()).out;
		}
	};

	/*
	 * Close object, call the close method with the destructor
	 * @param Type type to close
	 */
	template<typename Type>
	class ToClose {
		Type& v;

	public:
		ToClose(Type& v) : v(v) {}
		inline ~ToClose() {
			v.close();
		}
	};
	/*
	 * Close function, call the function with the destructor
	 */
	class ToCloseFunc {
		std::function<void()> f;

	public:
		ToCloseFunc(std::function<void()> f) : f(f) {}
		inline ~ToCloseFunc() {
			f();
		}
	};


}