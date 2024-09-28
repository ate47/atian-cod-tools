#pragma once

namespace core::async {
	enum AsyncTypes : uint64_t {
		AT_ANY = 1,
		AT_LOGS = 2,
		AT_HASHES = 4,

		AT_ALL = ~0ull,
	};

	bool IsSync(AsyncTypes type);
	uint64_t GetAsyncTypes();
	void SetAsync(uint64_t types);

	template<typename Mutex>
	class opt_lock_guard {
	private:
		Mutex* lock;
	public:
		opt_lock_guard(Mutex* lock) : lock(lock) {
			if (lock) {
				lock->lock();
			}
		}
		~opt_lock_guard() {
			if (lock) {
				lock->unlock();
			}
		}
	};
}