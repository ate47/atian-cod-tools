#pragma once
#include <includes_shared.hpp>

/*
 * Utilities
 */
namespace utils {
	/*
	 * Read a file inside a buffer
	 * @param path file path
	 * @param buffer buffer to write the size, NULL to allocate it (with std::malloc)
	 * @param size size of the buffer
	 * @param nullTerminate add a 0 byte at the end of the file
	 * @return if the file was read
	 */
	bool ReadFileNotAlign(const std::filesystem::path& path, LPVOID& buffer, size_t& size, bool nullTerminate = false);
	/*
	 * Read a file inside a buffer and align the result at 128 bits
	 * @param path file path
	 * @param buffer buffer to write the size, NULL to allocate it (with std::malloc)
	 * @param bufferAligned buffer location aligned
	 * @param size size of the buffer
	 * @param sizeAligned size of the aligned buffer
	 * @return if the file was read
	 */
	bool ReadFileAlign(const std::filesystem::path& path, LPVOID& buffer, LPVOID& bufferAligned, size_t& size, size_t& sizeAligned);
	/*
	 * write a buffer into a file
	 * @param path file path
	 * @param ptr buffer location
	 * @param size buffer size
	 * @return if the file was written
	 */
	bool WriteFile(const std::filesystem::path& path, LPCVOID ptr, size_t size);

	/*
	 * Align a pointer
	 * @param Type type to align
	 * @param ptr pointer to align
	 * @return aligned ptr value
	 */
	template<typename Type>
	inline BYTE* Aligned(BYTE* ptr) {
		return reinterpret_cast<BYTE*>((reinterpret_cast<uintptr_t>(ptr) + (sizeof(Type) - 1)) & ~(sizeof(Type) - 1));
	}
	/*
	 * Align a pointer
	 * @param Type type to align
	 * @param TypeIn pointer type
	 * @param ptr pointer to align
	 * @return aligned ptr value
	 */
	template<typename Type, typename TypeIn = uintptr_t>
	inline TypeIn Aligned(TypeIn ptr) {
		return (ptr + (sizeof(Type) - 1)) & ~(sizeof(Type) - 1);
	}

	/*
	 * Align a vector buffer with its size
	 * @param Type type to align
	 * @param data vector buffer to align
	 */
	template<typename Type>
	inline void Aligned(std::vector<BYTE>& data) {
		size_t pre = data.size();
		size_t post = Aligned<Type, size_t>(pre);

		// add padding bytes
		for (size_t i = pre; i < post; i++) {
			data.push_back(0);
		}
	}
	/*
	 * Write a value into a vector buffer
	 * @param Type value type
	 * @param data buffer
	 * @param val value to write
	 */
	template<typename Type>
	inline void WriteValue(std::vector<BYTE>& data, Type val) {
		BYTE* valLoc = reinterpret_cast<BYTE*>(&val);
		data.insert(data.end(), valLoc, valLoc + sizeof(val));
	}
	/*
	 * Write a padding into a stream
	 * @param out stream
	 * @param padding padding size
	 * @return stream
	 */
	std::ostream& Padding(std::ostream& out, int padding);
	/*
	 * Cat 2 UINT32 into a UINT64
	 * @param name_space high value
	 * @param local low value
	 * @return cat value
	 */
	UINT64 CatLocated(UINT32 name_space, UINT32 local);
	/*
	 * Uncat 2 UINT32 into a UINT64
	 * @param located cat value
	 * @return high value and low value
	 */
	std::pair<UINT32, UINT32> UnCatLocated(UINT64 located);
}