#pragma once
#include <tools/cw/cw.hpp>
#include <core/memory_allocator.hpp>
#include <tools/cordycep/cordycep_dump.hpp>

namespace tool::cordycep::dump::t9 {
	typedef uint32_t ScrString;

	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace cw;
	class PoolOptionImpl : public PoolOption {
	public:
		bool m_help = false;
		bool m_any_type = false;
		bool m_dump_info = false;
		bool m_dump_all_available = false;
		bool m_pools{};
		bool m_cf_files{};
		bool m_usev1{};
		bool m_sp{};
		bool m_showErr{};
		bool m_mapLocalized{};
		bool m_ignoreOld{};
		bool analVal{};
		bool decompressLua{};
		bool ignoreArchiveDDL{};
		bool header{};
		const char* m_output = "output_cw";
		const char* m_dump_strings{};
		std::vector<bool> m_dump_types{};
		uint64_t flags{};
		std::unordered_set<std::string> dstrings{};

		bool Compute(const char** args, INT startIndex, INT endIndex);
		void PrintHelp();

		const char* AddString(const char* str) override;

	};

	struct UnlinkerContext {
		Process& proc;
		PoolOptionImpl& opt;
		std::filesystem::path outDir;
		std::function<const char* (uint64_t hash)> GetLocalized;
		compatibility::scobalula::csi::CordycepProc& cordycep;

		const char* GetScrString(ScrString str) const;
	};

	class Unlinker {
	public:
		Unlinker() {}
		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) = 0;
	};
	std::unordered_map<cw::XAssetType, Unlinker*>& GetUnlinkers();
}