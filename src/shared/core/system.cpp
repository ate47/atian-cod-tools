#include "includes_shared.hpp"
#include "system.hpp"

namespace core::system {
	enum SystemFlags {
		SF_INIT = 1,
		SF_POSTINIT = 2,
		SF_IGNORED = 4,
	};
	struct System {
		uint64_t uid{};
		SystemCallback init{};
		SystemCallback postinit{};
		int flags{};
	};
	class Systems {
	public:
		std::unordered_map<uint64_t, System> loaded{};
	};

	static Systems& GetSystems() {
		static Systems sys{};
		return sys;
	}

	void Register(uint64_t uid, SystemCallback init, SystemCallback postinit) {
		auto& sys = GetSystems().loaded[uid];

		sys.uid = uid;
		sys.init = init;
		sys.postinit = postinit;
	}

	void Ignore(uint64_t uid) {
		GetSystems().loaded[uid].flags |= SF_IGNORED;
	}

	void Init() {
		for (auto& [uid, sys] : GetSystems().loaded) {
			if (!(sys.flags & (SF_INIT | SF_IGNORED)) && sys.init) {
				sys.init(uid);
				sys.flags |= SF_INIT;
			}
		}
	}

	void PostInit() {
		for (auto& [uid, sys] : GetSystems().loaded) {
			if (!(sys.flags & (SF_POSTINIT | SF_IGNORED)) && sys.postinit) {
				sys.postinit(uid);
				sys.flags |= SF_POSTINIT;
			}
		}
	}

	SystemContainer::SystemContainer(uint64_t uid, SystemCallback init, SystemCallback postinit, bool ignored) {
		if (ignored) {
			Ignore(uid);
		}
		else {
			Register(uid, init, postinit);
		}
	}

}