#include <includes.hpp>
#include <tools/cordycep_dump.hpp>
#include <tools/mw19/mw19.hpp>
#include "compatibility/xensik_gscbin.hpp"

namespace {
	using namespace tool::cordycep::dump;
	class PoolOptionImpl : public PoolOption {
	public:
		bool m_help = false;
		bool m_any_type = false;
		bool m_dump_info = false;
		bool m_dump_all_available = false;
		bool m_pools{};
		bool m_cf_files{};
		bool m_showErr{};
		bool m_mapLocalized{};
		const char* m_output = "output_mw19";
		const char* m_dump_strings{};
		std::vector<bool> m_dump_types{};
		uint64_t flags{};
		std::unordered_set<std::string> dstrings{};

		bool Compute(const char** args, INT startIndex, INT endIndex) {
			m_dump_types.clear();
			m_dump_types.reserve(tool::mw19::IW19_ASSETTYPE_COUNT);
			for (size_t i = 0; i < tool::mw19::IW19_ASSETTYPE_COUNT; i++) {
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
					tool::mw19::IW19PoolType assetType = tool::mw19::PoolIdOld(arg);

					if (assetType == tool::mw19::IW19_ASSETTYPE_COUNT) {
						if (std::isdigit(arg[0])) {
							try {
								assetType = (tool::mw19::IW19PoolType)std::strtol(arg, nullptr, 10);
							}
							catch (const std::invalid_argument& e) {
								std::cerr << e.what() << "\n";
								assetType = tool::mw19::IW19_ASSETTYPE_COUNT;
							}
						}
						if (assetType < 0 || assetType >= tool::mw19::IW19_ASSETTYPE_COUNT) {
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
		}

		const char* AddString(const char* str) override {
			if (m_dump_strings) {
				dstrings.insert(str);
			}
			return str;
		}
	};
	struct ScriptFile {
		uintptr_t name; // const char*
		int compressedLen;
		int len;
		int bytecodeLen;
		uintptr_t buffer; // const char* 
		uintptr_t bytecode; // unsigned __int8* 
	};


	int dpcordimpl(Process& proc, compatibility::scobalula::csi::CordycepProc& cordycep, int argc, const char* argv[]) {
		PoolOptionImpl opt{};

		if (!opt.Compute(argv, 2, argc) || opt.m_help) {
			opt.PrintHelp();
			return tool::OK;
		}


		auto [pools, ok] = proc.ReadMemoryArray<compatibility::scobalula::csi::XAssetPool64>(cordycep.poolsAddress, tool::mw19::IW19_ASSETTYPE_COUNT);

		if (!ok) {
			LOG_ERROR("Can't read pools");
			return tool::BASIC_ERROR;
		}

		std::filesystem::path outDir = opt.m_output;


		if (!opt.m_any_type) {
			opt.PrintHelp();
			return tool::OK;
		}

		hashutils::ReadDefaultFile();

		size_t total{};

		auto HandlePool = [&opt, &pools, &proc, &total](tool::mw19::IW19PoolType type, const std::filesystem::path& outDir, std::function<bool(const compatibility::scobalula::csi::XAsset64& asset, size_t count)> func) {
			if (!opt.m_dump_types[type] && !opt.m_dump_all_available) {
				return;
			}

			opt.m_dump_types[type] = false;

			std::filesystem::create_directories(outDir);

			int du = ForEachEntry(proc, pools[type], func);

			if (du < 0) {
				LOG_ERROR("Error reading pool: {}", tool::mw19::PoolNameOld(type));
			}
			else {
				total += du;
			}
			};

		const std::filesystem::path gscDir = outDir / "gsc";
		HandlePool(tool::mw19::IW19_ASSETTYPE_SCRIPTFILE, gscDir, [&opt, &proc, gscDir](const compatibility::scobalula::csi::XAsset64& asset, size_t count) -> bool {
			ScriptFile entry{};
			if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
				LOG_ERROR("Can't read scriptfile entry {:x}", asset.Header);
				return false;
			}

			std::vector<byte> dumpFile{};

			const char* fileNameRead = proc.ReadStringTmp(entry.name);

			utils::Allocate(dumpFile, sizeof(compatibility::xensik::gscbin::GscBinHeader));

			compatibility::xensik::gscbin::GscBinHeader& header{ *reinterpret_cast<compatibility::xensik::gscbin::GscBinHeader*>(dumpFile.data()) };

			header.Magic() = compatibility::xensik::gscbin::GSCBIN_MAGIC;
			header.compressedLen = entry.compressedLen;
			header.len = entry.len;
			header.bytecodeLen = entry.bytecodeLen;

			size_t bufferLoc = utils::Allocate(dumpFile, entry.compressedLen);
			size_t byteCodeLoc = utils::Allocate(dumpFile, entry.bytecodeLen);

			if (entry.compressedLen) {
				if (!proc.ReadMemory(&dumpFile[bufferLoc], entry.buffer, entry.compressedLen)) {
					LOG_ERROR("Can't read scriptfile buffer {}", fileNameRead);
					return false;
				}
			}
			if (entry.bytecodeLen) {
				if (!proc.ReadMemory(&dumpFile[byteCodeLoc], entry.bytecode, entry.bytecodeLen)) {
					LOG_ERROR("Can't read scriptfile bytecode buffer {}", fileNameRead);
					return false;
				}
			}

			char* fileName = utils::CloneString(fileNameRead);

			std::string_view nameView{ fileName };

			if (nameView.ends_with(".gsc")) {
				fileName[nameView.length() - 4] = 0;
			}
			fileName = utils::va("%s.gscbin", fileName);

			std::filesystem::path out = gscDir / fileName;
			std::filesystem::create_directories(out.parent_path());

			if (!utils::WriteFile(out, dumpFile.data(), dumpFile.size())) {
				LOG_ERROR("Can't write file {}", out.string());
				return false;
			}
			LOG_INFO("Dumped {} ({})", out.string(), fileNameRead);

			return true;
			});

		LOG_INFO("Dumped {} files", total);

		return tool::OK;
	}

	utils::ArrayAdder<tool::cordycep::dump::CordycepDumper> impl{ tool::cordycep::dump::GetDumpers(), compatibility::scobalula::csi::CG_MW4, dpcordimpl, tool::mw19::IW19_ASSETTYPE_COUNT };
}