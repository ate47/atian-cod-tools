#pragma once
#include <includes_shared.hpp>
#include "memapi.hpp"

namespace memapi {

#ifdef ASMJIT_STATIC
	// param+ = stack, last first pushed
	inline void CallExtFuncBuildStack(hook::process::AssemblerExp& asml) {
	}
	template<typename Arg0, typename... Args>
	inline void CallExtFuncBuildStack(hook::process::AssemblerExp& asml, float arg, Args... args) {
		using namespace asmjit::x86;

		CallExtFuncBuildStack(asml, args...);
		//asml.movsd(xmm0, arg);
		//asml.mov(rax, qword_ptr(rcx, idx));
		//asml.push(rax);
	}
	template<typename Arg0, typename... Args>
	inline void CallExtFuncBuildStack(hook::process::AssemblerExp& asml, double arg, Args... args) {
		using namespace asmjit::x86;

		CallExtFuncBuildStack(asml, args...);
		//asml.movsd(xmm0, arg);
		//asml.mov(rax, qword_ptr(rcx, idx));
		//asml.push(rax);
	}
	template<typename Arg0, typename... Args>
	inline void CallExtFuncBuildStack(hook::process::AssemblerExp& asml, Arg0 arg, Args... args) {
		using namespace asmjit::x86;

		CallExtFuncBuildStack(asml, args...);
		asml.mov(rax, arg);
		asml.push(rax);
	}

	// param 3 -> R9  | XMM3
	inline void CallExtFuncBuild3(hook::process::AssemblerExp& asml) {
	}
	template<typename... Args>
	inline void CallExtFuncBuild3(hook::process::AssemblerExp& asml, double arg, Args... args) {
		using namespace asmjit::x86;
		CallExtFuncBuildStack(asml, args...);
		//asml.movsd(xmm3, arg);
	}
	template<typename... Args>
	inline void CallExtFuncBuild3(hook::process::AssemblerExp& asml, float arg, Args... args) {
		using namespace asmjit::x86;
		CallExtFuncBuildStack(asml, args...);
		//asml.movss(xmm3, arg);
	}
	template<typename Arg0, typename... Args>
	inline void CallExtFuncBuild3(hook::process::AssemblerExp& asml, Arg0 arg, Args... args) {
		using namespace asmjit::x86;
		CallExtFuncBuildStack(asml, args...);
		asml.mov(r9, arg);
	}

	// param 2 -> R8  | XMM2
	inline void CallExtFuncBuild2(hook::process::AssemblerExp& asml) {
	}
	template<typename... Args>
	inline void CallExtFuncBuild2(hook::process::AssemblerExp& asml, double arg, Args... args) {
		using namespace asmjit::x86;
		CallExtFuncBuild3(asml, args...);
		//asml.movsd(xmm2, arg);
	}
	template<typename... Args>
	inline void CallExtFuncBuild2(hook::process::AssemblerExp& asml, float arg, Args... args) {
		using namespace asmjit::x86;
		CallExtFuncBuild3(asml, args...);
		//asml.movss(xmm2, arg);
	}
	template<typename Arg0, typename... Args>
	inline void CallExtFuncBuild2(hook::process::AssemblerExp& asml, Arg0 arg, Args... args) {
		using namespace asmjit::x86;
		CallExtFuncBuild3(asml, args...);
		asml.mov(r8, arg);
	}

	// param 1 -> RDX | XMM1
	inline void CallExtFuncBuild1(hook::process::AssemblerExp& asml) {
	}
	template<typename... Args>
	inline void CallExtFuncBuild1(hook::process::AssemblerExp& asml, double arg, Args... args) {
		using namespace asmjit::x86;
		CallExtFuncBuild2(asml, args...);
		//asml.movsd(xmm1, arg);
	}
	template<typename... Args>
	inline void CallExtFuncBuild1(hook::process::AssemblerExp& asml, float arg, Args... args) {
		using namespace asmjit::x86;
		CallExtFuncBuild2(asml, args...);
		//asml.movss(xmm1, arg);
	}
	template<typename Arg0, typename... Args>
	inline void CallExtFuncBuild1(hook::process::AssemblerExp& asml, Arg0 arg, Args... args) {
		using namespace asmjit::x86;
		CallExtFuncBuild2(asml, args...);
		asml.mov(rdx, arg);
	}

	// param 0 -> RCX | XMM0
	inline void CallExtFuncBuild0(hook::process::AssemblerExp& asml) {
	}
	template<typename... Args>
	inline void CallExtFuncBuild0(hook::process::AssemblerExp& asml, double arg, Args... args) {
		using namespace asmjit::x86;
		CallExtFuncBuild1(asml, args...);
		//asml.movsd(xmm0, arg);
	}
	template<typename... Args>
	inline void CallExtFuncBuild0(hook::process::AssemblerExp& asml, float arg, Args... args) {
		using namespace asmjit::x86;
		CallExtFuncBuild1(asml, args...);
		//asml.movsd(xmm0, arg);
	}
	template<typename Arg0, typename... Args>
	inline void CallExtFuncBuild0(hook::process::AssemblerExp& asml, Arg0 arg, Args... args) {
		using namespace asmjit::x86;
		CallExtFuncBuild1(asml, args...);
		asml.mov(rcx, arg);
	}

	/*
	 * Call a function in the target process
	 * @param location function location
	 * @param args function parameters
	 * @return if the function was called correctly
	 */
	template<typename... Args>
	bool Call(Process& proc, uintptr_t location, Args... args) {
		using namespace asmjit::x86;

		constexpr auto argcount = sizeof...(args);
		auto& jr = hook::process::GetJitRuntime();
		asmjit::CodeHolder holder;
		holder.init(jr.environment());
		void* fullInt = nullptr;
		{
			using namespace asmjit::x86;
			hook::process::AssemblerExp asml{ &holder };
			// https://learn.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-170#parameter-passing

			// param0 = RCX | XMM0
			// param1 = RDX | XMM1
			// param2 = R8  | XMM2
			// param3 = R9  | XMM3
			// param+ = stack, last first pushed

			// build params into asm

			CallExtFuncBuild0(asml, args...);

			asml.sub(rsp, 0x20);
			asml.mov(rax, location);
			asml.call(rax);
			// clean the push and store back the func
			asml.add(rsp, 0x20 + (argcount > 4 ? argcount - 4 : 0) * 8);
			asml.ret();

		}
		auto size = holder.codeSize();
		jr.add(&fullInt, &holder);

		if (!fullInt) {
			return false;
		}

		if (core::logs::getlevel() <= core::logs::LVL_TRACE) {
			std::stringstream ss{};

			for (size_t i = 0; i < size; i++) {
				ss << " " << std::format("{:02x}", (int)((byte*)fullInt)[i]);
			}

			LOG_TRACE("Code:{}", ss.str());
		}

		auto allocsize = size + 16;
		auto func = proc.AllocateMemory(allocsize, PAGE_EXECUTE_READWRITE);

		if (!func) {
			LOG_ERROR("bad alloc");
			proc.FreeMemory(func, allocsize);
			return false;
		}


		if (!proc.WriteMemory(func, fullInt, size)) {
			LOG_ERROR("bad write");
			proc.FreeMemory(func, allocsize);
			return false;
		}

		bool ok{};
		HANDLE thr = proc.Exec(func, 0);

		if (thr == INVALID_HANDLE_VALUE || !thr) {
			LOG_ERROR("Invalid handle");
			ok = false;
		}
		else {
			ok = true;
			LOG_TRACE("Wait for thread {}", thr);
			WaitForSingleObject(thr, INFINITE);
			LOG_TRACE("Done {}", thr);
			CloseHandle(thr);
		}
		proc.FreeMemory(func, allocsize);
		return ok;
	}

	/*
	 * Call a function in the target process
	 * @param exp function export
	 * @param args function parameters
	 * @return if the function was called correctly
	 */
	template<typename... Args>
	inline bool Call(Process& proc, const ProcessModuleExport& exp, Args... args) {
		if (!exp) return false;
		return Call(proc, exp.m_location, args...);
	}
#endif

}