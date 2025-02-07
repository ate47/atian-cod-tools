#pragma once
#include <utils/memapi.hpp>
#include <compatibility/scobalula_csi.hpp>

namespace tool::cordycep::dump {
	struct XAsset64 {
		uintptr_t Header;
		uint64_t Temp;
		uintptr_t Next;
		uintptr_t Previous;
		uint64_t ID;
		uint64_t Type;
		int64_t HeaderSize;
	};

	struct XAssetPool64 {
		uintptr_t Root;
		uintptr_t End;
		uint64_t LookupTable;
		uint64_t HeaderMemory;
		uint64_t AssetMemory;
	};

	class PoolOption {
	public:
		virtual const char* AddString(const char* str) = 0;
	};
	int ForEachEntry(Process& proc, XAssetPool64& pool, std::function<bool(const XAsset64& asset, size_t count)> func);

	typedef int (*dumpfunc)(Process& proc, compatibility::scobalula::csi::CordycepProc& cordycep, int argc, const char* argv[]);
	struct CordycepDumper {
		compatibility::scobalula::csi::CordycepGame game;
		dumpfunc dump;
	};

	std::vector<CordycepDumper*>& GetDumpers();
}