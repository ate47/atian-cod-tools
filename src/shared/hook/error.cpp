#include <includes_shared.hpp>
#include "error.hpp"
#include "process.hpp"
#include "library.hpp"
#include "utils.hpp"
#include "memory.hpp"


namespace hook::error {
	bool GetLocInfo(const void* location, uintptr_t& relativeLocation, const char*& moduleName) {
		HMODULE hmod = library::GetLibraryInfo(location);

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
	namespace {
		struct ErrorConfig {
			DWORD mainThread{};
			bool heavyDump{};

			HMODULE hmod{};
			int showCmd{};
		};

		ErrorConfig cfg{};

		const char* ExceptionName(DWORD ex) {
			switch (ex) {
			case EXCEPTION_ACCESS_VIOLATION: return "ACCESS_VIOLATION";
			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "ARRAY_BOUNDS_EXCEEDED";
			case EXCEPTION_BREAKPOINT: return "BREAKPOINT";
			case EXCEPTION_DATATYPE_MISALIGNMENT: return "DATATYPE_MISALIGNMENT";
			case EXCEPTION_FLT_DENORMAL_OPERAND: return "FLT_DENORMAL_OPERAND";
			case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "FLT_DIVIDE_BY_ZERO";
			case EXCEPTION_FLT_INEXACT_RESULT: return "FLT_INEXACT_RESULT";
			case EXCEPTION_FLT_INVALID_OPERATION: return "FLT_INVALID_OPERATION";
			case EXCEPTION_FLT_OVERFLOW: return "FLT_OVERFLOW";
			case EXCEPTION_FLT_STACK_CHECK: return "FLT_STACK_CHECK";
			case EXCEPTION_FLT_UNDERFLOW: return "FLT_UNDERFLOW";
			case EXCEPTION_ILLEGAL_INSTRUCTION: return "ILLEGAL_INSTRUCTION";
			case EXCEPTION_IN_PAGE_ERROR: return "IN_PAGE_ERROR";
			case EXCEPTION_INT_DIVIDE_BY_ZERO: return "INT_DIVIDE_BY_ZERO";
			case EXCEPTION_INT_OVERFLOW: return "INT_OVERFLOW";
			case EXCEPTION_INVALID_DISPOSITION: return "INVALID_DISPOSITION";
			case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "NONCONTINUABLE_EXCEPTION";
			case EXCEPTION_PRIV_INSTRUCTION: return "PRIV_INSTRUCTION";
			case EXCEPTION_SINGLE_STEP: return "SINGLE_STEP";
			case EXCEPTION_STACK_OVERFLOW: return "STACK_OVERFLOW";
			default: return "UNKNOWN";
			}
		}

		LONG ACTSUnhandledExceptionFilter(_EXCEPTION_POINTERS* ExceptionInfo) {
			LOG_ERROR("--- EXCEPTION DETECTED ---");
			uintptr_t relativeLocation;
			const char* moduleName;

			if (!GetLocInfo(ExceptionInfo->ExceptionRecord->ExceptionAddress, relativeLocation, moduleName)) {
				LOG_ERROR("Error code: 0x{:x} at {}", ExceptionInfo->ExceptionRecord->ExceptionCode, ExceptionInfo->ExceptionRecord->ExceptionAddress);
			}
			else {
				LOG_ERROR("Error code: 0x{:x} at {} ({} 0x{:x})", ExceptionInfo->ExceptionRecord->ExceptionCode, ExceptionInfo->ExceptionRecord->ExceptionAddress, moduleName, relativeLocation);
			}
			LOG_ERROR("Error type: {}", ExceptionName(ExceptionInfo->ExceptionRecord->ExceptionCode));
			DWORD threadId = GetCurrentThreadId();
			LOG_ERROR("Thread: {} ({})", threadId, threadId == cfg.mainThread ? "Main" : "Other");

			// error info
			switch (ExceptionInfo->ExceptionRecord->ExceptionCode) {
			case EXCEPTION_ACCESS_VIOLATION: {
				LOG_ERROR("Error info: invalid {} at 0x{:x}", 
					ExceptionInfo->ExceptionRecord->ExceptionInformation[0] ? "read" : "write", ExceptionInfo->ExceptionRecord->ExceptionInformation[1]);
				break;
			}
			case EXCEPTION_IN_PAGE_ERROR: {
				LOG_ERROR("Error info: invalid {} at 0x{:x} status: 0x{:x}", 
					ExceptionInfo->ExceptionRecord->ExceptionInformation[0] ? "read" : "write", ExceptionInfo->ExceptionRecord->ExceptionInformation[1], ExceptionInfo->ExceptionRecord->ExceptionInformation[2]);
				break;
			}
			default: {
				if (ExceptionInfo->ExceptionRecord->NumberParameters) {
					std::stringstream info{};

					for (size_t i = 0; i < ExceptionInfo->ExceptionRecord->NumberParameters; i++) {
						info << " " << ExceptionInfo->ExceptionRecord->ExceptionInformation[i];
					}

					LOG_ERROR("Error info: {}", info.str());
				}
			}
				break;
			}

			if (cfg.heavyDump) {
				LOG_ERROR("Registers:");
				auto PrintRegister = [](const char* name, uint64_t val) {
					uintptr_t relativeLocation;
					const char* moduleName;

					std::stringstream ss{};

					ss << "- " << name << " . 0x" << std::hex << val;

					if (GetLocInfo(reinterpret_cast<void*>(val), relativeLocation, moduleName)) {
						ss << " | " << moduleName << " 0x" << std::hex << relativeLocation;
					}

					byte strBuff[0x100]{};

					if (memory::ReadMemorySafe(reinterpret_cast<void*>(val), strBuff, sizeof(strBuff))) {
						bool isValidStr{ true };
						size_t i = 0;
						while (i < sizeof(strBuff)) {
							byte c = strBuff[i];
							if (!c) {
								break;
							}
							if ((c < 0x20 && c != '\r' && c != '\n' && c != '\t') || c >= 0x7F) {
								isValidStr = false;
								break;
							}
							i++;
						}
						if (i == sizeof(strBuff)) {
							isValidStr = false;
						}

						if (isValidStr && *strBuff) {
							ss << " | \"" << strBuff << "\"";
						}
						else {
							ss << " ->";
							for (size_t j = 0; j < 8; j++) {
								ss << " " << std::hex << std::setfill('0') << std::setw(2) << (int)strBuff[j];
							}
						}
					}


					LOG_ERROR("{}", ss.str());
				};


				LOG_ERROR("DebugRegisters: 0:0x{:x} 1:0x{:x} 2:0x{:x} 3:0x{:x} 6:0x{:x} 7:0x{:x}",
					ExceptionInfo->ContextRecord->Dr0, ExceptionInfo->ContextRecord->Dr1,
					ExceptionInfo->ContextRecord->Dr2, ExceptionInfo->ContextRecord->Dr3,
					ExceptionInfo->ContextRecord->Dr6, ExceptionInfo->ContextRecord->Dr7
				);
				PrintRegister("rax", ExceptionInfo->ContextRecord->Rax);
				PrintRegister("rbx", ExceptionInfo->ContextRecord->Rbx);
				PrintRegister("rcx", ExceptionInfo->ContextRecord->Rcx);
				PrintRegister("rdx", ExceptionInfo->ContextRecord->Rdx);
				PrintRegister("rsp", ExceptionInfo->ContextRecord->Rsp);
				PrintRegister("rbp", ExceptionInfo->ContextRecord->Rbp);
				PrintRegister("rsi", ExceptionInfo->ContextRecord->Rsi);
				PrintRegister("rdi", ExceptionInfo->ContextRecord->Rdi);
				PrintRegister("r8 ", ExceptionInfo->ContextRecord->R8);
				PrintRegister("r9 ", ExceptionInfo->ContextRecord->R9);
				PrintRegister("r10", ExceptionInfo->ContextRecord->R10);
				PrintRegister("r11", ExceptionInfo->ContextRecord->R11);
				PrintRegister("r12", ExceptionInfo->ContextRecord->R12);
				PrintRegister("r13", ExceptionInfo->ContextRecord->R13);
				PrintRegister("r14", ExceptionInfo->ContextRecord->R14);
				PrintRegister("r15", ExceptionInfo->ContextRecord->R15);
				PrintRegister("rip", ExceptionInfo->ContextRecord->Rip);

				LOG_ERROR("Stack trace:");
				DumpStackTraceFrom(alogs::LVL_ERROR, ExceptionInfo->ExceptionRecord->ExceptionAddress);
			}

			if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) {
				return EXCEPTION_CONTINUE_EXECUTION;
			}
			return EXCEPTION_EXECUTE_HANDLER;
		}

		void WINAPI EmptySetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter) {
			if (alogs::getlevel() > alogs::LVL_TRACE) {
				return;
			}

			uintptr_t relativeLocation;
			const char* moduleName;

			if (!GetLocInfo(lpTopLevelExceptionFilter, relativeLocation, moduleName)) {
				LOG_TRACE("Tried to insert hook form {} 0x{} ({})", moduleName, relativeLocation, (PVOID)lpTopLevelExceptionFilter);
			}
			else {
				LOG_TRACE("Tried to insert hook form {}", (PVOID)lpTopLevelExceptionFilter);
			}
		}

	}

	void DumpStackTraceFrom(alogs::loglevel level, const void* location) {
		void* locs[50];
		WORD capture = RtlCaptureStackBackTrace(0, ARRAYSIZE(locs), locs, NULL);

		uintptr_t relativeLocation;
		const char* moduleName;
		size_t i{};
		if (location) {
			for (; i < capture; i++) {
				if (locs[i] == location) break; // skip until we reach our location
			}
		}
		if (i == capture) {
			i = 0;
		}
		for (; i < capture; i++) {
			if (GetLocInfo(locs[i], relativeLocation, moduleName)) {
				LOG_LVL(level, "- {} 0x{:x} ({})", moduleName, relativeLocation, locs[i]);
			}
			else {
				LOG_LVL(level, "- {}", locs[i]);
			}
		}
	}

	void InstallErrorHooks(bool clearSetFunction) {
		cfg.mainThread = GetCurrentThreadId();
		SetUnhandledExceptionFilter(ACTSUnhandledExceptionFilter);
		try {
			if (clearSetFunction) {
				// redirect the previous function so it is not used
				memory::RedirectJmp(SetUnhandledExceptionFilter, EmptySetUnhandledExceptionFilter);
			}
		}
		catch (std::exception& e) {
			LOG_ERROR("Can't install error hooks: {}", e.what());
		}
	}

	void InstallErrorUI(HMODULE hmod, int showCmd) {
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
			size_t r = std::min(ARRAYSIZE(buff), size);

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
}