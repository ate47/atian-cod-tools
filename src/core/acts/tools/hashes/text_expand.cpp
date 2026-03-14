#include <includes.hpp>
#include <BS_thread_pool.hpp>
#include <core/async.hpp>
#include "text_expand.hpp"
#include <unit_test.hpp>

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
				buffer[idx++] = dict[(v - 1) % n];
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
				for (size_t i = 1 + t; i < max; i += threads) {
					size_t v = i;
					size_t idx{};
					while (v) {
						buffer[idx++] = dict[(v - 1) % n];
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
		buffer[off] = 0;
	}

	void GetDynamicAsyncDictPtr(size_t max, void(*func)(const char** str, void* data), const char** dict, BS::thread_pool* pool, void* data, AsyncStop* stop) {
		if (!dict || !*dict) throw std::runtime_error("Empty dictionary");

		size_t n{};
		while (dict[n]) {
			n++;
		}
		n++; // end nullptr

		size_t threads = pool->get_thread_count();
		for (size_t t = 0; t < threads; t++) {
			if (stop) stop->IncRun();
			pool->detach_task([t, n, max, threads, func, data, dict, stop]() -> void {
				thread_local const char* buffer[0x100]{ nullptr };
				for (size_t i = 1 + t; i < max; i += threads) {
					size_t v = i;
					size_t idx{};
					while (v) {
						buffer[idx++] = dict[(v - 1) % n];
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

	namespace {
		void text_expand_sync_test() {
			struct MapVal {
				size_t tests{};
				char str[4];
			};
			std::unordered_map<uint64_t, MapVal> map{};

			char dict[]{ "abcdefgh" };
			constexpr size_t dictSize = sizeof(dict);

			char buff[4]{};
			for (size_t i = 0; i < dictSize; i++) {
				for (size_t j = 0; j < dictSize; j++) {
					for (size_t k = 0; k < dictSize; k++) {
						buff[0] = dict[i];
						buff[1] = dict[j];
						buff[2] = dict[k];

						uint64_t h{ ::hash::Hash64A(buff) };
						MapVal& v{ map[h] };
						std::memcpy(v.str, buff, sizeof(v.str));
						LOG_TRACE("add val {}", v.str);
					}
				}
			}

			GetDynamicPtr(dictSize * dictSize * dictSize, [](const char* str, void* data) {
				std::unordered_map<uint64_t, MapVal>& map{ *(std::unordered_map<uint64_t, MapVal>*)data };
				uint64_t h{ ::hash::Hash64A(str) };
				auto it{ map.find(h) };
				LOG_TRACE("check entry {}", str);

				ASSERT_VAL(std::format("missing entry for key {}", str), it != map.end());

				it->second.tests++;
			}, &map, dict, dictSize);

			for (auto& [k, v] : map) {
				LOG_TRACE("check val {}", v.str);
				ASSERT_VAL(std::format("missing val for key {}", v.str), v.tests);
			}
		}


		void text_expand_async_test() {
			struct MapVal {
				size_t tests{};
				char str[4];
			};
			struct MapInfo {
				std::unordered_map<uint64_t, MapVal> map{};
				std::mutex mtx{};
			};

			MapInfo map{};

			char dict[]{ "abcdefgh" };
			constexpr size_t dictSize = sizeof(dict);

			char buff[4]{};
			for (size_t i = 0; i < dictSize; i++) {
				for (size_t j = 0; j < dictSize; j++) {
					for (size_t k = 0; k < dictSize; k++) {
						buff[0] = dict[i];
						buff[1] = dict[j];
						buff[2] = dict[k];

						uint64_t h{ ::hash::Hash64A(buff) };
						MapVal& v{ map.map[h] };
						std::memcpy(v.str, buff, sizeof(v.str));
						LOG_TRACE("add val {}", v.str);
					}
				}
			}

			GetDynamicAsyncPtr(dictSize * dictSize * dictSize, [](const char* str, void* data) {
				MapInfo& map{ *(MapInfo*)data };
				uint64_t h{ ::hash::Hash64A(str) };
				std::lock_guard lg{ map.mtx };

				auto it{ map.map.find(h) };

				LOG_TRACE("check entry {}", str);

				ASSERT_VAL(std::format("missing entry for key {}", str), it != map.map.end());

				it->second.tests++;
			}, &map, dict, dictSize);

			for (auto& [k, v] : map.map) {
				LOG_TRACE("check val {}", v.str);
				ASSERT_VAL(std::format("missing val for key {}", v.str), v.tests);
			}
		}

		const char* GenTmpStr(const char** strings, const char* mid) {
			static thread_local char buffer[0x200];

			char* b{ buffer };
			MergeInto(b, sizeof(buffer), strings, mid);
			return b;
		}

		template<bool sync>
		void text_expand_dict_async_test() {
			struct MapVal {
				size_t tests{};
				char str[32];
			};
			struct MapInfo {
				std::unordered_map<uint64_t, MapVal> map{};
				std::mutex mtx{};
			};

			MapInfo map{};

			const char* dict[]{
				"aaaaa",
				"bbbb",
				"ccc",
				"eeeee",
				"ddd",
				"gggggg",
				nullptr
			};
			constexpr size_t dictSize = sizeof(dict) / sizeof(*dict);
			constexpr const char* mid = "_";

			const char* arr[4]{};
			char buff[0x100]{};
			for (size_t i = 0; i < dictSize; i++) {
				for (size_t j = 0; j < dictSize; j++) {
					for (size_t k = 0; k < dictSize; k++) {
						arr[0] = dict[i];
						arr[1] = dict[j];
						arr[2] = dict[k];
						MergeInto(buff, sizeof(buff), arr, mid);

						uint64_t h{ ::hash::Hash64A(buff) };
						MapVal& v{ map.map[h] };
						std::memcpy(v.str, buff, sizeof(v.str));
						//LOG_TRACE("add val '{}'", v.str);
					}
				}
			}

			AsyncStop as{};

			BS::thread_pool pool{ sync ? 1 : 4 };
			GetDynamicAsyncDictPtr(dictSize* dictSize* dictSize, [](const char** strs, void* data) {
				MapInfo& map{ *(MapInfo*)data };
				const char* str{ GenTmpStr(strs, mid) };
				uint64_t h{ ::hash::Hash64A(str) };
				std::lock_guard lg{ map.mtx };
				auto it{ map.map.find(h) };
				//LOG_TRACE("check entry {}", str);


				ASSERT_VAL(std::format("missing entry for key '{}'", str), it != map.map.end());

				it->second.tests++;
			}, dict, &pool, &map, &as);

			pool.wait();
			as.running = false;

			for (auto& [k, v] : map.map) {
				//LOG_TRACE("check val {}", v.str);
				ASSERT_VAL(std::format("missing val for key {}", v.str), v.tests);
			}
		}

		ADD_TEST(text_expand_sync_test, text_expand_sync_test);
		ADD_TEST(text_expand_async_test, text_expand_async_test);
		ADD_TEST(text_expand_dict_sync_test, text_expand_dict_async_test<true>);
		ADD_TEST(text_expand_dict_async_test, text_expand_dict_async_test<false>);
	}
	
}