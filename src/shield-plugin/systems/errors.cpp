#include <dll_includes.hpp>
#include <core/config.hpp>
#include <hook/library.hpp>
#include <hook/error.hpp>
#include <rapidcsv.h>

namespace {
	std::unordered_map<uint32_t, std::string> errorCodes{};

	const char* GetErrorMessage(uint32_t code) {
		auto it{ errorCodes.find(code) };
		if (it != errorCodes.end()) return it->second.data();
		return nullptr;
	}

	hook::library::Detour Sys_ErrorDetour;
	hook::library::Detour Com_ErrorDetour;

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

	void PostInit(uint64_t uid) {
		Sys_ErrorDetour.Create(0x3D36CC0_a, Sys_ErrorStub);
		Com_ErrorDetour.Create(0x288B110_a, Com_ErrorStub);

		
		std::string errorFile{ core::config::GetString("logger.errorfile") };
		std::filesystem::path errorPath;
		if (errorFile.empty()) {
			errorPath = utils::GetProgDir() / "acts" / "errors.tsv";
		}
		else {
			errorPath = errorFile;
		}

		ReadErrorFile(errorPath);
	}

	REGISTER_SYSTEM(errors, nullptr, PostInit);
}