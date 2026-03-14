#include <includes_shared.hpp>
#include "async.hpp"

namespace core::async {
	namespace {
		uint64_t asyncMode{};
	}
	bool IsSync(AsyncTypes type) {
		return (asyncMode & type) != 0;
	}
	uint64_t GetAsyncTypes() {
		return asyncMode;
	}
	void SetAsync(uint64_t types) {
		asyncMode = types;
	}
}