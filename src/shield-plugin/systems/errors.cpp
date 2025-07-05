#include <dll_includes.hpp>
#include <data/refs.hpp>
#include <core/config.hpp>
#include <hook/library.hpp>
#include <hook/error.hpp>
#include <rapidcsv.h>
#include <systems/errors.hpp>

namespace systems::errors {
	namespace {
		std::unordered_map<uint32_t, std::string> errorCodes{};

		const char* GetErrorMessage(uint32_t code) {
			auto it{ errorCodes.find(code) };
			if (it != errorCodes.end()) return it->second.data();
			return nullptr;
		}

		hook::library::Detour Sys_ErrorDetour;
		hook::library::Detour Com_ErrorDetour;
		hook::library::Detour ScrVm_Error_Detour;
		hook::library::Detour ScrVm_LogCompilerError_Detour;

		void Com_ErrorStub(uint32_t code) {
			const char* trans{ GetErrorMessage(code) };
			LOG_ERROR("Com_Error({}) {}", code, trans ? trans : "");
			LOG_ERROR("--- Stack ---");
			hook::error::DumpStackTraceFrom();
			LOG_ERROR("-------------");

			Com_ErrorDetour.Call(code);
		}


		void Sys_ErrorStub(uint32_t code, const char* msg) {
			if (code != 546354288 && code != 2840917045) { // syserror in another thread, don't care
				LOG_ERROR("Sys_Error({}{})", code, msg ? msg : "");
				const char* trans{ GetErrorMessage(code) };
				if (trans) {
					LOG_ERROR("Translate: {}", trans);
				}
				LOG_ERROR("--- Stack ---");
				hook::error::DumpStackTraceFrom();
				LOG_ERROR("-------------");
			}
			Sys_ErrorDetour.Call(code, msg);
		}

		void ScrVm_Error_Stub(uint32_t code, bo4::scriptInstance_t inst, char* unused, bool terminal) {
			static char buffer[bo4::scriptInstance_t::SI_COUNT][0x200]{};
			switch (code) {
			case 2737681163: { // Assert(val, msg) with message error
				const char* msg = bo4::ScrVm_GetString(inst, 1);
				sprintf_s(buffer[inst], "assert fail: %s", msg);
				bo4::scrVarPub[inst].error_message = buffer[inst];
			}
			break;
			case 1385570291: { // AssertMsg(msg)
				const char* msg = bo4::ScrVm_GetString(inst, 0);
				sprintf_s(buffer[inst], "assert fail: %s", msg);
				bo4::scrVarPub[inst].error_message = buffer[inst];
			}
			break;
			case 2532286589: { // ErrorMsg(msg)
				const char* msg = bo4::ScrVm_GetString(inst, 0);
				sprintf_s(buffer[inst], "error: %s", msg);
				bo4::scrVarPub[inst].error_message = buffer[inst];
			}
			break;
			default:
				// put custom message for our id
				if (code == CUSTOM_ERROR_ID) {
					bo4::scrVarPub[inst].error_message = unused;
				}
				else {
					bo4::scrVarPub[inst].error_message = GetErrorMessage(code);
				}
				break;
			}

			ScrVm_Error_Detour.Call(code, inst, unused, terminal);
		}

		void ScrVm_LogCompilerError_Stub(const char* fmt, ...) {
			va_list va;
			va_start(va, fmt);
			char buffer[0x800];

			int e = vsprintf_s(buffer, fmt, va);
			va_end(va);

			if (e > 0 && buffer[e - 1] == '\n') {
				buffer[e - 1] = 0; // remove end new line
			}

			LOG_ERROR("[LogCompilerError] {}", buffer);
		}

		void ReadErrorFile(const std::filesystem::path& path) {
			std::string buffer;
			if (!utils::ReadFile(path, buffer)) {
				if (!std::filesystem::exists(path)) {
					utils::WriteFile(path, "");
				}
				return;
			}

			rapidcsv::Document doc{};

			std::stringstream stream{ buffer };

			doc.Load(stream, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams('\t'));

			if (doc.GetColumnCount() < 3) {
				LOG_WARNING("Can't read error tsv {}: invalid file", path.string());
				return;
			}

			for (size_t i = 0; i < doc.GetRowCount(); i++) {
				uint32_t code = doc.GetCell<uint32_t>(1, i);
				errorCodes[code] = doc.GetCell<std::string>(2, i);
			}

			if (!errorCodes.empty()) LOG_INFO("{} error code(s) loaded", errorCodes.size());
		}

		void Init(uint64_t uid) {
			std::string errorFile{ core::config::GetString("logger.errorfile") };
			std::filesystem::path errorPath;
			if (errorFile.empty()) {
				errorPath = utils::GetProgDir() / "project-bo4" / "acts" / "errors.tsv";
			}
			else {
				errorPath = errorFile;
			}

			ReadErrorFile(errorPath);
		}

		void PostInit(uint64_t uid) {
			Sys_ErrorDetour.Create(0x3D36CC0_a, Sys_ErrorStub);
			Com_ErrorDetour.Create(0x288B110_a, Com_ErrorStub);
			ScrVm_Error_Detour.Create(0x2770330_a, ScrVm_Error_Stub);
			ScrVm_LogCompilerError_Detour.Create(0x2890470_a, ScrVm_LogCompilerError_Stub);
		}

		REGISTER_SYSTEM(errors, Init, PostInit);
	}

	void ScrVm_Error(bo4::scriptInstance_t inst, const char* msg, bool terminal, ...) {
		static char buffer[bo4::scriptInstance_t::SI_COUNT][0x800];

		va_list va;
		va_start(va, terminal);
		vsprintf_s(buffer[inst], msg, va);
		va_end(va);

		bo4::ScrVm_Error(CUSTOM_ERROR_ID, inst, buffer[inst], terminal);
	}
}