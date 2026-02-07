#pragma once
#include <core/config.hpp>
#include <hook/scan_container.hpp>

namespace acts::scan_dir {
	std::filesystem::path GetBaseDir();

	enum ScanType {
		TYPE_UNKNOWN = 0,
		TYPE_RELATIVE,
		TYPE_ABSOLUTE,
	};

	struct ScanData {
		std::string name;
		std::string path;
		std::string ctype;
		size_t offset{};
		ScanType type;
		bool single{};
	};

	class ScanDir {
	public:
		core::config::Config cfg;
		const char* dirname;
		hook::scan_container::ScanContainer& scan;

		ScanDir(const char* dirname, hook::scan_container::ScanContainer& scan);
		ScanDir(const ScanDir& other) = delete;
		ScanDir(ScanDir&& other) = delete;


		ScanData GetScan(const char* id);

		template<typename Type = void*>
		Type GetPointer(const char* id) {
			ScanData data{ GetScan(id) };

			hook::library::ScanResult res{
				data.single ? scan.ScanSingle(data.path.data(), data.name[0] == '$' ? nullptr : data.name.data())
				: scan.ScanAny(data.path.data(), data.name[0] == '$' ? nullptr : data.name.data())
			};

			switch (data.type) {
			case TYPE_ABSOLUTE: return res.GetPtr<Type>(data.offset);
			case TYPE_RELATIVE: return res.GetRelative<int32_t, Type>(data.offset);
			default: throw std::runtime_error(std::format("SCAN DATA TYPE NOT IMPLEMENTED : {}", (int)data.type));
			}
		}

		template<typename Type = void*>
		std::vector<Type> GetPointerArray(const char* id) {
			ScanData data{ GetScan(id) };

			std::vector<hook::library::ScanResult> sres{ scan.Scan(data.path.data(), data.name[0] == '$' ? nullptr : data.name.data()) };

			if (data.single && sres.size() > 1) {
				throw std::runtime_error(std::format("Too many result for scan {}::{}", dirname, id));
			}

			std::vector<Type> res{};
			res.resize(sres.size());

			for (size_t i = 0; i < sres.size(); i++) {
				switch (data.type) {
				case TYPE_ABSOLUTE:
					res[i] = sres[i].GetPtr<Type>(data.offset);
					break;
				case TYPE_RELATIVE:
					res[i] = sres[i].GetRelative<int32_t, Type>(data.offset);
					break;
				default: throw std::runtime_error(std::format("SCAN DATA TYPE NOT IMPLEMENTED : {}", (int)data.type));
				}
			}

			return res;
		}
	};
}