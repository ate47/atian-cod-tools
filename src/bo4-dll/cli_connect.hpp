#pragma once
#include <dll_includes.hpp>

extern clisync::CliSyncData g_cliData;

namespace cliconnect {
	void RegisterDetour(LPCCH title, LPVOID pointer, LPVOID detour);

	template<typename Output, typename... Args>
	class DetourInfo {
		typedef Output(__fastcall* Func)(Args...);
		uintptr_t m_location;
		Func m_pointer;
		Func m_detour;
	public:
		DetourInfo(LPCCH title, uintptr_t location, Func detour) 
			: m_location(location), m_pointer(reinterpret_cast<Func>(&(bo4::BasePtr()[location]))), m_detour(detour) {
			RegisterDetour(title, &m_pointer, m_detour);
		}

		inline Output operator()(Args... args) {
			return m_pointer(args...);
		}
	};

}
