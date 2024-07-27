#include "includes_shared.hpp"
#include "memory.hpp"
#include "process.hpp"
#include "utils.hpp"

namespace hook::memory {
	namespace {
		class NearContainer {
			void* container;
			size_t used{};
			size_t capacity{};
		public:
			NearContainer(void* container, size_t capacity) : container(container), capacity(capacity) {
			}

			constexpr bool CanAllocate(size_t len) const {
				return used + len <= capacity;
			}

			void* Allocate(size_t size) {
				if (!CanAllocate(size)) {
					throw std::exception(utils::va("Can't allocate into this container 0x%llx + 0x%llx > 0x%llx", used, size, capacity));
				}

				void* ptr = (byte*)container + used;

				used += size;

				return ptr;
			}

			bool IsInside(const void* location) const {
				return location >= container && location <= (byte*)container + capacity;
			}

			bool IsCloseEnough(void* location, size_t len = 1) const {
				return Int32Distance((byte*)location + used, container) && Int32Distance((byte*)location + used + len, container);
			}

			bool CanAllocateClose(void* location, size_t len) const {
				return CanAllocate(len) && Int32Distance((byte*)location + used, container) && Int32Distance((byte*)location + used + len, container);
			}
		};

		SYSTEM_INFO& GetSysInfo() {
			static SYSTEM_INFO sysInfo{};
			static std::once_flag pageSizeFlag{};
			std::call_once(pageSizeFlag, [] {
				GetSystemInfo(&sysInfo);
			});
			return sysInfo;
		}
	}

	bool Int32Distance(void* from, void* to) {
		constexpr size_t uint32dist = 1ull << 31;
		return ((uintptr_t)from - (uintptr_t)to) < uint32dist;
	}

	void* AllocateNearPage(void* location) {
		SYSTEM_INFO& sysInfo = GetSysInfo();
		uint64_t pageSize = sysInfo.dwPageSize;
		uint64_t startAddr = (uint64_t(location) & ~(pageSize - 1));
		uint64_t minAddr = std::min(startAddr - 0x7FFFFF00, (uint64_t)sysInfo.lpMinimumApplicationAddress);
		uint64_t maxAddr = std::max(startAddr + 0x7FFFFF00, (uint64_t)sysInfo.lpMaximumApplicationAddress);

		uint64_t startPage = (startAddr - (startAddr % pageSize));

		uint64_t pageOffset = 1;
		while (true) {
			uint64_t byteOffset = pageOffset * pageSize;
			uint64_t highAddr = startPage + byteOffset;
			uint64_t lowAddr = (startPage > byteOffset) ? startPage - byteOffset : 0;

			bool needsExit = highAddr > maxAddr && lowAddr < minAddr;

			if (highAddr < maxAddr) {
				void* outAddr = VirtualAlloc((void*)highAddr, pageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
				if (outAddr) {
					return outAddr;
				}
			}

			if (lowAddr > minAddr) {
				void* outAddr = VirtualAlloc((void*)lowAddr, pageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
				if (outAddr != nullptr) {
					return outAddr;
				}
			}

			pageOffset++;

			if (needsExit) {
				throw std::exception(utils::va("Can't allocate near page %p", location));
			}
		}
	}

	namespace {
		std::vector<NearContainer>& NearPool() {
			static std::vector<NearContainer> nearPool{};
			return nearPool;
		}
	}

	void* AllocateNear(void* location, size_t size) {
		std::vector<NearContainer>& nearPool = NearPool();
		for (auto& container : nearPool) {
			if (!container.CanAllocateClose(location, size)) {
				continue;
			}

			return container.Allocate(size);
		}
		DWORD pageSize = GetSysInfo().dwPageSize;
		if (pageSize < size) {
			throw std::exception(utils::va("Can't allocate near %p of size 0x%llx: too big (0x%llx max)", location, size, GetSysInfo().dwPageSize));
		}

		void* pool = AllocateNearPage(location);
		auto& container = nearPool.emplace_back(pool, pageSize);

		return container.Allocate(size);
	}

	bool IsInsideNearContainer(const void* location) {
		for (const auto& container : NearPool()) {
			if (container.IsInside(location)) {
				return true;
			}
		}
		return false;
	}

	bool ReadMemorySafe(void* location, void* buffer, size_t size) {
		return ReadProcessMemory(GetCurrentProcess(), location, buffer, size, NULL);
	}

	void RedirectJmp(void* location, void* to, bool r64) {
		// https://www.felixcloutier.com/x86/jmp
		bool r32 = Int32Distance(location, to);
		if (!r32 && !r64) {
			// use long jump
			void* nearPtr = AllocateNear(location, 13);
			r64 = true;

			if (!nearPtr) {
				throw std::exception(utils::va("Can't jump from %p to %p: too far (0x%llx)", location, to, (byte*)location - (byte*)to));
			}
			RedirectJmp(location, nearPtr);
			location = nearPtr;
			r32 = Int32Distance(location, to);
		}

		if (!r32 && r64) {
			byte jmp[]{
				0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov r10, addr
				0x41, 0xFF, 0xE2 // jmp r10
			};

			*reinterpret_cast<void**>(&jmp[2]) = to;

			process::WriteMemSafe(location, jmp, sizeof(jmp));
		}
		else {
			byte jmp[]{
				0xE9, 0x00, 0x00, 0x00, 0x00 // jmp rel32
			};

			*reinterpret_cast<int32_t*>(&jmp[1]) = (int32_t)((byte*)to - (byte*)location - sizeof(jmp));

			process::WriteMemSafe(location, jmp, sizeof(jmp));
		}


	}

	void Nop(void* location, size_t size) {
		byte tmp[0x20];
		byte* loc = (byte*)location;

		while (size) {
			size_t w = std::min(sizeof(tmp), size);
			memset(tmp, 0x90, w);
			process::WriteMemSafe(loc, tmp, w);
			size -= w;
			loc += w;
		}
	}

	void Int3(void* location, size_t size) {
		byte tmp[0x20];
		byte* loc = (byte*)location;

		while (size) {
			size_t w = std::min(sizeof(tmp), size);
			memset(tmp, 0xCC, w);
			process::WriteMemSafe(loc, tmp, w);
			size -= w;
			loc += w;
		}
	}

	void DumpMemory(void* location, size_t size) {
		byte* b = (byte*)location;

		DWORD old{};
		VirtualProtect(location, size, PAGE_EXECUTE_READWRITE, &old);


		for (size_t i = 0; i < size;) {
			std::stringstream ss{};
			for (size_t j = 0; j < 8 && i < size; j++, i++) {
				ss << std::setw(2) << std::setfill('0') << std::hex << (int)b[i] << " ";
			}
			LOG_DEBUG("{}", ss.str());
		}

		VirtualProtect(location, size, old, &old);
	}

	void* Assembler(std::function<void(process::AssemblerExp& a)> build) {
		auto& jr = process::GetJitRuntime();

		asmjit::CodeHolder holder;
		holder.init(jr.environment());

		process::AssemblerExp asml{ &holder };
		build(asml);

		void* out{};
		auto err = jr.add(&out, &holder);

		if (err || !out) {
			throw std::runtime_error(utils::va("Error when assembling %lld", err));
		}
		return out;
	}
}