#pragma once
#include <hook/module_mapper.hpp>
#include <hook/memory.hpp>

namespace deps::bo4 {

	struct BDiffState {
		bool headerRead{};
		bool error{};
		bool eof{};
		unsigned int features{};
	};

	class Bo4 {
		hook::module_mapper::Module mod{ true };
		hook::library::Library bo4{};

	public:
		Bo4() {}
		Bo4(hook::library::Library&& b) : bo4(b) {}
		Bo4(hook::library::Library& b) : bo4(b) {}

		bool LoadFromDeps() {
			if (mod.Load("bo4.exe", true, false) || mod.Load("BlackOps4.exe", true, false) || mod.Load("BlackOps4_dump.exe", true, false)) {
				bo4 = *mod;
			}
			else {
				bo4.ClearModule();
			}

			return bo4;
		}

		constexpr const hook::library::Library* operator->() const {
			return &bo4;
		}

		constexpr const hook::library::Library& operator*() const {
			return bo4;
		}

		constexpr operator bool() const {
			return bo4;
		}

		bool bdiff(BDiffState* state, 
			byte*(*srcDataCallback)(uint64_t offset, uint64_t size),
			byte*(*patchDataCB)(uint64_t offset, uint64_t size, uint64_t* pOffset),
			byte*(*destDataCB)(uint64_t size)) {
			return bo4.Get<bool(void*, void*, void*, void*)>(0x3CDDDE0)(state, srcDataCallback, patchDataCB, destDataCB);
		}

		void PatchError(void(*errorCallBack)(uint32_t a1)) {
			hook::memory::RedirectJmp(bo4[0x288B110], errorCallBack); // Com_Error_
		}

		void PatchErrorAsThrow() {
			PatchError([](uint32_t code) { throw std::runtime_error(std::format("error {}", code)); });
		}

	};


}