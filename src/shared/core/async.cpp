#include <includes_shared.hpp>
#include "async.hpp"

namespace core::async {
	namespace {
		bool asyncMode{};
	}
	bool IsAsync() {
		return asyncMode;
	}
	void SetAsync(bool async) {
		asyncMode = async;
	}
}