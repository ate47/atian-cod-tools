#pragma once
#include <utils/memapi.hpp>
#include <compatibility/scobalula_csi.hpp>

namespace tool::cordycep::dump {

	class PoolOption {
	public:
		virtual const char* AddString(const char* str) = 0;
	};
	int ForEachEntry(Process& proc, compatibility::scobalula::csi::XAssetPool64& pool, std::function<bool(const compatibility::scobalula::csi::XAsset64& asset, size_t count)> func);

	typedef int (*dumpfunc)(Process& proc, compatibility::scobalula::csi::CordycepProc& cordycep, int argc, const char* argv[]);
	struct CordycepDumper {
		compatibility::scobalula::csi::CordycepGame game;
		dumpfunc dump;
		size_t numpools;
	};

	std::vector<CordycepDumper*>& GetDumpers();
}