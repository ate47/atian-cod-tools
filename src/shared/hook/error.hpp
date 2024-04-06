#pragma once


namespace hook::error {
	/*
	 * Install an error hook
	 * @param clearSetFunction remove the set function so nothing else can set it
	 */
	void InstallErrorHooks(bool clearSetFunction = false);
	/*
	 * Print more error information inside the error hook
	 */
	void EnableHeavyDump();
}