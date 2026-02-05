#pragma once
#include <core/strings.hpp>
#include <utils/enumlist.hpp>
#include <utils/hash_mini.hpp>
#include <hook/library.hpp>
#include <hook/scan_container.hpp>
#include <hook/module_mapper.hpp>

/*
 * Container for pool names. Creates a mapping between hashed ids and game ids
 */
namespace games::cod::asset_names {

	constexpr size_t MIN_ASSET_COUNT = 40;
	constexpr size_t MAX_ASSET_COUNT = 512;

	struct AssetDumpFileOptions {
		bool dumpTypeNames{ true };
		bool dumpHashedHeader{ true };
		bool dumpTypeHeader{ true };
		bool dumpAssetInfo{ true };
		const char* baseFileName{ "asset" };
		const char* assetPrefix{ "ASSET_TYPE_" };
		const char* assetHashedPrefix{ "ASSET_HASHED_" };
		const char* assetTypeName{ "AssetType" };
		const char* assetHashedName{ "AssetHashed" };
		const char* assetNames{ "poolNames" };
		core::logs::loglevel logLevel{ core::logs::LVL_DEBUG };
	};

	template<typename AssetHashed = uint32_t, typename AssetId = uint32_t>
	class AssetNames {
		struct HashedType {
			AssetHashed hash;
			AssetId type;
			const char* name;
		};

		const char* first;
		const char* last;
		AssetHashed(*PoolId)(const char* id);

		HashedType typeMaps[MAX_ASSET_COUNT]{};
		size_t typeMapsCount{};
		const char* typeNames[MAX_ASSET_COUNT]{};
		AssetHashed typeExeToActs[MAX_ASSET_COUNT]{};
		utils::EnumList<AssetId, (AssetId)MAX_ASSET_COUNT> handleList;

	public:
		/*
		 * Asset name constructor
		 * @param first first asset type name
		 * @param last post last asset type name
		 * @param PoolId hash function for asset names, default to hash::HashX32
		 */
		AssetNames(const char* first, const char* last, AssetHashed(*PoolId)(const char* id) = [](const char* id) { return (AssetHashed)hash::HashX32(id); })
			: first(first), last(last), PoolId(PoolId),
			handleList{ [this](const char* type) { return GetExePoolId(type); } } {
		}

		template<typename Type>
		void InitMap0(Type scan, const hook::library::Library& lib) {
			for (hook::library::ScanResult& firstStringOffsetRes : scan.ScanString(first)) {
				uintptr_t firstStringOffset{ firstStringOffsetRes.GetPtr<uintptr_t>() };
				LOG_TRACE("try string \"{}\" -> 0x{:x}", first, lib.Rloc(firstStringOffset));
				for (hook::library::ScanResult& poolNamesRes : scan.ScanNumber(firstStringOffset)) {
					uintptr_t poolNamesOffset{ poolNamesRes.GetPtr<uintptr_t>() };

					uintptr_t* poolNames{ reinterpret_cast<uintptr_t*>(poolNamesOffset) };
					LOG_TRACE("try poolNames -> 0x{:x}", lib.Rloc(poolNamesOffset));

					// we can try to see if the next one is a valid string

					void* next{ reinterpret_cast<void*>(poolNames[1]) };

					if (next < *lib || next > lib[0x10000000]) {
						LOG_TRACE("Not inside library");
						continue; // not inside the module
					}


					size_t count{};
					while (true) {
						if (!poolNames[count]) {
							if (last) {
								LOG_ERROR("Can't find last pool name"); // cw?
								count = 0;
							}
							break;
						}
						const char* cc = lib.Rebase<const char>(poolNames[count]);
						if (last && !_strcmpi(cc, last)) break;
						count++;
					}

					if (count <= MIN_ASSET_COUNT) {
						LOG_TRACE("Not enough candidates: {}", count);
						continue;
					}

					if (count > MAX_ASSET_COUNT) {
						LOG_TRACE("Too many candidates: {}", count);
						continue;
					}

					if (poolNamesRes.entry) {
						poolNamesRes.entry->name = "g_assetNames";
					}

					typeMapsCount = count;
					for (size_t i = 0; i < count; i++) {
						AssetId tid{ (AssetId)i };
						HashedType* type = &typeMaps[i];
						type->type = tid;
						type->name = lib.Rebase<const char>(poolNames[i]);
						type->hash = PoolId(type->name);
						typeNames[i] = type->name;

						// map this id to acts
						typeExeToActs[tid] = type->hash;
						// add precomputed values
						hashutils::AddPrecomputed(type->hash, type->name, true);
					}

					// sort it for better usage
					std::sort(typeMaps, typeMaps + typeMapsCount, [](auto& a, auto& b) { return a.hash < b.hash; });
					LOG_DEBUG("{} asset names loaded", typeMapsCount);
					return;
				}
			}
			throw std::runtime_error("Can't scan asset pool names");
		}


		/*
		 * Create a type map using a game, throw an exception if it fails
		 * @param lib game library
		 */
		void InitMap(const hook::library::Library& lib) {
			InitMap0(lib, lib);
		}

		/*
		 * Create a type map using a game, throw an exception if it fails
		 * @param mod game module
		 */
		void InitMap(hook::module_mapper::Module& mod) {
			InitMap0(mod.GetScanContainer(), mod.GetLibrary());
		}

		/*
		 * Dump asset info in a directory
		 * @param dir dump directory
		 * @param opts options (non null)
		 */
		void DumpFiles(const std::filesystem::path& dir, AssetDumpFileOptions* opts) {
			std::filesystem::create_directories(dir);
			if (opts->dumpAssetInfo) {
				std::filesystem::path f{ dir / std::format("{}.csv", opts->baseFileName) };

				{
					utils::OutFileCE os{ f, true };

					os << "id,hash,name";
					for (size_t i = 0; i < typeMapsCount; i++) {
						os << "\n" << std::dec << i;
						if (opts->dumpHashedHeader) {
							os << "," << std::hex << PoolId(typeNames[i]);
						}
						os << "," << typeNames[i];
					}
				}
				LOG_LVLF(opts->logLevel, "Dump assets data to {}", f.string());
			}

			if (opts->dumpTypeNames) {
				std::filesystem::path f{ dir / std::format("{}_names.cpp", opts->baseFileName) };

				{
					utils::OutFileCE os{ f, true };

					os << "const char* " << opts->assetNames << "[] {";
					for (size_t i = 0; i < typeMapsCount; i++) {
						os << "\n    \"" << typeNames[i] << "\", // " << std::hex << i;
					}
					os << "\n};";
				}
				LOG_LVLF(opts->logLevel, "Dump assets data to {}", f.string());
			}

			if (opts->dumpTypeHeader) {
				std::filesystem::path f{ dir / std::format("{}_ids.hpp", opts->baseFileName) };

				{
					utils::OutFileCE os{ f, true };

					os << "enum " << opts->assetTypeName << " : uint32_t {";
					for (size_t i = 0; i < typeMapsCount; i++) {
						os << "\n    " << opts->assetPrefix << GetCppName((AssetId)i)
							<< " = 0x" << std::hex << i << ", // " << typeNames[i];
					}
					os << "\n};\n\n";
				}
				LOG_LVLF(opts->logLevel, "Dump assets data to {}", f.string());
			}

			if (opts->dumpHashedHeader) {
				std::filesystem::path f{ dir / std::format("{}_hashes.hpp", opts->baseFileName) };

				{
					utils::OutFileCE os{ f, true };

					os << "enum " << opts->assetHashedName << " : uint32_t {";
					for (size_t i = 0; i < typeMapsCount; i++) {
						os << "\n    " << opts->assetHashedPrefix << GetCppName((AssetId)i)
							<< " = 0x" << std::hex << PoolId(typeNames[i]) << ", // " << typeNames[i];
					}
					os << "\n};\n";
				}
				LOG_LVLF(opts->logLevel, "Dump assets data to {}", f.string());
			}

		}

		/*
		 * Load asset config, used with ShouldHandle
		 * @param cfg config data
		 */
		void LoadAssetConfig(const char* cfg) {
			// load the asset filter config 
			handleList.Clear();
			if (cfg) {
				handleList.LoadConfig(cfg);
			}
		}

		/*
		 * @return the invalid id
		 */
		constexpr AssetId InvalidId() const {
			return (AssetId)typeMapsCount;
		}

		/*
		 * @return the types count
		 */
		constexpr size_t TypesCount() const {
			return typeMapsCount;
		}

		/*
		 * test if an asset type config with LoadAssetConfig should be handled
		 * @param type type
		 * @return if the type should be handled
		 */
		bool ShouldHandle(AssetId type) {
			return handleList.Empty() || handleList[type];
		}

		/*
		 * Get the name of a type
		 * @param type type
		 * @return name
		 */
		const char* GetTypeName(AssetId type) {
			if (type >= typeMapsCount) {
				return "invalid";
			}
			return typeNames[type];
		}

		/*
		 * Get the cpp name of a type
		 * @param type type
		 * @return name
		 */
		const char* GetCppName(AssetId type) {
			const char* typeName{ GetTypeName(type) };
			return core::strings::GetCppIdentifier(typeName);
		}

		/*
		 * Get the name of a hashed type
		 * @param hash hashed type
		 * @return name
		 */
		const char* GetPoolName(AssetHashed hash) {
			return GetTypeName(GetExePoolId(hash));
		}

		/*
		 * Get the hashed value of a type
		 * @param type type
		 * @return hashed value
		 */
		AssetHashed GetHashType(AssetId type) {
			return PoolId(GetTypeName(type));
		}

		/*
		 * Get a type from its name
		 * @param name name
		 * @return type or invalid value
		 */
		AssetId GetExePoolId(const char* name) {
			return GetExePoolId(PoolId(name));
		}

		/*
		 * Get a config from an hashed value
		 * @param val value
		 * @param failMissing throw std::runtime_error in case of error
		 * @return hash info or throw exception if failMissing=true or nullptr
		 */
		const HashedType* GetMappedType(AssetHashed val, bool failMissing = true) const {
			size_t min{}, max{ typeMapsCount };

			while (min < max) {
				size_t mid{ (min + max) / 2 };
				if (typeMaps[mid].hash > val) {
					max = mid;
				}
				else if (typeMaps[mid].hash != val) {
					min = mid + 1;
				}
				else {
					return &typeMaps[mid];
				}
			}

			if (failMissing) {
				throw std::runtime_error(std::format("Invalid asset type name {}", hashutils::ExtractTmp("hash", val)));
			}

			return nullptr;
		}

		/*
		 * Get a type from its hashed type
		 * @param name name
		 * @return type or invalid value
		 */
		AssetId GetExePoolId(AssetHashed name) {
			const HashedType* ht{ GetMappedType(name, false) };
			if (!ht) {
				return (AssetId)typeMapsCount;
			}
			return ht->type;
		}
	};
}