#pragma once


namespace hook::error {
	/*
	 * Get location info
	 * @param location
	 * @param relativeLocation out relative location
	 * @param moduleName out module name
	 * @return if the location is inside a module
	 */
	bool GetLocInfo(const void* location, uintptr_t& relativeLocation, const char*& moduleName);
	/*
	 * Install an error hook
	 * @param clearSetFunction remove the set function so nothing else can set it
	 */
	void InstallErrorHooks(bool clearSetFunction = false);
	/*
	 * Display UI error when an error is generated, HMODULE
	 * @param hmod module
	 * @param showCmd show cmd param
	 */
	void InstallErrorUI(HMODULE hmod, int showCmd);
	/*
	 * Print more error information inside the error hook
	 */
	void EnableHeavyDump();

	/*
	 * Dump stack trace with a level
	 * @param level log level
	 * @param location start location
	 */
	void DumpStackTraceFrom(alogs::loglevel level = alogs::LVL_ERROR, const void* location = nullptr);
	/*
	 * Dump a vtable in the logs
	 * @param object object to dump
	 * @param size size
	 */
	void DumpVTable(void* object, size_t size);

	void DevSetContinue(bool cont);
}