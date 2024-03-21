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

namespace actslib {

	const char* va(const char* fmt, ...);

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

	template<typename Type>
	class ToClose {
		Type& v;

	public:
		ToClose(Type& v) : v(v) {}
		~ToClose() {
			v.close();
		}
	};
	class ToCloseFunc {
		std::function<void()> f;

	public:
		ToCloseFunc(std::function<void()> f) : f(f) {}
		~ToCloseFunc() {
			f();
		}
	};


}