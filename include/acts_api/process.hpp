#pragma once
#include "api.hpp"
/*
 * Atian Tools Process API
 */

namespace acts::api {

	// DumpProcessExe options
	struct DumpProcessOption {
		// rebuild the IAT table when mixed by AC
		bool rebuildIAT{};
	};

	struct GameDumpInformation {
		// dump id
		const char* id;
		// exe to dump
		const char* exe;
		// dll(s) to dump, \0 separated
		const char* ddls;
		// needs rebuild IAT
		bool rebuildIAT{};
	};

	class ActsAPIProcess {
	public:
		/*
		 * Dump a process executable file
		 * @param in input file
		 * @param out output file (if nullptr, deps/[in]_dump.exe will be used)
		 * @param opt dump options
		 * @return success or failure, ActsGetAPILastMessage is set to the result status
		 */
		virtual ActsStatus DumpProcessExe(const char* in, const char* out = nullptr, DumpProcessOption* opt = nullptr) = 0;
		/*
		 * Dump a process executable
		 * @param pid process ID
		 * @param out output file (if nullptr, deps/[proc name]_dump.exe will be used)
		 * @param opt dump options
		 * @return success or failure, ActsGetAPILastMessage is set to the result status
		 */
		virtual ActsStatus DumpProcess(uint32_t pid, const char* out = nullptr, DumpProcessOption* opt = nullptr) = 0;
		/*
		 * @return Registered game dumps
		 */
		virtual GameDumpInformation* GetGameDumps() = 0;
		/*
		 * @return Registered game dumps count
		 */
		virtual size_t GetGameDumpsCount() = 0;
		/*
		 * Dump a process executable
		 * @param game game to dump
		 * @param root root directory of the game to dump
		 * @param out output file (if nullptr, deps/[proc name]_dump.exe will be used)
		 * @return success or failure, ActsGetAPILastMessage is set to the result status
		 */
		virtual ActsStatus DumpGame(GameDumpInformation& game, const char* root, const char* out = nullptr) = 0;
	};

}

// @return process api
ACTS_COMMON_API acts::api::ActsAPIProcess& ActsAPIProcess();