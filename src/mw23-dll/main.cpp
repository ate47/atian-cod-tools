#include <includes_shared.hpp>
#include <hook/error.hpp>
#include <hook/library.hpp>

#define EXPORT extern "C" __declspec(dllexport)

namespace {
	void Main() {
		alogs::setfile("acts-mwiii.log");
		alogs::setlevel(alogs::LVL_TRACE);
		LOG_INFO("Init fast inject dll");

		hook::error::InstallErrorHooks(false);
		hook::error::EnableHeavyDump();


		//hook::library::InitScanContainer("acts-mwiii.scan");

		//hook::library::SaveScanContainer();

		LOG_INFO("system loaded");
	}
}
BOOL WINAPI DllMain(HMODULE hModule, DWORD Reason, LPVOID lpVoid) {
	if (Reason == DLL_PROCESS_ATTACH) {
		Main();
	}
	return TRUE;
}


// hook dxgi.dll for auto injection
EXPORT HRESULT CreateDXGIFactory1(void* riid, void* ppFactory) {
	static auto func = [] {
		hook::library::Library dxgi{ "dxgi.dll", true };

		if (!dxgi) throw std::runtime_error(utils::va("can't find system dxgi.dll"));

		return reinterpret_cast<decltype(&CreateDXGIFactory1)>(dxgi["CreateDXGIFactory1"]);
	}();

	return func(riid, ppFactory);
}


EXPORT HRESULT CreateDXGIFactory(void* riid, void* ppFactory) {
	static auto func = [] {
		hook::library::Library dxgi{ "dxgi.dll", true };

		if (!dxgi) throw std::runtime_error(utils::va("can't find system dxgi.dll"));

		return reinterpret_cast<decltype(&CreateDXGIFactory)>(dxgi["CreateDXGIFactory"]);
	}();

	return func(riid, ppFactory);
}
