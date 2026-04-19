#include <includes_shared.hpp>
#include <hook/memory.hpp>
#include <hook/process.hpp>
#include <utils/utils.hpp>

namespace hook::memory {
	bool ReadMemorySafe(void* location, void* buffer, size_t size) {
		return platform::ReadMemorySafe(location, buffer, size);
	}

	void* GetRelativeMemorySafe(void* location) {
		return platform::GetRelativeMemorySafe(location);
	}

	void* AllocateNearPage(void* location) {
		return platform::AllocateNearPage(location);
	}

	void* AllocateNear(void* location, size_t size) {
		return platform::AllocateNear(location, size);
	}

	bool Int32Distance(void* from, void* to) {
		constexpr size_t uint32dist = 1ull << 31;
		return ((uintptr_t)from - (uintptr_t)to) < uint32dist;
	}

	bool IsInsideNearContainer(const void* location) {
		return platform::IsInsideNearContainer(location);
	}

	bool ReadRelativeMemorySafe(void* location, void* buffer, size_t size) {
		location = GetRelativeMemorySafe(location);
		return location && ReadMemorySafe(location, buffer, size);
	}

	void RedirectJmp(void* location, void* to, bool r64) {
		// https://www.felixcloutier.com/x86/jmp
		bool r32 = Int32Distance(location, to);
		if (!r32 && !r64) {
			// use long jump
			void* nearPtr = AllocateNear(location, 13);
			r64 = true;

			if (!nearPtr) {
				throw std::exception(utils::va(actssec("Can't jump from %p to %p: too far (0x%llx)"), location, to, (byte*)location - (byte*)to));
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
	void ReturnVal(void* location, bool val) {
		byte hook[]{ 0, 0, 0xC3 }; // ? ?, ret
		if (val) {
			hook[0] = 0xB0;
			hook[1] = 0x01; // mov    al,0x1
		}
		else {
			hook[0] = 0x30;
			hook[1] = 0xC0; // xor    al,al
		}
		process::WriteMemSafe(location, hook, sizeof(hook));
	}

	void Nulled(void* location) {
		byte hook{ 0xC3 }; // ret
		process::WriteMemSafe(location, &hook, sizeof(hook));
	}

	void DumpMemory(void* location, size_t size) {
		platform::DumpMemory(location, size);
	}

#ifdef ASMJIT_STATIC
	void* Assembler(std::function<void(process::AssemblerExp& a)> build) {
		auto& jr = process::GetJitRuntime();

		asmjit::CodeHolder holder;
		holder.init(jr.environment());

		process::AssemblerExp asml{ &holder };
		build(asml);

		void* out{};
		auto err = jr.add(&out, &holder);

		if (err || !out) {
			throw std::runtime_error(utils::va(actssec("Error when assembling %lld"), err));
		}
		return out;
	}
#endif
}