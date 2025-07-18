#include <includes.hpp>
#include <BS_thread_pool.hpp>
#include <core/async.hpp>
#include "text_expand.hpp"

namespace tool::hash::text_expand {

	void AsyncStop::Stop() {
		std::lock_guard lg{ mutex };
		stopTrigger = true;
	}
	
	bool AsyncStop::IsStop() {
		std::lock_guard lg{ mutex };
		return stopTrigger;
	}
	bool AsyncStop::IsRunning() {
		std::lock_guard lg{ mutex };
		return running;
	}
	void AsyncStop::IncRun() {
		std::lock_guard lg{ mutex };
		running++;
	}
	void AsyncStop::DecRun() {
		std::lock_guard lg{ mutex };
		if (!running) throw std::runtime_error("Overflow DecRun");
		running--;
	}

	void GetDynamicPtr(size_t max, void(*func)(const char* str, void* data), void* data, const char* dict, size_t n) {
		char buffer[0x100]{ 0 };
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

	void GetDynamicAsyncPtr(size_t max, void(*func)(const char* str, void* data), void* data, const char* dict, size_t n) {
		BS::thread_pool pool{};

		size_t threads = pool.get_thread_count();
		LOG_TRACE("Load dynamic thread pool {}", threads);
		for (size_t t = 0; t < threads; t++) {
			pool.detach_task([t, max, threads, func, data, dict, n]() -> void {
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

	void GetDynamicAsyncDictPtr(size_t max, void(*func)(const char** str, void* data), const char** dict, void* data, AsyncStop* stop) {
		BS::thread_pool pool{};
		GetDynamicAsyncDictPtr(max, func, dict, &pool, data, stop);
		pool.wait();
		if (stop) stop->running = false;
	}

	void MergeInto(char* buffer, size_t len, const char** str, const char* mid) {
		size_t off{};

		for (const char** s = str; *s; s++) {
			if (off) {
				off += std::snprintf(&buffer[off], len - off, "%s", mid);
			}
			off += std::snprintf(&buffer[off], len - off, "%s", *s);
		}
	}

	void GetDynamicAsyncDictPtr(size_t max, void(*func)(const char** str, void* data), const char** dict, BS::thread_pool* pool, void* data, AsyncStop* stop) {
		size_t n{};
		while (dict[n]) {
			n++;
		}

		if (!n) throw std::runtime_error("Empty dictionary");

		size_t threads = pool->get_thread_count();
		for (size_t t = 0; t < threads; t++) {
			if (stop) stop->IncRun();
			pool->detach_task([t, n, max, threads, func, data, dict, stop]() -> void {
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
					if ((i & 0xFFFFFF) == 0) {
						if (stop && stop->IsStop()) break; // end task
						if (HAS_LOG_LEVEL(core::logs::LVL_TRACE)) {
							std::ostringstream oss{};
							for (size_t j = 0; j < idx; j++) {
								oss << " " << buffer[j];
							}

							LOG_TRACE("done {} ->{}", i, oss.str());
						}
					}
				}
				stop->DecRun();
			});
		}
	}

	
}