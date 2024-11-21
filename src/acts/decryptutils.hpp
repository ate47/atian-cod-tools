#pragma once

namespace acts::decryptutils {
	/*
	 * Decrypt a string using the configured implementation, return str if no implementation was loaded
	 * @param str string to decrypt
	 * @return decrypted string
	 */
	char* DecryptString(char* str);
	/*
	 * Load a decryption implementation from an executable
	 * @param exec the executable
	 */
	bool LoadDecrypt(const std::filesystem::path& exec);
}