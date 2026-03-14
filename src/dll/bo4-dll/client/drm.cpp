#include <dll_includes.hpp>
/*
namespace {

	void* s_original_tls = nullptr;

	void** GetTlsCallbacks() {
		const auto& entry = process::PImageOptHeader()->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];

		if (!entry.Size || !entry.VirtualAddress) {
			return nullptr;
		}

		return reinterpret_cast<void**>(reinterpret_cast<IMAGE_TLS_DIRECTORY*>(process::BasePtr() + entry.VirtualAddress)->AddressOfCallBacks);
	}

	void DisableTLSCallbacks() {
		auto* tls = GetTlsCallbacks();

		if (!tls) {
			return;
		}

		s_original_tls = *tls;
		process::WriteMemSafe(tls, nullptr);

	}

	void RestoreTLSCallbacks() {
		auto* tls = GetTlsCallbacks();

		if (tls) {
			process::WriteMemSafe(tls, s_original_tls);
		}
	}

	void PatchIntegrity() {
		auto& jr = process::GetJitRuntime();
		asmjit::CodeHolder holder;
		holder.init(jr.environment());
		
		void* fullInt = nullptr;
		{
			using namespace asmjit::x86;
			process::AssemblerExp asml{ &holder };
			asml.push(rax);

			asml.mov(rax, qword_ptr(rsp, 8));
			asml.sub(rax, 2);

			asml.push(rax);
			asml.pushregisters();

			asml.mov(r8, qword_ptr(rsp, 0x88));
			asml.mov(rcx, rax);
			asml.mov(rdx, rbp);
			asml.callAligned(nullptr);

			asml.mov(qword_ptr(rsp, 0x80), rax);

			asml.popregisters();
			asml.pop(rax);

			asml.add(rsp, 8);

			asml.mov(dword_ptr(rdx, rcx, 4), eax);

			asml.pop(rax);
			asml.xchg(rax, qword_ptr(rsp));

			asml.add(dword_ptr(rbp, rax), 0xFFFFFFFF);

			asml.mov(rax, dword_ptr(rdx, rcx, 4));

			asml.ret();

			jr.add(&fullInt, &holder);
		}
		
		

		auto full = process::Scan("89 04 8A 83 45 ?? FF");

		LOG_INFO("Patching {} full integrity check(s)...", full.size());

		for (const auto& check : full) {

		}

		auto partial = process::Scan("89 04 8A E9");

		LOG_INFO("Patching {} partial integrity check(s)...", partial.size());

		for (const auto& check : partial) {

		}

		LOG_INFO("Integrity check(s) patched.");
	}

	void PatchDRMPre() {
		DisableTLSCallbacks();
	}

	void PatchDRMPost() {
		PatchIntegrity();
	}
}


ADD_INJECTED_SYSTEM(patchdrm, "patchdrm", inject::PRIORITY_INIT, PatchDRMPre, PatchDRMPost);
*/