#pragma once
#include <core/config.hpp>
#include <hook/scan_container.hpp>
#include <deps/idc_builder.hpp>

namespace acts::game_data {
	std::filesystem::path GetBaseDir();

	enum CTypeType {
		CTT_UNKNOWN = 0,
		CTT_STRUCT,
		CTT_ENUM,
	};

	enum ScanType {
		SCT_UNKNOWN = 0,
		SCT_RELATIVE,
		SCT_ABSOLUTE,
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
	public:
		core::config::Config cfg;
		const char* dirname;
		hook::scan_container::ScanContainer* scan{};

		GameData(const char* dirname);
		GameData(const GameData& other) = delete;
		GameData(GameData&& other) = delete;

		void AddTypesToIdc(deps::idc_builder::IdcBuilder& builder);
		void ScanAllToIdc(deps::idc_builder::IdcBuilder& builder);
		void ApplyNullScans(const char* id);
		ScanData GetScan(const char* id, const char* parent = "scans");
		void SetScanContainer(hook::scan_container::ScanContainer* container) { scan = container; }
		hook::scan_container::ScanContainer& GetScanContainer();

		template<typename Type = void*>
		Type GetPointer(const char* id, const char* parent = "scans") {
			ScanData data{ GetScan(id, parent) };
			hook::scan_container::ScanContainer& scan{ GetScanContainer() };
			hook::library::ScanResult res{
				data.single ? scan.ScanSingle(data.path.data(), data.name[0] == '$' ? nullptr : data.name.data())
				: scan.ScanAny(data.path.data(), data.name[0] == '$' ? nullptr : data.name.data())
			};

			switch (data.type) {
			case SCT_ABSOLUTE: return res.GetPtr<Type>(data.offset + data.postOffset);
			case SCT_RELATIVE: return res.GetRelative<int32_t, Type>(data.offset, data.postOffset);
			default: throw std::runtime_error(std::format("SCAN DATA TYPE NOT IMPLEMENTED : {}", (int)data.type));
			}
		}

		template<typename Type = void*>
		std::vector<Type> GetPointerArray(const char* id, const char* parent = "scans") {
			ScanData data{ GetScan(id, parent) };
			hook::scan_container::ScanContainer& scan{ GetScanContainer() };
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
				default: throw std::runtime_error(std::format("SCAN DATA TYPE NOT IMPLEMENTED : {}", (int)data.type));
				}
			}

			return res;
		}
	};
}