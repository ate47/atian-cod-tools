#pragma once

#include <includes_shared.hpp>
#include <detours.h>
#include <clisync.hpp>
#include <process.hpp>

#include "bo4.hpp"

#include "data/custom_gsc_func.hpp"
#include "data/error_handler.hpp"
#include "data/hash_lookup.hpp"
#include "data/find_linking.hpp"
#include "data/stringtables.hpp"

#define EXPORT extern "C" __declspec(dllexport)

namespace inject {
	enum DllType {
		DLLTYPE_UNKNOWN = 0,
		DLLTYPE_CLIENT,
		DLLTYPE_EXTENSION,
	};

	extern DllType g_dllType;

	extern clisync::CliSyncData g_cliData;

	struct InjectedSystem {
		static void InjectSystem(InjectedSystem* sys);
		static std::vector<InjectedSystem*>& GetSystems();

		LPCCH name;
		UINT level;
		std::function<void()> preinit;
		std::function<void()> postinit;

		inline InjectedSystem(LPCCH name, UINT level, std::function<void()> preinit = nullptr, std::function<void()> postinit = nullptr)
		: name(name), level(level), preinit(preinit), postinit(postinit) {
			assert(name && "a system name can't be null");
			InjectSystem(this);
		}
	};

	enum DetourTime {
		PRE_INIT,
		POST_INIT
	};

	struct DetourRegistryData {
		LPCCH title;
		LPVOID pointer;
		LPVOID detour;
		DetourTime time;
	};

	void RegisterDetour(DetourRegistryData* data);

	template<typename Output, typename... Args>
	class DetourInfo {
	public:
		typedef Output(__fastcall* Func)(Args...);
		// function to detour location
		Func m_pointer;
		// detour function
		Func m_detour;
		// registry info
		DetourRegistryData data;
	public:
		DetourInfo(LPCCH title, void* location, Func detour, DetourTime time = POST_INIT)
			: m_pointer(reinterpret_cast<Func>(location)), m_detour(detour),
			data(title, &m_pointer, m_detour, time) {
			RegisterDetour(&data);
		}

		DetourInfo(LPCCH title, uintptr_t location, Func detour, DetourTime time = POST_INIT)
			: m_pointer(reinterpret_cast<Func>(&(process::BasePtr()[location]))), m_detour(detour),
			data(title, &m_pointer, m_detour, time) {
			RegisterDetour(&data);
		}

		inline Output operator()(Args... args) {
			return m_pointer(args...);
		}
	};

	enum INJECT_PRIORITY : UINT {
		PRIORITY_INIT = 0,
		PRIORITY_NORMAL = 0x1000,
		PRIORITY_DETOURS = 0xFFFFFFFF,
	};
}

#define ADD_INJECTED_SYSTEM(id, ...) static inject::InjectedSystem alloc_sys_##id { __VA_ARGS__ }
