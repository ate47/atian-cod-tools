#include <includes.hpp>
#include <BS_thread_pool.hpp>
#include <core/async.hpp>
#include "text_expand.hpp"

namespace tool::hash::text_expand {
	void GetDynamicPtr(size_t max, void(*func)(const char* str, void* data), void* data) {
		char buffer[0x100]{ 0 };
		constexpr const char dict[] = "abcdefghijklmnopqrstuvwxyz0123456789_";
		constexpr size_t n = sizeof(dict) - 1;

		for (size_t i = 1; i < max; i++) {
			size_t v = i;
			size_t idx{};
			while (v) {
				buffer[idx++] = dict[v % n];
				v /= n;
			}
			buffer[idx] = 0;
			func(buffer, data);
		}
	}

	void GetDynamicAsyncPtr(size_t max, void(*func)(const char* str, void* data), void* data) {
		constexpr const char dict[] = "abcdefghijklmnopqrstuvwxyz0123456789_";
		constexpr size_t n = sizeof(dict) - 1;

		BS::thread_pool pool{};

		size_t threads = pool.get_thread_count();
		LOG_TRACE("Load dynamic thread pool {}", threads);
		for (size_t t = 0; t < threads; t++) {
			pool.detach_task([t, max, threads, func, data]() -> void {
				thread_local char buffer[0x100]{ 0 };
				for (size_t i = t; i < max; i += threads) {
					size_t v = i;
					size_t idx{};
					while (v) {
						buffer[idx++] = dict[v % n];
						v /= n;
					}
					buffer[idx] = 0;
					func(buffer, data);
					if ((i & 0xFFFFFFF) == 0) {
						LOG_TRACE("done {} -> {}", i, buffer);
					}
				}
			});
		}
		pool.wait();
	}

	void GetDynamicAsyncDictPtr(size_t max, void(*func)(const char** str, void* data), const char** dict, void* data) {
		size_t n{};
		while (dict[n]) {
			n++;
		}

		if (!n) throw std::runtime_error("Empty dictionary");

		BS::thread_pool pool{};

		size_t threads = pool.get_thread_count();
		for (size_t t = 0; t < threads; t++) {
			pool.detach_task([t, n, max, threads, func, data, dict]() -> void {
				thread_local const char* buffer[0x100]{ nullptr };
				for (size_t i = t; i < max; i += threads) {
					size_t v = i;
					size_t idx{};
					while (v) {
						buffer[idx++] = dict[v % n];
						v /= n;
					}
					buffer[idx] = nullptr;
					func(buffer, data);
					if ((i & 0xFFFFFF) == 0 && alogs::getlevel() <= alogs::LVL_TRACE) {
						std::ostringstream oss{};
						for (size_t j = 0; j < idx; j++) {
							oss << " " << buffer[j];
						}

						LOG_TRACE("done {} ->{}", i, oss.str());
					}
				}
			});
		}
		pool.wait();
	}

	
}