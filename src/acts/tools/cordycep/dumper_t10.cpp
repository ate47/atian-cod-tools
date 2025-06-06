#include <includes.hpp>
#include <tools/hashes/compiled_files.hpp>
#include <tools/pool.hpp>
#include <tools/utils/data_utils.hpp>
#include <tools/cordycep/t10/dumper_t10.hpp>

namespace tool::cordycep::dump::t10 {
	using namespace compatibility::scobalula::csi;
	using namespace tool::cordycep::dump;
	using namespace bo6;
	namespace {
		struct LocalizeEntry {
			uint64_t hash;
			uintptr_t value;
		};

		bool IsNull(void* ptr, size_t p) {
			byte* pp{ (byte*)ptr };
			size_t r{ p };
			static byte nullBuff[0x1000]{};
			while (r) {
				size_t tr{ std::min<size_t>(sizeof(nullBuff), r) };

				if (std::memcmp(pp, nullBuff, tr)) {
					return false;
				}

				r -= tr;
				pp += tr;
			}
			return true;
		}

		int dpcordimpl(Process& proc, compatibility::scobalula::csi::CordycepProc& cordycep, int argc, const char* argv[]) {
			PoolOptionImpl opt{};

			if (!opt.Compute(argv, 2, argc) || opt.m_help) {
				opt.PrintHelp();
				return tool::OK;
			}


			auto [pools, ok] = proc.ReadMemoryArray<XAssetPool64>(cordycep.poolsAddress, T10R_ASSET_COUNT);

			if (!ok) {
				LOG_ERROR("Can't read pools");
				return tool::BASIC_ERROR;
			}

			std::filesystem::path outDir = opt.m_output;

			if (opt.m_pools) {
				std::filesystem::path loc{ outDir / "pools_bo6.csv" };
				std::ofstream os{ loc };
				if (!os) {
					LOG_ERROR("Can't open {}", loc.string());
					return tool::BASIC_ERROR;
				}
				utils::CloseEnd ce{ [&os] {os.close(); } };

				os << "id,name,size,elements";

				XAsset64 asset{};
				for (size_t i = 0; i < T10R_ASSET_COUNT; i++) {
					const char* name = bo6::PoolNameRelease((bo6::T10RAssetType)i);

					os << "\n" << i << "," << name << ",";

					XAssetPool64& pool = pools[i];

					if (!pool.Root) {
						os << "0,0";
						continue;
					}


					if (!proc.ReadMemory(&asset, pool.Root, sizeof(asset))) {
						os << "<error>,<error>";
						LOG_ERROR("Can't dump pool {}", name);
						continue;
					}

					size_t count{ 1 };

					while (asset.Next) {
						if (!proc.ReadMemory(&asset, asset.Next, sizeof(asset))) {
							count = std::string::npos;
							LOG_ERROR("Can't dump pool {}", name);
							break;
						}

						count++;
					}

					if (count == std::string::npos) {
						os << "<error>";
					}
					else {
						os << std::hex << asset.HeaderSize << "," << count;
					}
				}

				LOG_INFO("Dump into {}", loc.string());

				return tool::OK;
			}

			if (!opt.m_any_type) {
				opt.PrintHelp();
				return tool::OK;
			}

			hashutils::ReadDefaultFile();

			size_t total{};

			if (opt.analVal) {
				for (size_t i = 0; i < T10R_ASSET_COUNT; i++) {
					T10RAssetType type{ (T10RAssetType)i };

					if (!opt.m_dump_types[type] && !opt.m_dump_all_available) {
						continue; // not handled
					}

					size_t nullFiles{};
					const char* name{ PoolNameRelease(type) };
					std::filesystem::path outDirPool{ outDir / name };
					std::filesystem::create_directories(outDirPool);
					int du = ForEachEntry(proc, pools[type], [&nullFiles, &outDirPool, &proc](const XAsset64& asset, size_t count) -> bool {
						std::filesystem::path outFile{ outDirPool / std::format("file_{:x}.txt", asset.ID) };

						try {
							auto buff{ proc.ReadMemoryArrayEx<byte>(asset.Header, asset.HeaderSize) };
							if (IsNull(buff.get(), asset.HeaderSize)) {
								//LOG_ERROR("null file {}", outFile.string());
								nullFiles++;
								return false;
							}

							utils::OutFileCE os{ outFile };

							os << hashutils::ExtractTmp("hash", asset.ID) << "\n\n";

							tool::pool::WriteHex(os, asset.Header, (void*)buff.get(), asset.HeaderSize, proc);
							LOG_INFO("dump {}", outFile.string());
						}
						catch (...) {
							LOG_INFO("error reading {}", outFile.string());
						}

						return true;
					});

					if (nullFiles) {
						LOG_ERROR("Find {} null files", nullFiles);
					}

					if (du < 0) {
						LOG_ERROR("Error reading pool: {}", name);
					}
					else {
						total += du;
					}

				}

				return tool::OK;
			}

			auto HandlePool = [&opt, &pools, &proc, &total](T10RAssetType type, const std::filesystem::path& outDir, std::function<bool(const XAsset64& asset, size_t count)> func) {
				if (!opt.m_dump_types[type] && !opt.m_dump_all_available) {
					return;
				}

				opt.m_dump_types[type] = false;


				if (opt.m_sp) {
					auto& spMap{ GetSpMap() };
					auto it{ spMap.find(type) };
					if (it == spMap.end()) return; // ignore asset
					type = (bo6::T10RAssetType)it->second->to;
				}

				std::filesystem::create_directories(outDir);

				LOG_INFO("Reading pool {}", (int)type);

				int du = ForEachEntry(proc, pools[type], func);

				if (du < 0) {
					LOG_ERROR("Error reading pool: {}", (int)type);
				}
				else {
					total += du;
				}
				};

			struct {
				bool loaded{};
				core::memory_allocator::MemoryAllocator alloc{};
				std::unordered_map<uint64_t, const char*> map{};
			} localized;

			std::function<const char*(uint64_t hash)> GetLocalized = [&opt, &localized, &proc, &pools](uint64_t hash) -> const char* {
				if (!opt.m_mapLocalized) {
					return hashutils::ExtractTmp("hash", hash);
				}
				if (!localized.loaded) {

					localized.loaded = true;

					ForEachEntry(proc, pools[T10R_ASSET_LOCALIZE], [&proc, &localized](const XAsset64& asset, size_t count) {
						LocalizeEntry entry{};
						if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
							LOG_ERROR("Can't read LocalizeEntry {:x}", asset.Header);
							return false;
						}

						localized.map[entry.hash & hash::MASK60] = localized.alloc.CloneStr(proc.ReadStringTmp(entry.value));

						return true;
						});
				}

				auto it = localized.map.find(hash & hash::MASK60);
				if (it == localized.map.end()) {
					return hashutils::ExtractTmp("hash", hash);
				}
				return it->second;
				};

			std::filesystem::path dumpDir = outDir / "dump";
			std::filesystem::create_directories(dumpDir);


			UnlinkerContext uctx{ proc, opt, outDir, GetLocalized, cordycep };

			for (auto& [type, unlinker] : GetUnlinkers()) {
				HandlePool(type, outDir, [&uctx, &unlinker](const XAsset64& asset, size_t count) { return unlinker->Unlink(asset, uctx); });
			}

			// LocalizeEntry
			if (opt.m_dump_types[T10R_ASSET_LOCALIZE] || opt.m_dump_all_available) {
				opt.m_dump_types[T10R_ASSET_LOCALIZE] = false;

				std::ostringstream os{};

				os << "{";
				int du = ForEachEntry(proc, pools[T10R_ASSET_LOCALIZE], [&proc, &os](const XAsset64& asset, size_t count) {
					LocalizeEntry entry{};
					if (!proc.ReadMemory(&entry, asset.Header, sizeof(entry))) {
						LOG_ERROR("Can't read LocalizeEntry {:x}", asset.Header);
						return false;
					}

					if (count) os << ",";

					utils::Padding(os << "\n", 1) << "\"#" << hashutils::ExtractTmp("hash", entry.hash) << "\": \"";
					utils::PrintFormattedString(os, proc.ReadStringTmp(entry.value)) << "\"";

					return true;
					});

				os << "\n}";

				if (du < 0) {
					LOG_ERROR("Error reading pool T10R_ASSET_LOCALIZE");
				}
				else {
					total += du;

					std::filesystem::create_directories(outDir);

					std::filesystem::path loc{ dumpDir / utils::va("localize.%s", opt.m_cf_files ? ".cf" : "json") };
					std::filesystem::create_directories(loc.parent_path());
					if (opt.m_cf_files) {
						std::ofstream osf{ loc, std::ios::binary };
						if (osf) {
							tool::hashes::compiled_files::CompiledFileHeader header{};
							header.name = hash::Hash64("localize.json");
							header.isString = true;
							strcpy_s(header.type, "localize");
							strcpy_s(header.preferedExtension, "json");
							osf.write((const char*)&header, sizeof(header));
							std::string osc = os.str();
							osf.write(osc.data(), osc.size());
							osf.close();
							LOG_INFO("Dump into {}", loc.string());
						}
					}
					else {
						utils::WriteFile(loc, os.str());
					}

				}

			}

			LOG_INFO("Dumped {} file(s)", total);

			if (opt.m_dump_strings) {
				std::ofstream outStr{ opt.m_dump_strings };
				if (outStr) {
					outStr << "string";
					for (const std::string& str : opt.dstrings) {
						outStr << "\n" << str;
					}
					outStr.close();
					LOG_INFO("Dumped {} string(s) into '{}'", opt.dstrings.size(), opt.m_dump_strings);
				}
				else {
					LOG_ERROR("Can't open file '{}'", opt.m_dump_strings);
				}
			}

			return tool::OK;
		}
	}

	
	bool PoolOptionImpl::Compute(const char** args, INT startIndex, INT endIndex) {
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
			else if (!_strcmpi("--guessHeader", arg)) {
				guessHeader = true;
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
	void PoolOptionImpl::PrintHelp() {
		LOG_INFO("-h --help            : Print help");
		LOG_INFO("-o --output [d]      : Output dir");
		LOG_INFO("-p --pools           : Dump pools");
		LOG_INFO("-C --cf              : Produce .cf files");
		LOG_INFO("-e --err             : Show errors");
		LOG_INFO("-L --localized       : Map localized");
		LOG_INFO("-a --all             : Dump all available pools");
		LOG_INFO("--v1                 : Use old pool dumper (compatibility)");
		LOG_INFO("--analVal            : Dump binaries");
		LOG_INFO("--sp                 : Sp dumper");
		LOG_INFO("--ignoreOld          : Ignore old");
		LOG_INFO("--decompressLua      : Decompress lua");
		LOG_INFO("--guessHeader        : Use header guessers");
	}

	const char* PoolOptionImpl::AddString(const char* str) {
		hashutils::AddPrecomputed(hash::Hash64(str), str);
		hashutils::AddPrecomputed(hash::HashT10Scr(str), str);
		hashutils::AddPrecomputed(hash::HashIWAsset(str), str);
		if (m_dump_strings) {
			dstrings.insert(str);
		}
		return str;
	}

	const char* UnlinkerContext::GetScrString(ScrString str) const {
		return proc.ReadStringTmp(cordycep.stringsAddress + str);
	}

	std::unordered_map<bo6::T10RAssetType, Unlinker*>& GetUnlinkers() {
		static std::unordered_map<bo6::T10RAssetType, Unlinker*> unlinkers{};
		return unlinkers;
	}
	std::unordered_map<bo6::T10RAssetType, AssetMapping*>& GetSpMap() {
		static std::unordered_map<bo6::T10RAssetType, AssetMapping*> map{};
		return map;
	}

	utils::ArrayAdder<tool::cordycep::dump::CordycepDumper> impl{ tool::cordycep::dump::GetDumpers(), compatibility::scobalula::csi::CG_BO6, dpcordimpl, T10R_ASSET_COUNT };
}