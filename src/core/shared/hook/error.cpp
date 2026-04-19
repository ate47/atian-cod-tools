#include <includes_shared.hpp>
#include "module_mapper.hpp"
#include "error.hpp"
#include "process.hpp"
#include "library.hpp"
#include <utils/utils.hpp>
#include "memory.hpp"


namespace hook::error {
	static ErrorConfig cfg{};

	ErrorConfig& GetErrorConfig() {
		return cfg;
	}

	bool GetLocInfo(const void* location, uintptr_t& relativeLocation, const char*& moduleName) {
		void* hmod = library::GetLibraryInfo(location);

		if (!hmod) {
			relativeLocation = reinterpret_cast<uintptr_t>(location);
			if (hook::memory::IsInsideNearContainer(location)) {
				moduleName = "NearContainer";
				return true;
			}

			moduleName = "";
			return false;
		}

		relativeLocation = (reinterpret_cast<uintptr_t>(location) - reinterpret_cast<uintptr_t>(hmod));
		moduleName = library::GetLibraryName(hmod);


		return true;
	}

	void DumpStackTraceFrom(core::logs::loglevel level, const void* location) {
		platform::DumpStackTraceFrom(level, location);
	}

	void InstallErrorHooks(bool clearSetFunction) {
		platform::InstallErrorHooks(clearSetFunction);
	}

	void AddErrorDumper(void(*dumper)()) {
		cfg.dumpers.push_back(dumper);
	}

	void InstallErrorUI(void* hmod, int showCmd) {
		cfg.hmod = hmod;
		cfg.showCmd = showCmd;
	}

	void EnableHeavyDump() {
		cfg.heavyDump = true;
	}

	void DumpVTable(void* object, size_t size) {
		uintptr_t vtable{};
		if (!object || !memory::ReadMemorySafe(object, &vtable, sizeof(vtable))) {
			LOG_ERROR("Can't read vtable location");
			return;
		}
		

		void* buff[0x100]{};

		size_t offset{};
		while (size) {
			size_t r = std::min(ACTS_ARRAYSIZE(buff), size);

			if (!memory::ReadMemorySafe(reinterpret_cast<void*>(vtable), &buff, sizeof(buff[0]) * r)) {
				LOG_ERROR("Can't read vtable offset {}", offset);
				return;
			}

			uintptr_t relativeLocation;
			const char* moduleName;
			for (size_t i = 0; i < r; i++) {
				if (GetLocInfo(buff[i], relativeLocation, moduleName)) {
					LOG_DEBUG("VTABLE {} {} {:x} ({})", offset, moduleName, relativeLocation, buff[i]);
				}
				else {
					LOG_DEBUG("VTABLE {} {}", offset, buff[i]);
				}
				offset++;
			}

			size -= r;
			vtable += r * sizeof(buff[0]);
		}
	}
	void DevSetContinue(bool cont) {
		cfg.cont = cont;
	}
}