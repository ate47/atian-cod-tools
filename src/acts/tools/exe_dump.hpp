#pragma once

namespace tool::exe_dump {
	struct DumpProcessOpt {
		bool dumpHeader{};
		// rebuild the IAT table when mixed by AC
		bool rebuildIAT{};
		// create IAT table elements by scanning the values in the loaded data when mixed by AC
		bool searchIAT{};
	};
	/*
	 * Dump a process
	 * @param proc process
	 * @param out output path
	 * @param opt optional params
	 */
	void DumpProcess(Process& proc, const std::filesystem::path& out, DumpProcessOpt* opt = nullptr);

	/*
	 * Dump an exe by creating a process
	 * @param in exe path
	 * @param out out path
	 * @param opt optional params
	 */
	void DumpProcessExe(const std::filesystem::path& in, const std::filesystem::path& out, DumpProcessOpt* opt = nullptr);
}