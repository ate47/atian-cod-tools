#pragma once
#include <utils/hash.hpp>

namespace core::system {
	typedef std::function<void(uint64_t uid)> SystemCallback;

	void Register(uint64_t uid, SystemCallback init, SystemCallback postinit = nullptr);

	void Ignore(uint64_t uid);

	void Init();

	void PostInit();

	class SystemContainer {
	public:
		SystemContainer(uint64_t uid, SystemCallback init, SystemCallback postinit = nullptr, bool ignored = false);
	};
}
#define REGISTER_SYSTEM(id, ...) static core::system::SystemContainer __systemcontainer_##id(hash::Hash64(#id), __VA_ARGS__)
#define IGNORE_SYSTEM(id, ...) static core::system::SystemContainer __systemcontainer_##id(hash::Hash64(#id), nullptr, nullptr, true)