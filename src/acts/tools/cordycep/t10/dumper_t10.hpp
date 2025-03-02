#pragma once
#include <tools/bo6/bo6.hpp>
#include <core/memory_allocator.hpp>
#include <tools/cordycep_dump.hpp>

namespace tool::cordycep::dump::t10 {
	struct GfxImage;
	struct XModel;
	
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace bo6;
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
		const char* m_output = "output_bo6";
		const char* m_dump_strings{};
		std::vector<bool> m_dump_types{};
		uint64_t flags{};
		std::unordered_set<std::string> dstrings{};

		bool Compute(const char** args, INT startIndex, INT endIndex) {
			m_dump_types.clear();
			m_dump_types.reserve(T10R_ASSET_COUNT);
			for (size_t i = 0; i < T10R_ASSET_COUNT; i++) {
				m_dump_types.push_back(false);
			}
			// default values
			for (size_t i = startIndex; i < endIndex; i++) {
				const char* arg = args[i];

				if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
					m_help = true;
				}
				else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					m_output = args[++i];
				}
				else if (!_strcmpi("--all", arg) || !strcmp("-a", arg)) {
					m_dump_all_available = true;
					m_any_type = true;
				}
				else if (!_strcmpi("--pools", arg) || !strcmp("-p", arg)) {
					m_pools = true;
				}
				else if (!_strcmpi("--cf", arg) || !strcmp("-C", arg)) {
					m_cf_files = true;
				}
				else if (!_strcmpi("--localized", arg) || !strcmp("-L", arg)) {
					m_mapLocalized = true;
				}
				else if (!_strcmpi("--err", arg) || !strcmp("-e", arg)) {
					m_showErr = true;
				}
				else if (!_strcmpi("--sp", arg)) {
					m_sp = true;
				}
				else if (!_strcmpi("--v1", arg)) {
					m_usev1 = true;
				}
				else if (!_strcmpi("--analVal", arg)) {
					analVal = true;
				}
				else if (!_strcmpi("--ignoreOld", arg)) {
					m_ignoreOld = true;
				}
				else if (!_strcmpi("--decompressLua", arg)) {
					decompressLua = true;
				}
				else if (!_strcmpi("--ignoreArchiveDDL", arg)) {
					ignoreArchiveDDL = true;
				}
				else if (!_strcmpi("--header", arg)) {
					header = true;
				}
				else if (!strcmp("-S", arg) || !_strcmpi("--strings", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					m_dump_strings = args[++i];
				}
				else if (*arg == '-') {
					std::cerr << "Invalid argument: " << arg << "!\n";
					return false;
				}
				else {
					T10RAssetType assetType = PoolIdRelease(arg);

					if (assetType == T10R_ASSET_COUNT) {
						if (std::isdigit(arg[0])) {
							try {
								assetType = (T10RAssetType)std::strtol(arg, nullptr, 10);
							}
							catch (const std::invalid_argument& e) {
								std::cerr << e.what() << "\n";
								assetType = T10R_ASSET_COUNT;
							}
						}
						if (assetType < 0 || assetType >= T10R_ASSET_COUNT) {
							std::cerr << "Invalid pool name: " << arg << "!\n";
							return false;
						}
					}

					m_dump_types[assetType] = true;
					m_any_type = true;
				}
			}
			return true;
		}
		void PrintHelp() {
			LOG_INFO("-h --help            : Print help");
			LOG_INFO("-o --output [d]      : Output dir");
			LOG_INFO("-p --pools           : Dump pools");
			LOG_INFO("-C --cf              : Produce .cf files");
			LOG_INFO("-e --err             : Show errors");
			LOG_INFO("-L --localized       : Map localized");
			LOG_INFO("-a --all             : Dump all available pools");
			LOG_INFO("--v1                 : Use old pool dumper (compatibility)");
			LOG_INFO("--analVal            : Dump binaries");
			LOG_INFO("--ignoreOld          : Ignore old");
			LOG_INFO("--decompressLua      : Decompress lua");
		}

		const char* AddString(const char* str) override {
			hashutils::AddPrecomputed(hash::Hash64(str), str);
			hashutils::AddPrecomputed(hash::HashT10Scr(str), str);
			hashutils::AddPrecomputed(hash::HashIWRes(str), str);
			if (m_dump_strings) {
				dstrings.insert(str);
			}
			return str;
		}
	};

	struct UnlinkerContext {
		Process& proc;
		PoolOptionImpl& opt;
		std::filesystem::path outDir;
		std::function<const char* (uint64_t hash)> GetLocalized;
	};

	class Unlinker {
	public:
		Unlinker() {}
		virtual bool Unlink(const compatibility::scobalula::csi::XAsset64& asset, UnlinkerContext& ctx) = 0;
	};

	std::unordered_map<bo6::T10RAssetType, Unlinker*>& GetUnlinkers();

}