#pragma once
#include <hook/process.hpp>

namespace hook::memory {
	/*
	 * Is a distance from 2 pointers ok in 32 bits
	 * @param from pointer 1
	 * @param to pointer 2
	 * @return if the distance is fine for 32 bits jmp
	 */
	bool Int32Distance(void* from, void* to);

	/*
	 * Allocate a memory page near a location
	 * @param location location
	 * @return allocated page or null
	 */
	void* AllocateNearPage(void* location);

	/*
	 * Allocate memory near a location, can't be above a page size
	 * @param location location
	 * @param size size
	 * @return allocated page or null
	 */
	void* AllocateNear(void* location, size_t size);

	/*
	 * Create a jump at a location
	 * @param location origin
	 * @param to location to jump
	 * @param r64 use 64 bits far jmp
	 */
	void RedirectJmp(void* location, void* to, bool r64 = false);

	/*
	 * Nop codes
	 * @param location location to nop
	 * @param size count to nop
	 */
	void Nop(void* location, size_t size = 1);

	/*
	 * Int3 codes
	 * @param location location to int
	 * @param size count to int
	 */
	void Int3(void* location, size_t size = 1);

	/*
	 * Write memory in log
	 * @param location start
	 * @param size size
	 */
	void DumpMemory(void* location, size_t size);

	/*
	 * Is a location inside a near container
	 * @param location location
	 * @return true if inside a container
	 */
	bool IsInsideNearContainer(const void* location);

	/*
	 * Try to read a memory into a buffer at a location
	 * @param location location
	 * @param buffer buffer
	 * @param size size to read
	 * @return if the read was done
	 */
	bool ReadMemorySafe(void* location, void* buffer, size_t size);

	/*
	 * Try to read a relative memory into a buffer at a location
	 * @param location location
	 * @param buffer buffer
	 * @param size size to read
	 * @return if the read was done
	 */
	bool ReadRelativeMemorySafe(void* location, void* buffer, size_t size);

	/*
	 * Try to get a relative location
	 * @param location location
	 * @return relative offset or null in case of error
	 */
	void* GetRelativeMemorySafe(void* location);

#ifdef ASMJIT_STATIC
	/*
	 * Assemble a function and return it
	 * @param builder builder function
	 * @return assembled function
	 */
	void* Assembler(std::function<void(process::AssemblerExp& a)> builder);

#endif
}