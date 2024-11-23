#if false
#include <includes_shared.hpp>
#include <utils/hash.hpp>
#include <hook/error.hpp>
#include <hook/library.hpp>
#include "gsctools.hpp"

namespace {
	constexpr const char* fileName = "compiled.gscc";

	hook::library::Detour GscLinkDetour;
	std::string gscBuffer{};

	int GscLinkStub(void* ctx, mwiii::gsc::GscObj23* obj, uint32_t* error) {
		if ((obj->name & 0x7FFFFFFFFFFFFFFF) == 0x3f51a039c4a1a113) {
			static bool linking{};
			if (!linking) {
				linking = true;
				LOG_TRACE("Link custom script...");
				int errd{};
				GscLinkDetour.Call<int>(ctx, gscBuffer.data(), &errd);
				LOG_TRACE("End link custom script.");
				linking = false;
			}
		}
		
		return GscLinkDetour.Call<int>(ctx, obj, error);
	}

	void FastInject() {
		core::logs::setfile("acts-mwiii.log");
		core::logs::setlevel(core::logs::LVL_TRACE);
		LOG_INFO("Init fast inject dll");

		hook::error::InstallErrorHooks(false);
		hook::error::EnableHeavyDump();


		if (!utils::ReadFile(fileName, gscBuffer)) {
			LOG_ERROR("Can't read {}", fileName);
			return;
		}

		mwiii::gsc::GscObj23* primeObj{ reinterpret_cast<mwiii::gsc::GscObj23*>(gscBuffer.data()) };

		if (primeObj->GetMagic() != mwiii::gsc::GSC_MAGIC) {
			LOG_ERROR("Can't read {}: Invalid Magic 0x{:x}", fileName, primeObj->GetMagic());
			return;
		}

		LOG_INFO("GSC file loaded");

		hook::library::InitScanContainer("acts-mwiii.scan");

		GscLinkDetour.Create(hook::library::QueryScanContainerSinglePtr<void*>("gsc-link", "40 55 57 41 57 48 81 EC 80 02"), GscLinkStub);

		hook::library::SaveScanContainer();

		LOG_INFO("fast inject detours created");
	}
}



BOOL WINAPI DllMain(HMODULE hModule, DWORD Reason, LPVOID lpVoid) {
	if (Reason == DLL_PROCESS_ATTACH) {
		FastInject();
	}
	return TRUE;
}

#endif