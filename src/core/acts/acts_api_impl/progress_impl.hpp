#pragma once
#include <acts_api/api.h>


class ActsProgressHandlerImpl {
	ActsProgressHandler* handler;
public:
	ActsProgressHandlerImpl(ActsProgressHandler* handler) : handler(handler) {}

	void Report(float progress, const char* message) {
		if (handler) {
			handler->handler(progress, message, handler->ud);
		}
	}

	void Report(size_t count, size_t total, const char* message) {
		Report(total == 0 ? 0 : (float)(100.0 * count / total), message);
	}
	void Report(float progress, const std::string& message) {
		Report(progress, message.c_str());
	}
	void Report(size_t count, size_t total, const std::string& message) {
		Report(count, total, message.c_str());
	}
};