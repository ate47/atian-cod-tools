#ifndef __ACTS_API_PROCESS_H__
#define __ACTS_API_PROCESS_H__
#include "api.h"
/*
 * Atian Tools Process API
 */

// ActsAPIProcess_DumpProcessExe options
struct ActsAPIProcess_DumpProcessOption {
	// rebuild the IAT table when mixed by AC
	bool rebuildIAT;
};

// ActsAPIProcess_DumpGame options
struct ActsAPIProcess_GameDumpInformation {
	// dump id
	const char* id;
	// exe to dump
	const char* exe;
	// dll(s) to dump, \0 separated
	const char* ddls;
	// needs rebuild IAT
	bool rebuildIAT;
};

// ActsAPIProcess_DumpGame options list
struct ActsAPIProcess_GameDumpInformationList {
	// values
	ActsAPIProcess_GameDumpInformation* values;
	// count
	size_t count;
};

/*
 * Dump a process executable file
 * @param in input file
 * @param out output file (if nullptr, deps/[in]_dump.exe will be used)
 * @param opt dump options
 * @return success or failure, ActsGetAPILastMessage is set to the result status
 */
ACTS_COMMON_API ActsStatus ActsAPIProcess_DumpProcessExe(const char* in, const char* out, ActsAPIProcess_DumpProcessOption* opt);
/*
 * Dump a process executable
 * @param pid process ID
 * @param out output file (if nullptr, deps/[proc name]_dump.exe will be used)
 * @param opt dump options
 * @return success or failure, ActsGetAPILastMessage is set to the result status
 */
ACTS_COMMON_API ActsStatus ActsAPIProcess_DumpProcess(uint32_t pid, const char* out, ActsAPIProcess_DumpProcessOption* opt);
/*
 * @return Registered game dumps
 */
ACTS_COMMON_API ActsAPIProcess_GameDumpInformationList* ActsAPIProcess_GetGameDumpsList();
/*
 * Dump a process executable
 * @param game game to dump
 * @param root root directory of the game to dump
 * @param out output file (if nullptr, deps/[proc name]_dump.exe will be used)
 * @return success or failure, ActsGetAPILastMessage is set to the result status
 */
ACTS_COMMON_API ActsStatus ActsAPIProcess_DumpGame(ActsAPIProcess_GameDumpInformation* game, const char* root, const char* out);

#endif // __ACTS_API_PROCESS_H__