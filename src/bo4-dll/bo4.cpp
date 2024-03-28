#include <dll_includes.hpp>
#include <offsets.hpp>

t8internal::scrVmPub* bo4::scrVmPub = reinterpret_cast<t8internal::scrVmPub*>(Relativise(offset::scrVmPub));
t8internal::scrVarPub* bo4::scrVarPub = reinterpret_cast<t8internal::scrVarPub*>(Relativise(offset::gScrVarPub));
bo4::ObjFileInfoTable* bo4::objFileInfo = reinterpret_cast<bo4::ObjFileInfoTable*>(Relativise(offset::gObjFileInfo));
int* bo4::objFileInfoCount = reinterpret_cast<int*>(Relativise(offset::gObjFileInfoCount));

void bo4::ScrVm_Error(scriptinstance::ScriptInstance inst, const char* format, bool terminal, ...) {
	static char errorMessage[2][0x400];
	static char emptyStr[1] = { 0 };
	va_list va;
	va_start(va, terminal);

	auto& msg = errorMessage[inst];

	vsprintf_s(msg, format, va);

	va_end(va);

	scrVarPub[inst].error_message = msg;

	// call internal
	Internal_ScrVm_Error(custom_gsc_func::custom_error_id, inst, emptyStr, terminal);
}

bool bo4::FindGSCFuncLocation(byte* location, scriptinstance::ScriptInstance& inst, GSCOBJ*& obj, GSCExport*& exp, uint32_t& rloc) {
	if (!location) {
		return false;
	}
	auto pos = reinterpret_cast<uint64_t>(location);
	for (size_t i = 0; i < scriptinstance::SI_COUNT; i++) {
		for (size_t c = 0; c < objFileInfoCount[i]; c++) {
			const auto& info = objFileInfo[i][c];

			if (!info.activeVersion) {
				LOG_WARNING("Found NULL script in objFileInfo at index {}:{}", i, c);
				continue;
			}

			auto start = reinterpret_cast<uint64_t>(info.activeVersion);
			auto end = start + info.activeVersion->script_size;

			if (pos < start || pos >= end) {
				continue; // not inside this script
			}

			obj = info.activeVersion;


			auto* exports = reinterpret_cast<GSCExport*>(obj->magic + obj->export_table_offset);
			auto exports_count = obj->exports_count;

			uint32_t bestRLoc = 0;
			for (size_t e = 0; e < exports_count; e++) {
				auto estart = start + exports[e].address;

				if (estart > pos) {
					continue; // this export is after the asked location
				}

				if (bestRLoc > exports[e].address) {
					continue; // the rloc is better for the previous export
				}

				// set best
				bestRLoc = exports[e].address;
				rloc = (uint32_t)(pos - estart);
				obj = info.activeVersion;
				exp = &exports[e];
			}

			if (bestRLoc) {
				inst = (scriptinstance::ScriptInstance)i;
				return true;
			}
		}
	}

	return false;
}

