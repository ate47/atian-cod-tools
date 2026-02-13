#pragma once
#include <core/config.hpp>
#include <hook/scan_container.hpp>
#include <deps/idc_builder.hpp>
#include <games/cod/asset_names.hpp>

namespace acts::game_data {
	constexpr const char* BASE_PARENT = "scans";
	std::filesystem::path GetBaseDir();
	size_t ParseOffsetScan(const std::string& scan);
	std::vector<std::string> GetAllGameData();

	enum CTypeType {
		CTT_UNKNOWN = 0,
		CTT_STRUCT,
		CTT_ENUM,
	};

	enum ScanType {
		SCT_UNKNOWN = 0,
		SCT_RELATIVE,
		SCT_ABSOLUTE,
		SCT_GET_OFFSET32,
		SCT_OFFSET,
	};

	struct ScanData {
		std::string name;
		std::string path;
		std::string ctype;
		size_t offset{};
		int64_t postOffset{};
		ScanType type;
		bool single{};
	};

	class GameData {
		core::config::Config cfg;
		hook::scan_container::ScanContainer* scan{};
		const char* dirname;
	public:
		GameData(const char* dirname);
		GameData(const GameData& other) = delete;
		GameData(GameData&& other) = delete;

		// set the scan container
		void SetScanContainer(hook::scan_container::ScanContainer* container) { scan = container; }
		// get the scan container, fail if none set
		hook::scan_container::ScanContainer& GetScanContainer();
		// Data config
		core::config::Config& Config() { return cfg; };
		// Get name
		const char* GetName() const { return dirname; };

		// validate all scans
		bool ValidateScans();
		// add all types to idc builder
		void AddTypesToIdc(deps::idc_builder::IdcBuilder& builder);
		// add all scans to idc builder
		void ScanAllToIdc(deps::idc_builder::IdcBuilder& builder);
		// add all scans from a pool to idc builder
		void ScanToIdc(deps::idc_builder::IdcBuilder& builder, const char* parent = BASE_PARENT);
		// apply all the null scall of a pool
		void ApplyNullScans(const char* id);
		// Get dump filename
		const char* GetModuleName();
		// get scan data from its id
		ScanData GetScan(const char* id, const char* parent = BASE_PARENT);
		// redirect a scan to a location
		void Redirect(const char* id, void* to, const char* parent = BASE_PARENT);
		// null a scan
		void Nulled(const char* id, const char* parent = BASE_PARENT);
		// create a detour using a scan
		void Detour(const char* id, hook::library::Detour& detour, void* to, const char* parent = BASE_PARENT);
		// get a pointer
		template<typename T>
		void Get(const char* id, T* ptr, const char* parent = BASE_PARENT) {
			*ptr = GetPointer<T>(id, parent);
		}

		// init asset names
		template<typename AssetHashed = uint32_t, typename AssetId = uint32_t>
		void InitAssetNames(games::cod::asset_names::AssetNames<AssetHashed, AssetId>& names) {
			const char* firstAssetName{ cfg.GetCString("firstAssetName") };
			const char* endAssetName{ cfg.GetCString("endAssetName")};
			if (!firstAssetName || !*firstAssetName) {
				throw std::runtime_error(std::format("Missing firstAssetName for {}", dirname));
			}

			names.InitMap(GetScanContainer(), firstAssetName, endAssetName);
		}

		// Get a pointer from a scan
		template<typename Type = void*>
		Type GetPointer(const char* id, const char* parent = BASE_PARENT) {
			ScanData data{ GetScan(id, parent) };
			hook::scan_container::ScanContainer& scan{ GetScanContainer() };
			bool nullName{ data.name[0] == '$' };

			if (data.type == SCT_OFFSET) {
				return (Type)scan.GetLibrary().Get<void>(data.offset + data.postOffset);
			}

			hook::library::ScanResult res{
				data.single ? scan.ScanSingle(data.path.data(), nullName ? nullptr : data.name.data())
				: scan.ScanAny(data.path.data(), nullName ? nullptr : data.name.data())
			};
			if (nullName && !res.location) {
				LOG_ERROR("Can't find pattern {}::{}.{} : {}", dirname, parent, id, data.name);
			}

			switch (data.type) {
			case SCT_ABSOLUTE: return res.GetPtr<Type>(data.offset + data.postOffset);
			case SCT_RELATIVE: return res.GetRelative<int32_t, Type>(data.offset, data.postOffset);
			case SCT_GET_OFFSET32:return (Type)scan.GetLibrary().Get<Type>(res.Get<uint32_t>(data.offset) + data.postOffset);
			default: throw std::runtime_error(std::format("SCAN DATA TYPE NOT IMPLEMENTED : {}", (int)data.type));
			}
		}

		// Get a pointers array from a scan
		template<typename Type = void*>
		std::vector<Type> GetPointerArray(const char* id, const char* parent = BASE_PARENT) {
			ScanData data{ GetScan(id, parent) };
			hook::scan_container::ScanContainer& scan{ GetScanContainer() };

			if (data.type == SCT_OFFSET) {
				return { (Type)scan.GetLibrary()[data.offset + data.postOffset] };
			}

			std::vector<hook::library::ScanResult> sres{ scan.Scan(data.path.data(), data.name[0] == '$' ? nullptr : data.name.data()) };

			if (data.single && sres.size() > 1) {
				throw std::runtime_error(std::format("Too many result for scan {}::{}", dirname, id));
			}

			std::vector<Type> res{};
			res.resize(sres.size());

			for (size_t i = 0; i < sres.size(); i++) {
				switch (data.type) {
				case SCT_ABSOLUTE:
					res[i] = sres[i].GetPtr<Type>(data.offset + data.postOffset);
					break;
				case SCT_RELATIVE:
					res[i] = sres[i].GetRelative<int32_t, Type>(data.offset, data.postOffset);
					break;
				case SCT_GET_OFFSET32:
					res[i] = (Type)scan.GetLibrary().Get<Type>(sres[i].Get<uint32_t>(data.offset) + data.postOffset);
					break;
				default: throw std::runtime_error(std::format("SCAN DATA TYPE NOT IMPLEMENTED : {}", (int)data.type));
				}
			}

			return res;
		}
	};
}