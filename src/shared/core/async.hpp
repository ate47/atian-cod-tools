#pragma once

namespace core::async {
	bool IsAsync();
	void SetAsync(bool async);

	template<typename Mutex>
	class opt_lock_guard {
	private:
		Mutex* lock;
		bool isLocked;

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